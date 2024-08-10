#include <exl/types.h>

#include "util/safe_memory.hpp"

namespace botwsavs::util {

bool ptr_looks_safe(const void* ptr) {
    u64 raw = reinterpret_cast<u64>(ptr);

    if (raw > 0xFFFFFFFFFF || (raw >> 32 == 0)) {
        return false;
    }
    if ((raw & 3) != 0) {
        return false;
    }

    return true;
}

}
