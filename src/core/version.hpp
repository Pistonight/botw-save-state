#pragma once

namespace botw::savs {
enum Version {
    vLegacy = 4, // no longer supported
    v5 = 5,
    v6 = 6, // inventory (PMDM)
    v7 = 7, // speedometer
    v8 = 8, // rune
    vLatest = v8
};
}
