#pragma once

#include<hgl/CoreType.h>
#include<type_traits>
#include<hgl/util/hash/wyhash.h>

namespace hgl
{
    template<typename T>
    inline uint64 ComputeOptimalHash(const T& value)
    {
        if constexpr (std::is_integral_v<T> || std::is_enum_v<T>)
        {
            // CN:整数/枚举类型：直接转换（零开销）
            // EN:Integer/enum types: direct conversion (zero overhead)
            return static_cast<uint64>(value);
        }
        else if constexpr (std::is_pointer_v<T>)
        {
            // CN:指针类型：使用地址作为哈希
            // EN:Pointer types: use address as hash
            return reinterpret_cast<uint64>(value);
        }
        else
        {
            // CN:复杂类型：使用 王一Hash 哈希保证质量
            // EN:Complex types: use WYHASH for quality

            return wyhash(&value, sizeof(T), 0, _wyp);
        }
    }

    inline uint64 ComputeOptimalHash(const void* data, size_t size)
    {
        // CN:原始数据块：使用 王一Hash 哈希保证质量
        // EN:Raw data block: use WYHASH for quality
        return wyhash(data, size, 0, _wyp);
    }
}//namespace hgl
