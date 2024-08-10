#include <prim/seadSafeString.h>

namespace sead {
template <>
const char SafeStringBase<char>::cNullChar = '\0';

template <>
const char SafeStringBase<char>::cLineBreakChar = '\n';

template <>
const SafeStringBase<char> SafeStringBase<char>::cEmptyString("");

template <>
const char16 SafeStringBase<char16>::cNullChar = 0;

template <>
const char16 SafeStringBase<char16>::cLineBreakChar = static_cast<char16>('\n');

template <>
void BufferedSafeStringBase<char>::assureTerminationImpl_() const {
    const_cast<BufferedSafeStringBase*>(this)->getMutableStringTop_()[mBufferSize - 1] =
        this->cNullChar;
}
template <>
SafeStringBase<char>& SafeStringBase<char>::operator=(const SafeStringBase<char>& other) = default;

template <>
SafeStringBase<char16>&
SafeStringBase<char16>::operator=(const SafeStringBase<char16>& other) = default;

template <>
BufferedSafeStringBase<char>&
BufferedSafeStringBase<char>::operator=(const SafeStringBase<char>& other)
{
    copy(other);
    return *this;
}

template <>
BufferedSafeStringBase<char16>&
BufferedSafeStringBase<char16>::operator=(const SafeStringBase<char16>& other)
{
    copy(other);
    return *this;
}

}  // namespace sead
