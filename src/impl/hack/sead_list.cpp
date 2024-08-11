// Need private field access
#define private public
#define protected public
#include <container/seadListImpl.h>
#undef private
#undef protected

namespace botw::savs::hack {

void set_sead_list_m_count(sead::ListImpl* list, s32 count) {
    list->mCount = count;
}

s32 get_sead_list_real_size(sead::ListImpl* list) {
    s32 size = 0;
    sead::ListNode* ptr = list->mStartEnd.mNext;
    while(ptr && ptr != &list->mStartEnd){
        ptr = ptr->mNext;
        size++;
    }
    return size;
}

}
