#pragma once

#include "KingPtr.hpp"
#include "SafePtr.hpp"
#include "types.h"

namespace botwsavs::mem {

class MemPtr {
public:
    // Create pointer to main
    MemPtr() { mPtr = reinterpret_cast<char*>(&KingPtr::MainMemory); }
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
    SafePtr<T> AsPtrTo() {
        T* ptr = mError ? nullptr : reinterpret_cast<T*>(mPtr);
        SafePtr safePtr{ptr};
        return safePtr;
    }

private:
    char* mPtr;
    bool mError = false;
};

}  // namespace botwsavs::mem
