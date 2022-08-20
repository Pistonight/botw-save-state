#pragma once

#include "ksys/KSys.hpp"
#include "types.h"
#include "util/SafePtr.hpp"

namespace botwsavs::core {

class MemPtr {
public:
    // Create pointer to main
    MemPtr() { mPtr = reinterpret_cast<char*>(&ksys::MainMemory); }
    // Copy constructor
    MemPtr(const MemPtr& other) {
        mPtr = other.mPtr;
        mError = other.mError;
    }
    // Offset
    MemPtr& Add(s64 offset) {
        mPtr += offset;
        return *this;
    }
    // p+x is the same as p.Add(xxx)
    MemPtr& operator+(s64 offset) { return Add(offset); }
    // Deferring and storing the value as pointer
    MemPtr& Deref() {
        if (!mPtr) {
            mError = true;
            return *this;
        }
        char** pp = reinterpret_cast<char**>(mPtr);
        mPtr = *pp;
        return *this;
    }
    // p[x] is the same as p.Add(xxx).Deref()
    MemPtr& operator[](s64 offset) { return Add(offset).Deref(); }

    template <typename T>
    util::SafePtr<T> AsPtrTo() {
        T* ptr = mError ? nullptr : reinterpret_cast<T*>(mPtr);
        util::SafePtr safePtr{ptr};
        return safePtr;
    }

private:
    char* mPtr;
    bool mError = false;
};

}  // namespace botwsavs::core
