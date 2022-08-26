#pragma once
#include "ConfigFile.hpp"

namespace botwsavs{

namespace core{
class State;
}

namespace fs{

class StateSaveFile : public ConfigFile {
public:
    StateSaveFile(const char* path, core::State& state) : ConfigFile(path), mState(state) {};
    ~StateSaveFile() = default;

protected:
    void SaveInternal() override;
    void LoadInternal() override;

private:
    void ReadVersion1();
    void ReadVersion2();
    void ReadVersion3();

    core::State& mState;
};

}    
}

#define STATE_SAVE_FILE_VERSION 3
#define StateFileWrite(TYPE, MEMBER) Write##TYPE(#MEMBER, mState.MEMBER)
#define StateFileWriteArray(TYPE, MEMBER, SIZE) Write##TYPE##Array(#MEMBER, mState.MEMBER, SIZE)
#define StateFileWriteString(MEMBER, SIZE) WriteString(#MEMBER, mState.MEMBER, SIZE)
#define StateFileWriteNamedValue(TYPE, MEMBER)                                                \
    WriteString("Name of value below", mState.MEMBER.GetName());   \
    Write##TYPE(#MEMBER, mState.MEMBER.GetValue())

#define StateFileRead(TYPE, MEMBER) Read##TYPE(&mState.MEMBER)
#define StateFileReadArray(TYPE, MEMBER, SIZE) Read##TYPE##Array(mState.MEMBER, SIZE);
#define StateFileReadString(MEMBER, SIZE) ReadString(mState.MEMBER, SIZE)
#define StateFileReadNamedValue(TYPE, MEMBER, SIZE)                                                 \
    ReadString(mState.MEMBER.Name(), SIZE);                              \
    Read##TYPE(mState.MEMBER.GetValuePtr());
