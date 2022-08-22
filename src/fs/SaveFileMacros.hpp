#pragma once
#define SaveFileWrite(TYPE, MEMBER) result = result && Write##TYPE(buffer, #MEMBER, state.MEMBER);
#define SaveFileWriteArray(TYPE, MEMBER, SIZE)                                                     \
    result = result && Write##TYPE##Array(buffer, #MEMBER, state.MEMBER, SIZE);
#define SaveFileWriteString(MEMBER, SIZE)                                                          \
    result = result && WriteString(buffer, #MEMBER, state.MEMBER, SIZE)
#define SaveFileWriteNamedValue(TYPE, MEMBER, SIZE)                                                \
    result = result && WriteString(buffer, "Name of value below", state.MEMBER.GetName(), SIZE);   \
    result = result && Write##TYPE(buffer, #MEMBER, state.MEMBER.GetValue())

#define SaveFileRead(TYPE, MEMBER) result = result && Read##TYPE(buffer, &state.MEMBER);
#define SaveFileReadArray(TYPE, MEMBER, SIZE)                                                      \
    result = result && Read##TYPE##Array(buffer, state.MEMBER, SIZE);
#define SaveFileReadString(MEMBER, SIZE) result = result && ReadString(buffer, state.MEMBER, SIZE)
#define SaveFileReadNamedValue(TYPE, MEMBER, SIZE)                                                 \
    result = result && ReadString(buffer, state.MEMBER.Name(), SIZE);                              \
    result = result && Read##TYPE(buffer, state.MEMBER.GetValuePtr());
