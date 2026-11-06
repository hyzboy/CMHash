#pragma once

#include <hgl/CoreType.h>
#include <cstddef>
#include <cstring>
#include <memory>
#include <string>
#include <string_view>

namespace hgl::hash
{
    template<typename HashType>
    struct FNV1aTraits;

    template<>
    struct FNV1aTraits<uint32>
    {
        static constexpr uint32 OffsetBasis = 2166136261u;
        static constexpr uint32 Prime = 16777619u;
    };

    template<>
    struct FNV1aTraits<uint64>
    {
        static constexpr uint64 OffsetBasis = 1469598103934665603ull;
        static constexpr uint64 Prime = 1099511628211ull;
    };

    template<typename HashType>
    constexpr HashType FNV1aInit(const HashType seed = FNV1aTraits<HashType>::OffsetBasis) noexcept
    {
        return seed;
    }

    template<typename HashType, typename ValueType>
    constexpr HashType FNV1aAppend(HashType hash, const ValueType value) noexcept
    {
        hash ^= static_cast<HashType>(value);
        hash *= FNV1aTraits<HashType>::Prime;
        return hash;
    }

    template<typename HashType>
    inline HashType FNV1aAppendBytes(HashType hash, const void *data, const size_t size) noexcept
    {
        if (!data || size == 0)
            return hash;

        const auto *bytes = reinterpret_cast<const uchar *>(data);

        for (size_t i = 0; i < size; ++i)
            hash = FNV1aAppend(hash, bytes[i]);

        return hash;
    }

    template<typename HashType, typename ValueType>
    inline HashType FNV1aAppendValueBytes(HashType hash, const ValueType &value) noexcept
    {
        return FNV1aAppendBytes(hash, std::addressof(value), sizeof(ValueType));
    }

    // ────────────────────────────────────────────────
    // FNV1aHasher — 有状态链式哈希器，取代冗长的 FNV1aAppendValueBytes
    // ────────────────────────────────────────────────
    // 用法示例：
    //   FNV1aHasher64 h;
    //   h << field1 << field2 << field3;
    //   h << std::string("hello") << "literal" << std::string_view(buf, len);
    //   h.AppendBytes(raw_data, size);      // 原始字节缓冲区
    //   uint64 result = h;
    //
    // 字符串统一按「uint32 长度前缀 + 原始字节」编码，可安全区分拼接边界。
    // 也支持直接从构造链式调用：
    //   uint64 result = FNV1aHasher64{} << a << b << c;
    template<typename HashType>
    class FNV1aHasher
    {
        HashType hash_;

    public:
        constexpr FNV1aHasher() noexcept
            : hash_(FNV1aInit<HashType>()) {}

        // 从指定种子开始（用于向既有哈希继续追加）
        constexpr explicit FNV1aHasher(const HashType seed) noexcept
            : hash_(seed) {}

        // 追加任意值（按字节表示）
        template<typename T>
        FNV1aHasher &operator<<(const T &value) noexcept
        {
            hash_ = FNV1aAppendValueBytes(hash_, value);
            return *this;
        }

        // 追加原始字节（用于缓冲区、二进制数据等）
        FNV1aHasher &AppendBytes(const void *data, const size_t size) noexcept
        {
            hash_ = FNV1aAppendBytes(hash_, data, size);
            return *this;
        }

        // 追加 std::string：uint32 长度前缀 + 原始字节
        FNV1aHasher &operator<<(const std::string &value) noexcept
        {
            const uint32 length = static_cast<uint32>(value.size());
            hash_ = FNV1aAppendValueBytes(hash_, length);
            if (length > 0)
                hash_ = FNV1aAppendBytes(hash_, value.data(), length);
            return *this;
        }

        // 追加 C 字符串：uint32 长度前缀 + 原始字节（value 为 nullptr 时按空串处理）
        FNV1aHasher &operator<<(const char *value) noexcept
        {
            const uint32 length = value
                ? static_cast<uint32>(std::strlen(value)) : 0u;
            hash_ = FNV1aAppendValueBytes(hash_, length);
            if (length > 0)
                hash_ = FNV1aAppendBytes(hash_, value, length);
            return *this;
        }

        // 追加 string_view：uint32 长度前缀 + 原始字节
        FNV1aHasher &operator<<(std::string_view value) noexcept
        {
            const uint32 length = static_cast<uint32>(value.size());
            hash_ = FNV1aAppendValueBytes(hash_, length);
            if (length > 0)
                hash_ = FNV1aAppendBytes(hash_, value.data(), length);
            return *this;
        }

        HashType Result() const noexcept { return hash_; }
        operator HashType() const noexcept { return hash_; }

        void Reset() noexcept { hash_ = FNV1aInit<HashType>(); }
    };

    using FNV1aHasher32 = FNV1aHasher<uint32>;
    using FNV1aHasher64 = FNV1aHasher<uint64>;
}
