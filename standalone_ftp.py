
from ftplib import FTP
from os.path import isfile
import sys

def print_usage():
    """Print usage"""
    print(f"Usage: {sys.argv[0]} ip file <get|set>")
    print("ip        = your console's ip, like 192.168.0.123")
    print("file      = file name of the input for set, or output for get")
    print("<get|set> = get: download the latest save state from switch")
    print("            set: send input file to switch for restoring save state")
    print()
    print(f"Example: {sys.argv[0]} 192.168.0.123 inside_bombs.txt set")
    print("          ^ This will send inside_bombs.txt to your switch for restoring")
    print(f"Example: {sys.argv[0]} 192.168.0.123 output_state.txt get")
    print("          ^ This will download the last save state from switch and save it as output_state.txt on your computer")
def run(console_ip, local_path, command):
    """Main"""

    if '.' not in console_ip:
        print("Invalid IP:", console_ip)
        print_usage()
        sys.exit(-1)

    console_port = 5000

    ftpw = FtpWrapper(console_ip, console_port)

    if command in ("get", "g"):
        ftpw.connect()
        get(ftpw, local_path)
    elif command in ("set", "s"):
        ftpw.connect()
        set(ftpw, local_path)
    else:
        print("Unknown command:", command)
        print_usage()
        sys.exit(-1)
# Deploy
def set(ftpw, local_path):
    """Send save state to console"""
    target_path = "botwsavs/restore.txt"
    ftpw.send_file(local_path, target_path)

# Clean
def get(ftpw, local_path):
    """Remove deployment on console"""
    target_path = "botwsavs/latest.txt"
    ftpw.retrive_file(local_path, target_path)

class FtpWrapper:
    """Wrapper for ftp"""
    def __init__(self, host, port):
        """Constructor"""
        self.ftp = FTP()
        self.host = host
        self.port = port

    def connect(self):
        """Connect"""
        print(f'Connecting to {self.host}:{self.port}... ', end='')
        self.ftp.connect(self.host, self.port)
        print('Connected!')

    def send_file(self, local_path, sd_path):
        """Send local_path to sd_path"""
        sd_path = add_slash_if_need(sd_path)
        if not isfile(local_path):
            print("Error: ", local_path, " does not exist or is not a file")
            sys.exit(-1)
        print(f"Sending {local_path}")
        print(f'> STOR {sd_path}')
        with open(local_path, 'rb') as local_file:
            self.ftp.storbinary(f'STOR {sd_path}', local_file)

    def retrive_file(self, local_path, sd_path):
        """Download sd_path to local_path"""
        sd_path = add_slash_if_need(sd_path)
        print(f"Receiving {local_path}")
        with open(local_path, "wb+") as file:
            self.ftp.retrbinary(f"RETR {sd_path}", file.write)
            print(f"> RETR {sd_path}")

def add_slash_if_need(path):
    """ a/b/c -> /a/b/c"""
    return path if path.startswith("/") else "/"+path

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print_usage()
        sys.exit(-1)
    run(sys.argv[1], sys.argv[2], sys.argv[3])
