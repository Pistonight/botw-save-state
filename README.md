# botw-save-state
A BOTW 1.6.0 Switch save state mod for speedrun practices

## Install
First, download and extract the zip from [latest release](https://github.com/Pistonight/botw-save-state/releases/latest). There should be 2 folders `mods` and `botwsavs`.

I recommend using [Simple Mod Manager](https://github.com/nadrino/SimpleModManager) for installing the mod. It will allow you easily disable/enable it.
The downloaded package is already formatted and tested with Simple Mod Manager.

1. Install Simple Mod Manager.
2. Copy the `mods` and `botwsavs` folders to the root of your SD card.
3. Launch Simple Mod Manager, Select BOTW.
4. There should be a `Save State` mod in the mod list. Enable it.

You can also install it without SMM: Navigate to the `atmosphere` folder inside the `mods` folder and copy it to the root of the SD card.
The mod will be automatically active.

In either case, make sure all other mods are uninstalled/disabled, as there is practically no way this mod will work with other mods.

## Disable/Uninstall
If you use Simple Mod Manager, you can disable it within the tool. It will allow you to easily enable it again.

Otherwise, delete or rename `/atmosphere/contents/01007EF00011E000` from the SD card.
To fully delete the mod from existence from your device, also delete the `/botwsavs` folder from the SD card. This will remove config files, etc.

## Tutorial
Launch the game with the mod installed. You should the version in the title screen change to `1.6.0-SSx.y` where `x.y` is the version of the mod.
There will also be a welcome message displayed when you load into a save. This way you can verify the mod is active.

### Save and Restore
Now you can press `ZL + L + Dpad Left + Plus` to save the state. I typically hold `ZL + L + Dpad Left` then press `Plus` and then release all buttons.
You should see a message like this
```
Saved state to memory
```

Then you can move around, and when you want to restore to the saved state, press `ZL + L + Dpad Right + Plus`.
You should teleport to the saved position with the message
```
Restored state from memory
```
**Make sure you are only activating the key combos while having control of Link. The game may behave weirdly or even crash if you press them in loading screen or title screen**

**New in 2.0**: As long as you don't release all the keys, the mod will keep resetting Link's position, allowing for a more accurate restore while in ragdoll state.
For example, with the default restore combo, I typically release the other 3 keys and hold `Plus` for slightly longer until position is fully restored.

### Settings
Hold all triggers (`ZL + L + ZR + R`) and the whistle button (`Dpad Down`) for 3 seconds to enter setting mode. You should see the setting menu pop up in the top-right corner.
Use the dpad to navigate the menu and `A` to select. Use `B` to go back and exit the settings.

#### Key Binding
You can change the key bindings for save/restore. Follow these steps:
1. Open setting menu (see above), then select `Change Key Binding`
2. Use dpad to select the key you want to change, then press `A`
3. Hold the new combo for 3 seconds, or hold `ZL` to cancel.
   - If you combo includes dpad or plus/minus, open the menu (plus) or map (minus) first, then hold the dpad direction, and then press and hold plus/minus to unpause and hold the other keys.
4. You should see a confirmation message when the new binding is set

#### State Options

Since 2.0, you can now toggle individually what you want to save/restore. Open the setting menu and select `Change State Options`.

**Recommended (TL;DR)**:
- Keep `Timers` enabled as they don't have negative effects
- If you don't need to track durability during practice (i.e. Any%), you can disable `Overworld Durability` and `Inventory` to avoid accidental durability transfer

1. `Restore Message`
    - When disabled, there will be no message displayed upon restoring. Useful for grinding BTT or if you think the message is annoying.
2. `Timers` enables
    - Time of day values:
      - Including the current time of day and blood moon timer
    - Climate damage timers:
      - Time until next heat/cold damage
      - Time until Link starts burning
    - Champion abilities:
      - Cool down for all abilities
      - Uses left for gale, fury, and protection
      - Master Sword cool down
    - Potion timer:
      - Speed
      - Attack
      - Defense
      - Heat Resist
      - Cold Resist
      - Flame Resist
      - Shock Resist
      - Stealth
3. `Overworld Durability`
   - When enabled, the name and durability of equipped Weapon, Bow and Shield will be saved.
   - When restoring, the durability of equipped Weapon/Bow/Shield will only be restored if the same item by name is equipped (not necessarily the same one).
   - Note that the inventory durability is not restored unless you also enable `Inventory`
4. `Inventory` enables:
   - Equipped durability of Weapon, Bow, Shield and Arrow (similar to `Overworld Durability`, only restored if the same item is equipped)
   - Number of offset slots for IST

The following will always be enabled, and you cannot turn them off:
- Current Health
- Current Stamina
- Position (Havok Position and Main Position Matrix)
- Camera Matrix

Additionally, the following will happen when you restore, regardless of the setting:
- Runes will finish cooldown
- Bombs will be deleted
    
### Save/Restore from File
In additional to saving the state to memory, you can also save it to a file. The default save key combo is `ZL + L + Dpad Left + R3` and restore is `ZL + L + Dpad Right + R3`.

When saving, the file will be saved to `/botwsavs/latest.txt`. When restoring, the `/botwsavs/restore.txt` file is read.
This is intentional, so you can't overwrite `restore.txt` by accident.

The saved state file is human-readable, so you can also use this feature to edit the state manually (like breaking slots for IST practice quickly).
You can use this [python script](./scripts/ftp.py) to transfer save state files over FTP (with the `ftpd` homebrew app). Run `python3 ftp.py` to see how to use. 

(This has nothing to do in game, just makes managing the save state files generated by the mod easier)

## Developer
**This section is intended for developers**

### Environment
Only Linux environment is tested.

Install these tools:
- [`DevKitPro`](https://devkitpro.org/wiki/Getting_Started) for switch development. Distros with `pacman` can use the commands at [vmsetup](https://vmsetup.pistonite.org/tool/devkitpro)
- [`megaton`](https://github.com/Pistonight/megaton) for building - Rust toolchain required
- `clang-format` for code formatting

These tools are (very strongly) recommended but optional:
- This project has out-of-the-box support for `clangd`. Make sure `/usr/bin/g++` is available or you can change the compiler manually in `.clangd`
- [`magoo`](https://github.com/Pistonite/magoo) for git submodule management
- `lftp` for FTP transfer - install it with your package manager
- [`task`](https://taskfile.dev/) for running scripts

Clone the repo, then run `magoo install` to clone the submodules. Or clone the repo with `--recursive`

### Build
Run `megaton` (set it up first, see the link above) to build the project. This will build the nso and validate it with symbols from BOTW.

`task upload` will send the binary to your console over FTP.
Use `CONSOLE_ADDR` environment variable to specify the IP address of your console.

`task download` will download crash reports from the console (and delete them from the console).
