#pylint: disable=import-error
"""Generate ips patch"""
from os.path import isfile, isdir, join
from os import listdir
import enum
import sys
import re
import ctypes
import struct
import keystone
import toml

# consts
SRC = "patch"
PATCH_EXTENSION = ".patch.s"
MOD_NAME = "botwsavs"

NSO_HEADER_LEN = 0x100

IPS_FORMAT = ".ips"
IPS_HEADER_MAGIC = bytes("IPS32", 'ASCII')
IPS_EOF_MAGIC = bytes("EEOF", 'ASCII')

WORKSPACE_TOML = "config/workspace.toml"

BUILD_DIR = "build"
def read_config():
    """Read config from workspace.toml"""
    # pylint: disable-next=global-statement
    global BUILD_DIR

    if len(sys.argv) > 1:
        BUILD_DIR = sys.argv[1]

    if isfile(WORKSPACE_TOML):
        with open(WORKSPACE_TOML, "r", encoding="utf-8") as config_file:
            return toml.load(config_file)["ips"]
    print("Cannot read config")
    sys.exit(-1)

class Patch:
    """Data for patch. offset and byte array content"""
    def __init__(self, offset, content):
        self.offset = offset + NSO_HEADER_LEN
        self.content = content

    def __str__(self):
        return f"{hex(self.offset)}:{len(self.content)}:{self.content.hex()}"

    def __repr__(self) -> str:
        return str(self)

class PatchType(enum.Enum):
    """Enum for patch type"""
    UNKNOWN = 0
    BYTES = 1
    STRING = 2
    ASSEMBLY = 3

def run():
    """Main"""
    ips_config = read_config()
    if ips_config is None:
        print("Error: cannot read ips config")
        sys.exit(-1)

    # build_id: string -> Patch[]
    id_to_patchlist = {}
    mod_offset = int(ips_config["mod_offset"], 16)
    fail_count = process_path(SRC, mod_offset, ips_config, id_to_patchlist)
    if fail_count > 0:
        print("Failed to compile ips patches")
        sys.exit(fail_count)

    for build_id, patch_list in id_to_patchlist.items():
        #print(id)
        ips_path = join(BUILD_DIR, build_id + IPS_FORMAT)
        with open(ips_path, 'wb') as ips_file:
            ips_file.write(IPS_HEADER_MAGIC)
            for patch in patch_list:
                #print(patch)
                ips_file.write(struct.pack('>I', patch.offset))
                ips_file.write(struct.pack('>H', len(patch.content)))
                ips_file.write(patch.content)
            ips_file.write(IPS_EOF_MAGIC)
        print("Generated", ips_path)

def process_path(path, mod_offset, ips_config, id_to_patchlist):
    """
        Recursively compiles the all source patches in directory
        See compile_patch
        Return the number of files that failed to compile
    """

    if isfile(path):
        if path.endswith(PATCH_EXTENSION):
            return 0 if compile_patch(path, mod_offset, ips_config, id_to_patchlist) else 1
        return 0

    if isdir(path):
        fail_count = 0
        for sub_path in listdir(path):
            fail_count += process_path( \
                join(path, sub_path), mod_offset, ips_config, id_to_patchlist)
        return fail_count

    return 0

def compile_patch(path, mod_offset, ips_config, id_to_patchlist):
    """
        Compile the patch.
        Adds the patch to patch list if successful.
        Returns whether the compilation was successful

        path             File path
        mod_offset       The offset for mod relative to start of main,
                         used for calculating branches in asm patches
        ips_config       The ips config, used to read offsets for targets
                         (i.e. ips_config[target]["offset"]
        id_to_patchlist  The map (string -> Patch[]) to add the patches to
    """

    target = None
    mod_offset_from_target = 0
    # int -> bytearray
    target_to_addr_to_patchbytes = {}
    address = 0
    patch_type = PatchType.UNKNOWN

    with open(path, "r", encoding="utf-8") as patch_file:
        for i, line in enumerate(patch_file):
            # Strip off comments
            line = line.split(';', 1)[0].strip()
            # Skip empty lines
            if len(line) == 0:
                continue

            # Process directives
            if line.startswith(".target "):
                target = line.split(' ', 1)[1].strip()
                target_offset = int(ips_config[target]["offset"], 16)
                mod_offset_from_target = mod_offset - target_offset
                continue
            if line.startswith(".asm"):
                patch_type = PatchType.ASSEMBLY
                continue
            if line.startswith(".bytes"):
                patch_type = PatchType.BYTES
                continue
            if line.startswith(".string"):
                patch_type = PatchType.STRING
                continue

            # parse patches
            if target is None:
                print_build_error(path, i, "Target not specified")
                return False
            # if address starts with 0x, set the address
            if line.startswith("0x"):
                address_split = line.split(' ', 1)

                address = int(address_split[0], 16)
                if len(address_split) < 2:
                    continue
                line = address_split[1]

            if patch_type is None or patch_type == PatchType.UNKNOWN:
                print_build_error(path, i, "Patch type not specified. Use \
.asm, .bytes or .string to specify the type of the patch")
                return False
            patch_content = \
                produce_patch_content(patch_type, line, address, mod_offset_from_target)

            if patch_content is None:
                print_build_error(path, i, "Failed to compile")
                return False

            add_patch_line(target_to_addr_to_patchbytes, target, address, patch_content)

    add_to_patch_map(target_to_addr_to_patchbytes, id_to_patchlist, ips_config)

    print(f"Compiled {path}")
    return True

def produce_patch_content(patch_type, line, address, mod_offset_from_target):
    """ Produce patch bytes according to type, or None if type is not valid"""
    if patch_type == PatchType.BYTES:
        return gen_bytes_patch_bytes(line)
    if patch_type == PatchType.STRING:
        return gen_string_patch_bytes(line)
    if patch_type == PatchType.ASSEMBLY:
        return gen_asm_patch_bytes(address, line, mod_offset_from_target)

    return None

def add_patch_line(target_to_addr_to_patchbytes, target, address, patch_content):
    """ Add patch_content to the map"""
    if target not in target_to_addr_to_patchbytes:
        target_to_addr_to_patchbytes[target] = {}
    addr_to_patchbytes = target_to_addr_to_patchbytes[target]
    if address not in addr_to_patchbytes:
        addr_to_patchbytes[address] = patch_content
    else:
        addr_to_patchbytes[address] = addr_to_patchbytes[address] + patch_content

def add_to_patch_map(target_to_addr_to_patchbytes, id_to_patchlist, ips_config):
    """ Optimize the compiled patch pieces and add to patch list """
    # Generate a list of patches
    for tar in target_to_addr_to_patchbytes:
        addr_to_patchbytes = target_to_addr_to_patchbytes[tar]
        build_id = ips_config[tar]["id"]
        if build_id not in id_to_patchlist:
            id_to_patchlist[build_id] = []
        patchlist = id_to_patchlist[build_id]
        # Optimize patches
        start_addr = None
        current_addr = None
        current_patch = None
        for addr in sorted(addr_to_patchbytes):
            if current_addr == addr:
                # The patches are next to each other, we can just append
                next_patch = addr_to_patchbytes[addr]
                current_patch = current_patch+next_patch
                current_addr = current_addr+len(next_patch)
            else:
                if start_addr is not None:
                    patchlist.append(Patch(start_addr, current_patch))
                next_patch = addr_to_patchbytes[addr]
                start_addr = addr
                current_addr = addr+len(next_patch)
                current_patch = next_patch
        if start_addr is not None:
            patchlist.append(Patch(start_addr, current_patch))

def gen_bytes_patch_bytes(hex_str):
    """ Return bytes """
    try:
        return bytearray.fromhex(hex_str)
    except ValueError:
        print(f"Error: invalid bytes format: {hex_str}")
        return None

def gen_string_patch_bytes(content):
    """ Return bytes containing utf-8 string """
    match = re.search(r'"(.+)"', content)
    if match:
        return bytearray(bytes(match.group(1), 'utf-8').decode('unicode_escape') + '\0', 'utf-8')
    print(f"Error: invalid string format: {content}")
    return None

ASSEMBLER = keystone.Ks(keystone.KS_ARCH_ARM64, keystone.KS_MODE_LITTLE_ENDIAN)
def gen_asm_patch_bytes(address, assembly_code_str, mod_offset_from_target):
    """
        Return bytes containing code, or None if an error occurs
        address                      Address of the instruction,
                                     relative to start of target
        assembly_code_str            Code
        mod_offset_from_target       Offset from target binary to mod
    """
    # asm patch
    bl_match = re.match(r'([Bb][Ll]?\s+)([^\#]+$)', assembly_code_str)
    if bl_match:
        branch_instruction = bl_match.group(1)
        branch_symbol = bl_match.group(2)
        branch_offset = resolve_bl_symbol(address,mod_offset_from_target, branch_symbol)
        if branch_offset is None:
            print(f"^ Failed to assemble instruction: {assembly_code_str}")
            return None
        assembly_code_str = f"{branch_instruction}#{hex(branch_offset)}"

    encoded_bytes, _ = ASSEMBLER.asm(assembly_code_str)
    return bytearray(encoded_bytes)

def print_build_error(file, line_num, error):
    """ Print build error """
    print(f"Error in {file}:{line_num}: {error}")

def resolve_bl_symbol(branch_address, mod_offset_from_target, branch_symbol):
    """
        Return the offset for branching

        branch_address               Where the bl instruction is, relative to start of target
        mod_offset_from_target       Offset from target binary to mod
        branch_symbol                The symbol to resolve;
                                     can be a symbol in mod or address
                                     relative to the load offset
    """

    target_address = 0 # relative to start of target
    # resolve offset
    plus_split = branch_symbol.split('+', 1)
    if len(plus_split) > 1:
        branch_symbol = plus_split[0]
        target_address += int(plus_split[1], 16)

    # If the symbol is an address
    if branch_symbol.startswith("0x"):
        target_address += int(branch_symbol, 16)
        return target_address - branch_address

    # If not, try searching the symbol in mod
    symbol_address = search_mod_symbol_addr(branch_symbol)
    if symbol_address is not None:
        target_address += symbol_address
        target_address += mod_offset_from_target
        return target_address - branch_address

    print(f"^ Failed to resolve branch symbol: {branch_symbol}")
    return None

# Memoization
BOTW_IGD_SYMBOL_MAP = None
BOTW_IGD_SYMBOL_CACHE = {}
def search_mod_symbol_addr(symbol):
    """
        Return the address of the symbol in mod, relative to start of mod
        If not found, None is returned
        The result is memoized
    """
    # pylint: disable-next=global-statement
    global BOTW_IGD_SYMBOL_MAP
    if symbol in BOTW_IGD_SYMBOL_CACHE:
        return BOTW_IGD_SYMBOL_CACHE[symbol]
    # Lazy load symbol map
    if BOTW_IGD_SYMBOL_MAP is None:
        with open(join(BUILD_DIR, f"{MOD_NAME}.map"), 'r', encoding="utf-8") as map_file:
            BOTW_IGD_SYMBOL_MAP = map_file.read()

    func_search_regex = symbol + r'\('
    pos, address =  find_symbol_address(func_search_regex, 0, BOTW_IGD_SYMBOL_MAP)
    if pos == -1:
        print(f"Error: Cannot find {MOD_NAME} symbol: {symbol}")
        return None
    # check uniqueness
    while True:
        pos, other_address = find_symbol_address(func_search_regex, pos, BOTW_IGD_SYMBOL_MAP)
        if pos == -1:
            break

        if other_address != address:
            print(f"Error: Ambiguous {MOD_NAME} symbol: {symbol}")
            return None

    BOTW_IGD_SYMBOL_CACHE[symbol] = address
    return ctypes.c_long(address).value

def find_symbol_address(func_search_regex, start_pos, map_file_str):
    """
        Find in map_file_str
        Returns <next_search_pos>, <address> if found, or -1,-1 if not found
    """

    regex_match = re.search(func_search_regex, map_file_str[start_pos:])
    if not regex_match:
        return -1, -1

    found_line_pos = map_file_str.rfind('\n', 0, regex_match.span()[0] + start_pos) + 1
    found_line_end_pos = map_file_str.find('\n', regex_match.span()[1] + start_pos)
    found_line = map_file_str[found_line_pos:found_line_end_pos].strip()
    found_addr = int(found_line[:found_line.find(" ")].replace("00000000:00000071", ''), 16)
    return found_line_end_pos, found_addr

if __name__ == "__main__":
    run()
