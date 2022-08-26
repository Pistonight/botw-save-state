#include "State.hpp"

namespace botwsavs::core {

bool State::ReadFromGame(u32 level) {
    mError = Error::None;
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
    return mError == Error::None;
}

bool State::WriteToGame(u32 level) {
    if (mLevel < level) {
        return false;
    }
    mError = Error::None;
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
    return mError == Error::None;
}

}  // namespace botwsavs::core
