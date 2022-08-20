# botw-save-state
A BOTW 1.6.0 Switch save state mod for speedrun practices

## Install
1. Download and extract the zip. There should be 2 folders `atmosphere` and `botwsavs`
2. Copy both folders to the root of your SD card. It's safe to overwrite existing files if the only mod you have is the save state. There's no guarantee that other mods work with this

## Uninstall
1. Remove these folders from the SD card if they exist
    - `/atmosphere/contents/01007EF00011E000/exefs`
    - `/atmosphere/contents/01007EF00011E000/romfs`
    - `/atmosphere/exefs_patches/botwsavs`
    - `/botwsavs`

# Key Combo

There are 2 modes in the save state mod, the Active Mode and the Setting Mode.
- Active Mode: Can save and restore state
- Setting Mode: Can change the level of save state:
  - Level 0: Disabled (Cannot save or restore)
  - Level 1: Basic (Position, Health, Stamina, Camera angle, Rune recharge)
  - Level 2: Basic + Durability
  - Level 3: All

Hold Dpad Down + All 4 triggers (ZL L R ZR) for 4 seconds to switch between modes.

You will see "Save state: Active Mode" or "Save state: Setting Mode" when the mode switches

### Save/Restore
You can use these key combos in the Active Mode
- PLUS + Dpad Left: Save state to memory
- PLUS + Dpad Left + R3: Save state to botwsavs/latest.txt on sd card
- PLUS + Dpad Right: Restore state from memory
- PLUS + Dpad Right + R3: Restore state from botwsavsrestore.txt on sd card

You can only restore if the current setting level is less than or equal to the save state level. For example, if the setting level is 2 (Basic + Durability), and you restore a save state with level 3, only Basic + Durability would be restored. However, if the setting is level 2 and you try to restore a save state with level 1, the mod will show "You can't do that"

### Setting
You can use these key combos in the Setting Mode
- Hold R for 1 second: Increase level
- Hold L for 1 second: Decrease level
