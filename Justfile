
CHANGELOG_LATEST_LAST := `grep -n "###" CHANGELOG.md | cut -d ":" -f 1 | tr "\n" " " | cut -d " " -f 2`
VERSION_TEXT := ```
    grep LATEST CHANGELOG.md | cut -d "\`" -f 2
```
# ``` 

# Lint .cpp, .hpp and .py files. Pass --verbose to see more output
lint VERBOSE="": (lintpy VERBOSE) (lintc VERBOSE)

lintpy VERBOSE="":
    python3 tools/lint.py {{VERBOSE}}
    pylint tools

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
make EXTRA_DEFINES="":
    mkdir -p build{{EXTRA_DEFINES}}
    make -C build{{EXTRA_DEFINES}} -f ../build.mk EXTRA_DEFINES={{EXTRA_DEFINES}}
    python3 tools/ips.py

build: (make "-DDEBUG")

release EXTRA_DEFINES="":
    @just clean
    @just make {{EXTRA_DEFINES}}
    mkdir -p release/atmosphere/contents/01007EF00011E000/exefs
    mkdir -p release/atmosphere/contents/01007EF00011E000/romfs/System
    mkdir -p release/atmosphere/exefs_patches/botwsavs
    mkdir -p release/botwsavs
    cp build/botwsavs.nso release/atmosphere/contents/01007EF00011E000/exefs/subsdk9
    cp build/app.npdm release/atmosphere/contents/01007EF00011E000/exefs/main.npdm
    cp build/8E9978D50BDD20B4C8395A106C27FFDE.ips release/atmosphere/exefs_patches/botwsavs/8E9978D50BDD20B4C8395A106C27FFDE.ips
    cp README.md release
    cp CHANGELOG.md release
    echo "" > release/botwsavs/main.log
    echo "" > release/botwsavs/latest.txt
    if {{EXTRA_DEFINES}} == "-DGOLD_RUSH" { export GR_SYMBOL="-GR" } else { export GR_SYMBOL="" }
    echo {{VERSION_TEXT}}$GR_SYMBOL > release/atmosphere/contents/01007EF00011E000/romfs/System/Version.txt
    zip -r save-state-{{VERSION_TEXT}}$GR_SYMBOL.zip release

release-gold-rush: (release "-DGOLD_RUSH")

publish:
    @echo
    @echo "Latest Version: v{{VERSION_TEXT}}"
    @echo
    @read -p "Press Enter to publish this version to github release " output
    git tag v{{VERSION_TEXT}}
    git push origin v{{VERSION_TEXT}}

draft-release:
    sed -n $(grep -n "LATEST" CHANGELOG.md | cut -d ":" -f 1)',{{CHANGELOG_LATEST_LAST}}p;{{CHANGELOG_LATEST_LAST}}q' CHANGELOG.md | head -n -1

clean:
    rm -rf build build-DDEBUG build-DGOLD_RUSH release

ftp COMMAND="install":
    echo "" > temp.txt
    python3 tools/ftp.py {{COMMAND}}
    rm -f temp.txt
