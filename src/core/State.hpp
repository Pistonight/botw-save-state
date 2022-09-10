#pragma once
#include "fs/ConfigFile.hpp"
#include "util/NamedValue.hpp"

#include "StateLevel1.hpp"
#include "StateLevel2.hpp"
#include "StateLevel3.hpp"

#include "types.h"

#define LATEST_TXT_PATH "sd:/botwsavs/latest.txt"
#define RESTORE_TXT_PATH "sd:/botwsavs/restore.txt"
#define STATE_SAVE_FILE_VERSION 4

namespace botwsavs::core {
class State : fs::Config {
public:
    u32 GetErrorMask() const {
        return mStorageLevel1.GetErrorMask() | mStorageLevel2.GetErrorMask() |
               mStorageLevel3.GetErrorMask();
    }
    bool HasError(StateError mask) const { return (GetErrorMask() & mask) != 0; }
    bool HasAnyError() const { return GetErrorMask() != StateError::None; }

    u32 GetLevel() const { return mLevel; }

    // Read from game memory
    bool ReadFromGame(u32 level);
    // Write to game memory
    bool WriteToGame(u32 level);
    // Read from file
    bool ReadFromFile();
    bool CanReadFromFile();
    // Write to file
    bool WriteToFile() const;

private:
    void ClearError() {
        mStorageLevel1.ClearError();
        mStorageLevel2.ClearError();
        mStorageLevel3.ClearError();
    }

    // internal functions for read/write file
    void Load(fs::ConfigFile& file) override;
    void Save(fs::ConfigFile& file) const override;

private:
    u32 mLevel = 0;  // 0 = nothing stored
    StateLevel1 mStorageLevel1;
    StateLevel2 mStorageLevel2;
    StateLevel3 mStorageLevel3;
};

}  // namespace botwsavs::core
