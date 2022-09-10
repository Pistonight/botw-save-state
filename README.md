# botw-save-state
A BOTW 1.6.0 Switch save state mod for speedrun practices

## Install and Uninstall
### Install
1. Download and extract the zip from [latest release](https://github.com/iTNTPiston/botw-save-state/releases/latest). There should be 2 folders `atmosphere` and `botwsavs`
2. Copy both folders to the root of your SD card. It's safe to overwrite existing files if the only mod you have is the save state. There's no guarantee that other mods work with this

### Uninstall
Remove these folders from the SD card if they exist
  - `/atmosphere/contents/01007EF00011E000/exefs`
  - `/atmosphere/contents/01007EF00011E000/romfs`
  - `/atmosphere/exefs_patches/botwsavs`
  - `/botwsavs`

## Usage (Key Combos)

**Make sure you are only activating the key combos while having control of Link. The game may bahave weirdly or even crash if you press them in loading screen or title screen**

### Active and Setting Mode
There are 2 modes in the save state mod, the Active Mode and the Setting Mode.
- Active Mode: Can save and restore state
- Setting Mode: Can change the level of save state

To switch between the 2 modes. Hold `Dpad Down` (whistle) and all 4 triggers for 3 seconds. You will see a message displayed like this when the mode is switched
```
Save state: Active Mode
```

### Active Mode Combos
You can use these key combos in the Active Mode.

- `ZL + L + PLUS + Dpad Left`: Save state to memory
- `ZL + L + PLUS + R3 + Dpad Left`: Save state to `/botwsavs/latest.txt` on sd card
- `ZL + L + PLUS + Dpad Right`: Restore state from memory
- `ZL + L + PLUS + R3 + Dpad Right`: Restore state from `/botwsavs/restore.txt` on sd card

Note that for save to/restore from file, you can't press `R3` last. Otherwise you would trigger save to/restore from memory instead. I usually prefer holding `Triggers + Dpad + R3`, then hit `PLUS` to trigger.

Also note that the mod has a logging function that will block reading the `/botwsavs` folder when the game is running, if you read it from a ftp client like FileZilla. If you use the [script](#save-state-transfer-script) it will be fine.

If you try to restore without saving, or without the right file on sd card. You will see a message like this
```
You can't do that right now
```

If you see a message like this:
```
Pointer error!
```
This means one or more of the values cannot be accessed. Mostly likely due to invalid memory pointer. Currently it's known that there's a small chance that Link's havok coordinates point to invalid memory. Try again or restart the game if this happens.

### Setting Mode Combos

In setting mode. You can switch between levels of save state. 

- Hold `L` for 1 second: Decrease level
- Hold `R` for 1 second: Increase level
- Hold save/restore to memory/file key binding for 3 seconds: Change the key binding (Hold the new keys for 3 seconds to change)

The settings are preserved through restarts. For example, if you close the game on a level, you will be on the same level when you boot up the game again.

If you somehow messed up the settings and want to reset them, you can do so by deleting `/botwsavs/worker.txt` file on your sd card.

### Levels

There are currently 4 levels:

#### Level 0: Disabled 
The key combos in Active Mode will be disabled in this level

#### Level 1: Basic
Level 1 save state includes:
- Health
- Stamina
- Position: Coordinate, Facing Angle, Camera Angle
- Runes (finishes cooldown on restore)
#### Level 2: Basic + Durability
Level 2 save state includes everything in Level 1 and equipped Arrow count + Weapon/Bow/Shield Durability

You will see a warning like below if you don't have something equipped, but the ones you do have equipped will still save/restore fine
```
Warning: Something not equipped?
```
When restoring, the values will only be restored if you are equipping the same item as when you saved. Otherwise you will see a warning message like below. The items that do match will still restore fine.
```
Warning: Some equipped items are different!
```
#### Level 3: All
Level 3 save state includes everything available, which includes all from Level 1 and 2, plus:
- Time of day values:
  - Including the current time of day and blood moon timer
- Climate damage timers:
  - Time until next heat/cold damage
  - Time until Link starts burning
- Champion abilities:
  - Cool down for all abilities
  - Uses left for gale, fury, and protection
  - Master sword cool down
- Potion timer:
  - Speed
  - Attack
  - Defense
  - Heat Resist
  - Cold Resist
  - Flame Resist
  - Shock Resist
  - Stealth
- Inventory offset:
  - i.e. the number of broken slots

### Save/Restore with Different Levels
If you save a state with a high level (for example, level 3), then switch to a lower level (for example, level 1) and restore, only the lower level values will be restored. However, the state still contains the higher level data. You can switch back to the higher level and be able to restore all the values

If you save a state with a low level (for example, level 1), then switch to a higher level (for example, level 3) and restore, you will see an error message like:
```
You need to lower the setting level to restore!
```

### Change Key Binding

You can change the key binding for save/restore following these steps:
1. Hold all triggers + whistle for 3 seconds to enter setting mode
2. Hold the current key binding for the one you want to change for 3 seconds
3. Let go of all buttons. You will see a message telling you to hold the new binding for 3 seconds
4. Hold the new binding for 3 seconds and you will see a confirmation message
5. Hold all triggers + whistle for 3 seconds to exit setting mode

### FTP Transfer Script
A python script `ftp.py` is also included for transfering save state files over FTP (with the `ftpd` homebrew app). Run `python3 ftp.py` to see how to use. 

(This has nothing to do in game, just makes managing the save state files generated by the mod easier)

## Developer
**This section is intented for developers**

### Environment
#### OS
You need a linux or WSL environment. Mac probably work as well.
#### DevkitPro
For Debian based systems (linux)
```
wget https://apt.devkitpro.org/install-devkitpro-pacman
chmod +x ./install-devkitpro-pacman
sudo ./install-devkitpro-pacman
sudo dkp-pacman -S switch-dev
```
For other systems follow https://devkitpro.org/wiki/Getting_Started (need `switch-dev` package)
#### Just
Just is used to run command shortcuts. https://github.com/casey/just

Cargo install is recommended:
```
cargo install just
```
If you don't have rust installed: https://rustup.rs/
#### Python 
Need Python 3. (3.10 is preferred but other version probably works as well).

Install these packages
```
python3 -m pip install toml pylint pylint-quotes keystone-engine
```
#### clang-format
clang-format-12 is required. Run this on linux or the mac-equivalent on mac
```
sudo apt install clang-format-12
```

### Commands

Run `just --list` for a full list of commands.

Command workflows:
- `just build` to build the (debug) mod files
- `just ftp` to send the debug build to your switch over FTP
  - One time setup: Run `just set-ip <IP>` to set your console IP
- `just lint` before PR/push

Release workflow:
- Make sure `CHANGELOG.md` is up to date
- `just release` to build for release configuration and produce the release zip
  - `just release-gold-rush` to build for gold rush configuration
- `just publish` to tag the current commit with latest version
- Upload the built release zip to GitHub manually
- `just clean`

### Linking BOTW
Follow these steps to statically link a botw symbol
- Include the symbol in the project
  - If the symbol is in decomp project, copy over the headers to `lib/botw`
  - Otherwise, declare the symbol (data or function) in `mem/KingPtr.hpp`. Put the address in a comment
- Use the symbol as normal (i.e. call the function or access the data)
- Build with `just build`
- Find the mangled symbol with `just find-symbol <symbol>`
  - `<symbol>` is a search string that will be used to `grep` the symbol table
- If the symbol is not in decomp project, copy the mangled symbol and put in in a comment in `mem/KingPtr.hpp`
- Add the symbol `just add-symbol <symbol> <address>`
- Verify that `just find-symbol <symbol>` cannot find the symbol
