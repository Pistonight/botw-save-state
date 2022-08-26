#include "WorkerSaveFile.hpp"
#include "core/Worker.hpp"

namespace botwsavs::fs {
void WorkerSaveFile::LoadInternal()  {
    ReadInteger(&mWorker.mLevel);
}

void WorkerSaveFile::SaveInternal() {
    WriteInteger("level", mWorker.mLevel);
}

}
