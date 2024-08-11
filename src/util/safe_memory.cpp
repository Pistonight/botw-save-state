#include <exl/types.h>

namespace botw::savs {

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

}  // namespace botw::savs
