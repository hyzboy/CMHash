#pragma once

#include <cstddef>

namespace hgl::hash
{
    // 64-bit hash-seed merge using a golden-ratio constant.
    constexpr size_t HashMergeGolden64(const size_t seed, const size_t value) noexcept
    {
        return seed ^ (value + 0x9e3779b97f4a7c15ULL + (seed << 6U) + (seed >> 2U));
    }
}
