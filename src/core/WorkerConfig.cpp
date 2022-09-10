#include "fs/Logger.hpp"
#include "Worker.hpp"


namespace botwsavs::core {

void Worker::Save(fs::ConfigFile& file) const {
    file.WriteInteger(named(mLevel));
    mKeyMgr.Save(file);
    
}

void Worker::Load(fs::ConfigFile& file) {
    file.ReadInteger(&mLevel);
    mKeyMgr.Load(file);
}

bool Worker::SaveConfig() const {
    info("Saving worker config to file");
    fs::ConfigFile configFile(WORKER_TXT_PATH);

    if (!configFile.Save(*this)) {
        warn("File operation failed. Cannot save worker config.");
        return false;
    }
    return true;
}

bool Worker::LoadConfig() {
    info("Loading worker config from file");
    fs::ConfigFile configFile(WORKER_TXT_PATH);

    if (!configFile.Load(*this)) {
        warn("File operation failed. Cannot init worker config. Will use default values.");
        return false;
    }
    return true;
}

}
