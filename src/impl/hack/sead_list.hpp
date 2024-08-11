#pragma once

#include <exl/types.h>

namespace sead {
class ListImpl;
}

namespace botw::savs::hack {

void set_sead_list_m_count(sead::ListImpl* list, s32 count);

s32 get_sead_list_real_size(sead::ListImpl* list);

}
