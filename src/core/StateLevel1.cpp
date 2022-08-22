#include "State.hpp"
#include "StateMacros.hpp"
#include "fs/Logger.hpp"
#include "mem/GamePtr.h"

namespace botwsavs::core {

void State::ReadLevel1() {
    GameReadEz(Health);
    GameReadEz(Stamina);
    GameReadArrayEz(HavokPosition, 3);
    GameReadArrayEz(MainPositionMatrix, 12);
    GameReadArrayEz(CameraPanMatrix, 12);
    GameReadEz(CameraZoom);
    GameReadEz(CameraTilt);
}

void State::WriteLevel1() {
    GameWriteEz(Health);
    GameWriteEz(Stamina);
    GameWriteArrayEz(HavokPosition, 3);
    GameWriteArrayEz(MainPositionMatrix, 12);
    GameWriteArrayEz(CameraPanMatrix, 12);
    GameWriteEz(CameraZoom);
    GameWriteEz(CameraTilt);

    // extras
    GameWrite(GamePtr::RoundBombCooldown, 360.0F);
    GameWrite(GamePtr::SquareBombCooldown, 360.0F);
    GameWrite(GamePtr::StasisCooldown, 1.0F);
}

}  // namespace botwsavs::core
