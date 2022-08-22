#pragma once
#include "NamedValue.hpp"
#include "types.h"

namespace botwsavs::core {
class State {
public:
    enum Error : u32 {
        None = 0,
        Pointer = 1,
        NotEquipped = 1 << 1,
        DifferentName = 1 << 2,
    };

    Error GetError() { return mError; }
    bool HasError(Error mask) { return (mError & mask) != 0; }

    // Read from game memory
    bool ReadFromGame(u32 level);
    // Write to game memory
    bool WriteToGame(u32 level);

private:
    Error mError = Error::None;
    void SetError(Error mask) { mError = static_cast<Error>(mError | mask); }

public:
    u32 mLevel = 0;  // 0 = nothing stored

    // Level 1: Position, Health, Stam, Runes, Camera
public:
    u32 mHealth;
    f32 mStamina;
    f32 mHavokPosition[3];
    f32 mMainPositionMatrix[12];
    f32 mCameraPanMatrix[12];
    f32 mCameraZoom;
    f32 mCameraTilt;
    // Extras:
    // reset rune cooldown
    // reset fall damage

private:
    void ReadLevel1();
    void WriteLevel1();

    // Level 2: Durability
public:
    NamedValue<u32, 64> mMenuEquippedArrow{sInvalid};
    NamedValue<u32, 64> mMenuEquippedWeapon{sInvalid};
    NamedValue<u32, 64> mMenuEquippedBow{sInvalid};
    NamedValue<u32, 64> mMenuEquippedShield{sInvalid};
    NamedValue<u32, 64> mOverworldEquippedWeapon{sInvalid};
    NamedValue<u32, 64> mOverworldEquippedBow{sInvalid};
    NamedValue<u32, 64> mOverworldEquippedShield{sInvalid};
    void ReadLevel2();
    void WriteLevel2();

    // Level 3: All
private:
    void ReadLevel3();
    void WriteLevel3();

private:
    static const u32 sInvalid = 0xFFFFFFFF;
};

}  // namespace botwsavs::core
