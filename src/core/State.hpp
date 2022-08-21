#pragma once

#include "types.h"

namespace botwsavs::core {
class State {
public:
    // Read from game memory
    bool ReadFromGame(u32 level);
    // Write to game memory
    bool WriteToGame();

public:
    u32 mLevel = 0;  // 0 = nothing stored
    bool mError = false;

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
private:
    u32 mMenuEquippedArrow;
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
