#pragma once
#define GameReadOrError(POINTER, MEMBER)                                                           \
    if (!(mem::POINTER)().Get(&MEMBER)) {                                                          \
        errorf("Read %s failed", #POINTER);                                                        \
        SetError(Error::Pointer);                                                                  \
    }
#define GameReadEz(COMMON_NAME) GameReadOrError(GamePtr::COMMON_NAME, m##COMMON_NAME)
#define GameRead(POINTER, MEMBER) if ((mem::POINTER)().Get(&MEMBER))
#define GameReadArray(POINTER, SIZE, MEMBER)                                                       \
    if (!(mem::POINTER)().GetArray(MEMBER, SIZE)) {                                                \
        errorf("Read %s array failed", #POINTER);                                                  \
        SetError(Error::Pointer);                                                                  \
    }
#define GameReadArrayEz(COMMON_NAME, SIZE) GameReadArray(GamePtr::COMMON_NAME, SIZE, m##COMMON_NAME)

#define GameWrite(POINTER, MEMBER)                                                                 \
    if (!(mem::POINTER)().Set(MEMBER)) {                                                           \
        errorf("Write %s failed", #POINTER);                                                       \
        SetError(Error::Pointer);                                                                  \
    }
#define GameWriteEz(COMMON_NAME) GameWrite(GamePtr::COMMON_NAME, m##COMMON_NAME)
#define GameWriteArray(POINTER, SIZE, MEMBER)                                                      \
    if (!(mem::POINTER)().SetArray(MEMBER, SIZE)) {                                                \
        errorf("Write %s array failed", #POINTER);                                                 \
        SetError(Error::Pointer);                                                                  \
    }
#define GameWriteArrayEz(COMMON_NAME, SIZE)                                                        \
    GameWriteArray(GamePtr::COMMON_NAME, SIZE, m##COMMON_NAME)

#define GameUsePtr(POINTER, TYPE, VAR)                                                             \
    TYPE* VAR;                                                                                     \
    if ((POINTER).TakePtr(&VAR))
#define GameUsePtrOrError(POINTER, TYPE, VAR)                                                      \
    TYPE* VAR;                                                                                     \
    if (!(POINTER).TakePtr(&VAR)) {                                                                \
        errorf("Get pointer %s failed", #POINTER);                                                 \
        SetError(Error::Pointer);                                                                  \
    } else
#define GameUseSafePtr(UNSAFE_POINTER, TYPE, VAR)                                                  \
    GameUsePtr(mem::SafePtr(UNSAFE_POINTER), TYPE, VAR)
#define GameUseSafePtrOrError(UNSAFE_POINTER, TYPE, VAR)                                           \
    GameUsePtrOrError(mem::SafePtr(UNSAFE_POINTER), TYPE, VAR)
