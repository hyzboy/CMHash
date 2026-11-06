#pragma once

#include<hgl/CoreType.h>
#include<hgl/platform/Platform.h>
#include<hgl/type/MemoryUtil.h>
#include<hgl/type/Str.Hex.h>
#include<hgl/util/hash/Algorithm.h>
#include<hgl/util/hash/generated/AlgorithmRegistry.gen.h>

#include<compare>

/**
 * CMHash —— 引擎的唯一散列实现模块。
 *
 * 边界约定
 * ========
 *   · 本模块**只依赖 CMCoreType**：没有文件 IO、没有 hgl::String、没有容器。
 *   · 算法名、数量、严格解析全部来自 Clang AST 生成的
 *     `generated/AlgorithmRegistry.gen.h`，任何地方不得再手写算法名表。
 *   · 文件 / 流的哈希由上层（CMCore 的 hgl::util::hash::HashFile）基于
 *     FileInputStream 分块调用本模块完成，本模块不关心数据从哪里来。
 */
namespace hgl::util::hash
{
    /**
    * Hash 摘要结构模板
    *
    * 定长、按字节字典序比较，适合直接作为 map 键或写进序列化流。
    */
    template<size_t SIZE> struct Digest
    {
        uint8 code[SIZE]{};

    public:

        template<typename T>
        using CharArray=T[(SIZE<<1)+1];

    public:

        static constexpr size_t size()noexcept{return SIZE;}

        const uint8 operator[](int index)const{return code[index];}

        void CopyFrom   (const void *ptr){memcpy(code,ptr,SIZE);}
        void FromString (const char *str){ParseHexStr(code,str,SIZE);}

        template<typename T> void ToUpperString(T *str,const T gap_char=0) const {ToUpperHexStr<T>(str,code,SIZE,gap_char);}
        template<typename T> void ToLowerString(T *str,const T gap_char=0) const {ToLowerHexStr<T>(str,code,SIZE,gap_char);}

        /**
         * 三路比较运算符（C++20）
         */
        std::strong_ordering operator<=>(const Digest &other) const
        {
            return hgl::mem_compare_ordering(code, other.code, SIZE);
        }

        bool operator==(const Digest &other) const
        {
            return hgl::mem_compare(code, other.code, SIZE) == 0;
        }

        bool operator!=(const Digest &other) const
        {
            return !(*this == other);
        }
    };//template<size_t SIZE> struct Digest

    // 向后兼容别名
    template<size_t SIZE> using HashCode = Digest<SIZE>;

    /**
     * CRTP Hash 基类模板 —— 纯模板设计，无虚函数。
     *
     * 派生类实现：
     *   void Init();
     *   void Update(const void *data, size_t size);
     *   void Final(void *result);
     *
     * 注意：`DigestSize` 必须等于 `Final()` 实际写出的字节数。历史上
     * Murmur3 声明 16 而实际只写 4，问题就出在这里。
     */
    template<typename Derived, size_t HashSize>
    class Base
    {
    protected:
        Base() = default;

    public:
        static constexpr size_t DigestSize = HashSize;

        ~Base() = default;

        // 删除复制（某些派生类持有资源）
        Base(const Base&) = delete;
        Base& operator=(const Base&) = delete;

        Base(Base&&) = default;
        Base& operator=(Base&&) = default;

        void Init()
        {
            static_cast<Derived*>(this)->Init();
        }

        void Update(const void *data, size_t size)
        {
            static_cast<Derived*>(this)->Update(data, size);
        }

        void Final(void *result)
        {
            static_cast<Derived*>(this)->Final(result);
        }

        /// 一次性计算整块数据的摘要
        struct Result
        {
            uint8 code[HashSize];
        };

        Result Hash(const void *data, size_t size)
        {
            Result result{};

            Init();
            Update(data, size);
            Final(result.code);

            return result;
        }
    };//template<typename Derived, size_t HashSize> class Base

    // ============== 元信息（全部来自 AST 生成的注册表）================

    /// 算法总数（= enum Algorithm 的真实成员数）
    constexpr size_t GetAlgorithmCount()noexcept{return generated::kAlgorithmCount;}

    /// 按声明顺序取第 index 个算法；越界返回 false 且不修改 out
    bool GetAlgorithmAt(size_t index, Algorithm &out);

    /// 摘要字节数；越界返回 0
    size_t GetDigestSize(Algorithm algo);

    /// 规范名称（用于写进清单、命令行提示）；越界返回空串
    const char *GetName(Algorithm algo);

    /// 稳定 ID；越界返回 0
    uint32 GetID(Algorithm algo);

    /// 该算法是否支持分块累加（Update 多次与一次效果相同）
    bool IsStreamable(Algorithm algo);

    /**
     * 严格名称解析：只接受注册表里的规范拼写，区分大小写。
     *
     * 这是**唯一**的名称解析入口。历史上的 `FromName` 在未命中时静默返回
     * MD5，把「拼错算法名」变成「整包按错误算法重算」，因此被彻底移除：
     * 调用方必须处理 false。
     */
    bool TryParseAlgorithm(const char *name, Algorithm &out);

    /// 严格 ID 解析：未命中返回 false 且不修改 out
    bool TryGetAlgorithmByID(uint32 id, Algorithm &out);

    // ============== 计算 ================

    /**
     * 一次性计算数据的摘要。
     *
     * @param size       数据字节数；0 合法（空输入有确定的摘要），此时 data 可为 nullptr
     * @param hash_code  输出缓冲区，容量必须 >= GetDigestSize(algo)
     * @return 成功返回 true；算法非法或参数非法返回 false（不会写入部分结果）
     */
    bool Hash(Algorithm algo, const void *data, size_t size, void *hash_code);

    /// 摘要转小写十六进制；out_hex 容量需 >= digest_size*2+1
    void DigestToLowerHex(const void *digest, size_t digest_size, char *out_hex);

    // ============ 向后兼容的旧 API 别名 ============
    inline bool ComputeHash(Algorithm ha, const void *data, size_t size, void *hash_code)
    {
        return Hash(ha, data, size, hash_code);
    }

    inline size_t GetHashDigestSize(Algorithm ha)
    {
        return GetDigestSize(ha);
    }

    inline const char* GetHashAlgorithmName(Algorithm ha)
    {
        return GetName(ha);
    }

    inline uint32 GetHashAlgorithmID(Algorithm ha)
    {
        return GetID(ha);
    }
}//namespace hgl::util::hash
