#pragma once
#include "ConfigFile.hpp"

namespace botwsavs {

namespace core {
class Worker;
}

namespace fs {

class WorkerSaveFile : public ConfigFile {
public:
    WorkerSaveFile(core::Worker& worker) : ConfigFile("sd:/botwsavs/worker.txt"), mWorker(worker){};
    ~WorkerSaveFile() = default;

protected:
    void SaveInternal() override;
    void LoadInternal() override;

    core::Worker& mWorker;
};

}  // namespace fs
}  // namespace botwsavs
