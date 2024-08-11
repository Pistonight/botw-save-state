#pragma once

namespace botw::savs {
enum Version {
    v1 = 1, // level 1 only
    v2 = 2, // add level 2
    v3 = 3, // add level 3
    
    // level 3: add num broken slots
    v4 = 4, 

    vLatest = v4
};
}
