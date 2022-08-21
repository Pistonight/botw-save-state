#include "State.hpp"
#include "PointerList.hpp"
#include "fs/Logger.hpp"

namespace botwsavs::core {

bool State::ReadFromGame(u32 level) {
    mError = false;
    switch (level) {
    case 3:
        ReadLevel3();
        // fall through
    case 2:
        ReadLevel2();
        // fall through
    case 1:
        ReadLevel1();
    }
    mLevel = level;
    return !mError;
}

bool State::WriteToGame(u32 level) {
    if(mLevel < level){
        return false;
    }
    mError = false;
    switch (level) {
    case 3:
        WriteLevel3();
        // fall through
    case 2:
        WriteLevel2();
        // fall through
    case 1:
        WriteLevel1();
    }
    return !mError;
}

#define Read(POINTER, MEMBER)                                                                      \
    if (!(POINTER)().Get(&MEMBER)) {                                                               \
        errorf("Read %s failed", #POINTER);                                                        \
        mError = true;                                                                             \
    }
#define ReadArray(POINTER, SIZE, MEMBER)                                                           \
    if (!(POINTER)().GetArray(MEMBER, SIZE)) {                                                     \
        errorf("Read %s array failed", #POINTER);                                                  \
        mError = true;                                                                             \
    }
#define Write(POINTER, MEMBER)                                                                     \
    if (!(POINTER)().Set(MEMBER)) {                                                                \
        errorf("Write %s failed", #POINTER);                                                       \
        mError = true;                                                                             \
    }
#define WriteArray(POINTER, SIZE, MEMBER)                                                          \
    if (!(POINTER)().SetArray(MEMBER, SIZE)) {                                                     \
        errorf("Write %s array failed", #POINTER);                                                 \
        mError = true;                                                                             \
    }

void State::ReadLevel1() {
    Read(ptr::Health, mHealth);
    Read(ptr::Stamina, mStamina);
    ReadArray(ptr::HavokPosition, 3, mHavokPosition);
    ReadArray(ptr::MainPositionMatrix, 12, mMainPositionMatrix);
    ReadArray(ptr::CameraPanMatrix, 12, mCameraPanMatrix);
    Read(ptr::CameraZoom, mCameraZoom);
    Read(ptr::CameraTilt, mCameraTilt);
}

void State::WriteLevel1() {
    Write(ptr::Health, mHealth);
    Write(ptr::Stamina, mStamina);
    WriteArray(ptr::HavokPosition, 3, mHavokPosition);
    WriteArray(ptr::MainPositionMatrix, 12, mMainPositionMatrix);
    WriteArray(ptr::CameraPanMatrix, 12, mCameraPanMatrix);
    Write(ptr::CameraZoom, mCameraZoom);
    Write(ptr::CameraTilt, mCameraTilt);

    // extras
    Write(ptr::RoundBombCooldown, 360.0F);
    Write(ptr::SquareBombCooldown, 360.0F);
    Write(ptr::StasisCooldown, 1.0F);
}

void State::ReadLevel2() { /*TODO*/
}
void State::WriteLevel2() { /*TODO*/
}
void State::ReadLevel3() { /*TODO*/
}
void State::WriteLevel3() { /*TODO*/
}

}  // namespace botwsavs::core
