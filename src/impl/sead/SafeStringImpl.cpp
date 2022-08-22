#include <prim/seadSafeString.h>

namespace sead {

template <>
void BufferedSafeStringBase<char>::assureTerminationImpl_() const {
    const_cast<BufferedSafeStringBase*>(this)->getMutableStringTop_()[mBufferSize - 1] =
        this->cNullChar;
}

}  // namespace sead
