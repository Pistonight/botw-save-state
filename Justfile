set dotenv-load
set shell := ["bash", "-uc"]

CHANGELOG_LATEST_LAST := `grep -n "###" CHANGELOG.md | cut -d ":" -f 1 | tr "\n" " " | cut -d " " -f 2`
VERSION_TEXT := ```
    grep LATEST CHANGELOG.md | cut -d "\`" -f 2
```
# ``` 

# Show help
default:
    @just --list

# Lint .cpp, .hpp and .py files. Pass --verbose to see more output
lint VERBOSE="": (lintpy VERBOSE) (lintc VERBOSE)

# Lint text files and python scripts
lintpy VERBOSE="":
    python3 tools/lint.py {{VERBOSE}}
    pylint tools

# Lint Cpp files with clang-format
lintc VERBOSE="":
    clang-format-12 -n -Werror {{VERBOSE}} $(find src -name *.hpp)
    clang-format-12 -n -Werror {{VERBOSE}} $(find src -name *.cpp) 

# Run clang-format on source files
format VERBOSE="":
    @echo "Checking .hpp"
    clang-format-12 -i {{VERBOSE}} $(find src -name *.hpp)
    @echo "Checking .cpp"
    clang-format-12 -i {{VERBOSE}} $(find src -name *.cpp) 

# Run make
_make EXTRA_DEFINES="":
    mkdir -p build{{EXTRA_DEFINES}}
    make -C build{{EXTRA_DEFINES}} -f ../build.mk EXTRA_DEFINES={{EXTRA_DEFINES}}
    python3 tools/ips.py build{{EXTRA_DEFINES}}

# Build debug configuration
build: (_make "-DDEBUG")
    echo -n SS-DEBUG > build-DDEBUG/DebugVersion.txt

# Build release configuration and package
release EXTRA_DEFINES="": clean (_make EXTRA_DEFINES)
    mkdir -p release/atmosphere/contents/01007EF00011E000/exefs
    mkdir -p release/atmosphere/contents/01007EF00011E000/romfs/System
    mkdir -p release/atmosphere/exefs_patches/botwsavs
    mkdir -p release/botwsavs
    cp build/botwsavs.nso release/atmosphere/contents/01007EF00011E000/exefs/subsdk9
    cp build/app.npdm release/atmosphere/contents/01007EF00011E000/exefs/main.npdm
    cp build/8E9978D50BDD20B4C8395A106C27FFDE.ips release/atmosphere/exefs_patches/botwsavs/8E9978D50BDD20B4C8395A106C27FFDE.ips
    cp README.md release
    cp CHANGELOG.md release
    cp standalone_ftp.py release/ftp.py
    echo "" > release/botwsavs/latest.txt
    echo -n {{VERSION_TEXT}} > release/atmosphere/contents/01007EF00011E000/romfs/System/Version.txt
    cd release && zip -r ../save-state-{{VERSION_TEXT}}.zip *

# Build gold rush configuration and package
release-gold-rush: (release "-DGOLD_RUSH")

# Create git tag and push
publish:
    @echo
    @echo "Latest Version: v{{VERSION_TEXT}}"
    @echo
    @read -p "Press Enter to tag this commit " output
    git tag v{{VERSION_TEXT}}
    git push origin v{{VERSION_TEXT}}

# Display draft release notes
draft-release:
    @sed -n $(grep -n "LATEST" CHANGELOG.md | cut -d ":" -f 1)',{{CHANGELOG_LATEST_LAST}}p;{{CHANGELOG_LATEST_LAST}}q' CHANGELOG.md | head -n -1

# Clean output
clean:
    rm -rf build build-DDEBUG build-DGOLD_RUSH release save-state-*.zip

nuke: clean
    rm -rf .env

# Set console IP
set-ip IP="":
    @export SWITCH_CONSOLE_IP={{IP}} 
    @echo SWITCH_CONSOLE_IP={{IP}} > .env 
    @echo "Done"

# Run ftp util. See tools/ftp.py
ftp COMMAND="install":
    @if [[ -z ${SWITCH_CONSOLE_IP:+x} ]] ; then python3 tools/ftp.py {{COMMAND}}; else python3 tools/ftp.py {{COMMAND}} $SWITCH_CONSOLE_IP; fi

# Run standalone ftp util wrapper for sending restore state.
send-restore:
    @if [[ -z ${SWITCH_CONSOLE_IP:+x} ]] ; then python3 tools/ftp.py install; else python3 standalone_ftp.py $SWITCH_CONSOLE_IP runtime/restore.txt set ; fi

# Run standalone ftp util wrapper for getting latest state.
get-latest:
    @if [[ -z ${SWITCH_CONSOLE_IP:+x} ]] ; then python3 tools/ftp.py install; else python3 standalone_ftp.py $SWITCH_CONSOLE_IP runtime/latest.txt get ; fi

# Search for symbol in dumpped (debug) build
find-symbol SYMBOL:
    @objdump -T build-DDEBUG/botwsavs.elf | grep {{SYMBOL}} | cut -c 49-

# Add symbol to config/symbols.ld and relink
add-symbol SYMBOL ADDRESS: && build
    echo "$(just find-symbol {{SYMBOL}}) = {{ADDRESS}} - 0x3483000; /* {{SYMBOL}} */" >> config/symbols.ld
    rm build-DDEBUG/botwsavs.*
