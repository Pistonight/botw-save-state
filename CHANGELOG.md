# CHANGELOG

### `SS2.0` `2024-08-11`
- Rewrite the mod to use Megaton + exlaunch
- Use widget UI experience for setting mod and more
- Bombs are deleted upon restoring
- **Previous config and state files are no longer compatible**. You might need to re-configure your key bindings if they were changed
- PauseMenuDataMgr access is now thread-safe

### `1.6.0-SS1.1` `2022-09-10`
- Add save and restore offset between pouch item list count (i.e. `mCount`) and actual inventory size to level 3
- Make save and restore key binding configurable
  - In setting mode, hold the current key binding for 3 seconds, then hold the new key binding for 3 seconds.

### `1.6.0-SS1.0` `2022-08-26`
- Level 3 save and restore
- Change save/restore key combo to include both left triggers (`L` and `ZL`). This is so that you don't accidentally save or restore during a super launch
- Enable logging (except for debug logging) in ship builds

### `1.6.0-SS0.2` `2022-08-21`
- Save and restore equipped weapon/bow/shield durability and equipped arrow count
- Durability/count will only restore if you have the same item equipped as when you saved
  - Otherwise you will see a warning message, but the ones that do match will restore fine.
- If you save/restore without something equipped. You will see a warning message, but the rest of the save/restore would 
- Fix saving/restoring to sd card bug
- Add a python script for managing save states over FTP

### `1.6.0-SS0.1` `2022-08-20`
- Only Level 0 and Level 1 works. The rest will be added soon
