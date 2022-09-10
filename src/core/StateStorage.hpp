#pragma once

#include "fs/Logger.hpp"
#include "mem/SafePtr.hpp"
#include "types.h"

namespace botwsavs{

namespace fs{
class ConfigFile;
}

namespace core {

enum StateError : u32 {
    None = 0,
    Pointer = 1,
    NotEquipped = 1 << 1,
    DifferentName = 1 << 2,
};

class StateStorage {
public:
    virtual void ReadFromGame();
    virtual void WriteToGame();
    virtual void ReadFromFile(fs::ConfigFile& file, u32 version);
    virtual void WriteToFile(fs::ConfigFile& file) const;

    u32 GetErrorMask() const { return mError; }
    bool HasError(StateError mask) const { return (mError & mask) != 0; }
    bool HasAnyError() const {return mError != StateError::None; }
    void ClearError() { mError = StateError::None; }
protected:
    void SetError(StateError mask) { mError = static_cast<StateError>(mError | mask); }
    // Helper functions for read/write game value
    template<typename T>
    void ReadMemory(const char* name, mem::SafePtr<T> ptr, T* out){
        if (!ptr.Get(out)){
            errorf("Read %s failed", name);                                                        
            SetError(StateError::Pointer);
        }
    }

    template<typename T>
    void WriteMemory(const char* name, mem::SafePtr<T> ptr, const T value) {
        if (!ptr.Set(value)) {                                                           
            errorf("Write %s failed", name);                                                       
            SetError(StateError::Pointer);                                                                  
        }
    }

    template<typename T>
    void ReadMemoryArray(const char* name, mem::SafePtr<T> ptr, T* outArray, u32 size){
        if (!ptr.GetArray(outArray, size)) {                                                
            errorf("Read %s array failed", name);                                                  
            SetError(StateError::Pointer);                                                                  
        }
    }

    template<typename T>
    void WriteMemoryArray(const char* name, mem::SafePtr<T> ptr, const T* outArray, u32 size){
        if (!ptr.SetArray(outArray, size)) {                                                
            errorf("Write %s array failed", name);                                                  
            SetError(StateError::Pointer);                                                                  
        }
    }

    template<typename T>
    bool TakePtrOrError(const char* name, mem::SafePtr<T> ptr, T** out) {
        if (!ptr.TakePtr(out)) {                                                                
            errorf("Get pointer %s failed", name);                                                 
            SetError(StateError::Pointer);
            return false;                                                               
        }
        return true;
    }

    StateError mError = StateError::None;

    static const u32 sInvalid = 0xFFFFFFFF;
};

}}

