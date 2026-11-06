#include<hgl/util/hash/Hash.h>
#include<hgl/util/hash/Hasher.h>

namespace hgl::util::hash
{
    // ── 各算法的一次性计算入口（由各 <algo>.cpp 提供）─────────────────────
    void ComputeHash_Adler32          (const void *data, size_t size, void *result);
    void ComputeHash_CRC32            (const void *data, size_t size, void *result);
    void ComputeHash_MD4              (const void *data, size_t size, void *result);
    void ComputeHash_MD5              (const void *data, size_t size, void *result);
    void ComputeHash_SHA1             (const void *data, size_t size, void *result);
    void ComputeHash_SHA1LE           (const void *data, size_t size, void *result);
    void ComputeHash_SHA256           (const void *data, size_t size, void *result);
    void ComputeHash_SHA512           (const void *data, size_t size, void *result);
    void ComputeHash_Blake3           (const void *data, size_t size, void *result);
    void ComputeHash_XXH32            (const void *data, size_t size, void *result);
    void ComputeHash_XXH64            (const void *data, size_t size, void *result);
    void ComputeHash_XXH3_64          (const void *data, size_t size, void *result);
    void ComputeHash_XXH3_128         (const void *data, size_t size, void *result);
    void ComputeHash_FNV1a            (const void *data, size_t size, void *result);
    void ComputeHash_MurmurHash3      (const void *data, size_t size, void *result);
    void ComputeHash_GoogleCityHash32 (const void *data, size_t size, void *result);
    void ComputeHash_GoogleCityHash64 (const void *data, size_t size, void *result);
    void ComputeHash_GoogleCityHash128(const void *data, size_t size, void *result);

    // ── 分块散列器工厂（仅可分块的算法提供）───────────────────────────────
    Hasher *CreateStreamHasher_Adler32  ();
    Hasher *CreateStreamHasher_CRC32    ();
    Hasher *CreateStreamHasher_MD4      ();
    Hasher *CreateStreamHasher_MD5      ();
    Hasher *CreateStreamHasher_SHA1     ();
    Hasher *CreateStreamHasher_SHA1LE   ();
    Hasher *CreateStreamHasher_SHA256   ();
    Hasher *CreateStreamHasher_SHA512   ();
    Hasher *CreateStreamHasher_BLAKE3   ();
    Hasher *CreateStreamHasher_XXH32    ();
    Hasher *CreateStreamHasher_XXH64    ();
    Hasher *CreateStreamHasher_XXH3_64  ();
    Hasher *CreateStreamHasher_XXH3_128 ();
    Hasher *CreateStreamHasher_FNV1a    ();

    namespace
    {
        struct AlgorithmInfo
        {
            uint32   id;
            uint32   digest_size;   ///< Final() 实际写出的字节数
            bool     streamable;    ///< Update 多次 == Update 一次
            void   (*compute)(const void *data, size_t size, void *result);
            Hasher *(*create_hasher)();
        };

        struct AlgorithmInfoTable
        {
            AlgorithmInfo items[generated::kAlgorithmCount];
        };

        // 整数型摘要（Adler32/CRC32/FNV1a/xxHash/CityHash）直接按主机的
        // uint32/uint64 内存写出。为保证跨平台一致，这里把「必须是小端」
        // 变成编译期约束，而不是运行期的隐性差异。
        static_assert(HGL_ENDIAN == HGL_LITTLE_ENDIAN,
                      "CMHash 的整数型摘要按小端写出；大端平台需要先补显式字节序转换");

        // 枚举值必须等于声明序号，否则按索引初始化的表会整体错位。
        // 这两条断言把「AST 顺序」与「C++ 枚举值」钉死在一起。
        static_assert((size_t)Algorithm::Adler32 == 0,
                      "Algorithm 的第一个成员必须位列 0（注册表按声明顺序索引）");
        static_assert((size_t)Algorithm::City128 == generated::kAlgorithmCount - 1,
                      "Algorithm 的最后一个成员必须位列 kAlgorithmCount-1");

        /**
         * 算法信息表。
         *
         * 索引用 Clang AST 生成的 `generated::AlgorithmIndex` 常量做下标，
         * 而不是靠“这一行排在第几个”对齐 —— 枚举中间插入新算法时，位置
         * 派生的写法会整体错位，下标写法不会。
         *
         * 这里用 constexpr 函数而不是聚合初始化：C++20 的指定初始化器
         * 不支持数组下标形式（那是 C99 扩展，MSVC 不接受）。
         */
        constexpr AlgorithmInfoTable MakeAlgorithmInfoTable()
        {
            AlgorithmInfoTable table{};

            table.items[generated::ALGORITHM_INDEX_Adler32]   = { ID::Adler32,   4,  true,  ComputeHash_Adler32,            CreateStreamHasher_Adler32  };
            table.items[generated::ALGORITHM_INDEX_CRC32]     = { ID::CRC32,     4,  true,  ComputeHash_CRC32,              CreateStreamHasher_CRC32    };
            table.items[generated::ALGORITHM_INDEX_MD4]       = { ID::MD4,      16,  true,  ComputeHash_MD4,                CreateStreamHasher_MD4      };
            table.items[generated::ALGORITHM_INDEX_MD5]       = { ID::MD5,      16,  true,  ComputeHash_MD5,                CreateStreamHasher_MD5      };
            table.items[generated::ALGORITHM_INDEX_SHA1]      = { ID::SHA1,     20,  true,  ComputeHash_SHA1,               CreateStreamHasher_SHA1     };
            table.items[generated::ALGORITHM_INDEX_SHA1LE]    = { ID::SHA1LE,   20,  true,  ComputeHash_SHA1LE,             CreateStreamHasher_SHA1LE   };
            table.items[generated::ALGORITHM_INDEX_SHA256]    = { ID::SHA256,   32,  true,  ComputeHash_SHA256,             CreateStreamHasher_SHA256   };
            table.items[generated::ALGORITHM_INDEX_SHA512]    = { ID::SHA512,   64,  true,  ComputeHash_SHA512,             CreateStreamHasher_SHA512   };
            table.items[generated::ALGORITHM_INDEX_BLAKE3]    = { ID::BLAKE3,   32,  true,  ComputeHash_Blake3,             CreateStreamHasher_BLAKE3   };
            table.items[generated::ALGORITHM_INDEX_XXH32]     = { ID::XXH32,     4,  true,  ComputeHash_XXH32,              CreateStreamHasher_XXH32    };
            table.items[generated::ALGORITHM_INDEX_XXH64]     = { ID::XXH64,     8,  true,  ComputeHash_XXH64,              CreateStreamHasher_XXH64    };
            table.items[generated::ALGORITHM_INDEX_XXH3_64]   = { ID::XXH3_64,   8,  true,  ComputeHash_XXH3_64,            CreateStreamHasher_XXH3_64  };
            table.items[generated::ALGORITHM_INDEX_XXH3_128]  = { ID::XXH3_128, 16,  true,  ComputeHash_XXH3_128,           CreateStreamHasher_XXH3_128 };
            table.items[generated::ALGORITHM_INDEX_FNV1a]     = { ID::FNV1a,     4,  true,  ComputeHash_FNV1a,              CreateStreamHasher_FNV1a    };
            // Murmur3 收尾时把总长度异或进状态，分块累加与一次算不等价。
            table.items[generated::ALGORITHM_INDEX_Murmur3]   = { ID::Murmur3,   4,  false, ComputeHash_MurmurHash3,        nullptr };
            // CityHash 没有增量接口，只能整块算。
            table.items[generated::ALGORITHM_INDEX_City32]    = { ID::City32,    4,  false, ComputeHash_GoogleCityHash32,   nullptr };
            table.items[generated::ALGORITHM_INDEX_City64]    = { ID::City64,    8,  false, ComputeHash_GoogleCityHash64,   nullptr };
            table.items[generated::ALGORITHM_INDEX_City128]   = { ID::City128,  16,  false, ComputeHash_GoogleCityHash128,  nullptr };

            return table;
        }

        constexpr AlgorithmInfoTable ALGORITHM_INFO = MakeAlgorithmInfoTable();

        /// 编译期完整性校验：条目缺失 = compute 为空 / 摘要长度为 0。
        consteval bool ValidateAlgorithmTable()
        {
            for(size_t i = 0; i < generated::kAlgorithmCount; ++i)
            {
                if(ALGORITHM_INFO.items[i].compute == nullptr)
                    return false;

                if(ALGORITHM_INFO.items[i].digest_size == 0)
                    return false;

                // 「声称可分块」与「真的提供了分块器」必须一致：
                // 任何一边漏改都会在编译期暴露，而不是等到运行期算错。
                if(ALGORITHM_INFO.items[i].streamable != (ALGORITHM_INFO.items[i].create_hasher != nullptr))
                    return false;
            }

            return true;
        }

        static_assert(ValidateAlgorithmTable(),
                      "算法信息表存在空白条目：新增 Algorithm 枚举项后必须补上 "
                      "ALGORITHM_INFO 的对应行（摘要长度 / 是否可分块 / 计算函数）");

        constexpr bool IsValid(Algorithm algo)
        {
            return (size_t)algo < generated::kAlgorithmCount;
        }

        const AlgorithmInfo &Info(Algorithm algo)
        {
            return ALGORITHM_INFO.items[(size_t)algo];
        }
    }//namespace

    bool GetAlgorithmAt(size_t index, Algorithm &out)
    {
        if(index >= generated::kAlgorithmCount)
            return false;

        out = (Algorithm)index;
        return true;
    }

    size_t GetDigestSize(Algorithm algo)
    {
        return IsValid(algo) ? Info(algo).digest_size : 0;
    }

    const char *GetName(Algorithm algo)
    {
        return IsValid(algo) ? generated::kAlgorithmNames[(size_t)algo] : "";
    }

    uint32 GetID(Algorithm algo)
    {
        return IsValid(algo) ? Info(algo).id : 0;
    }

    bool IsStreamable(Algorithm algo)
    {
        return IsValid(algo) ? Info(algo).streamable : false;
    }

    bool TryParseAlgorithm(const char *name, Algorithm &out)
    {
        if(!name || !*name)
            return false;

        // 名称表来自 Clang AST；这里只做精确匹配，未知名称一律失败。
        for(size_t i = 0; i < generated::kAlgorithmCount; ++i)
        {
            if(strcmp(generated::kAlgorithmNames[i], name) == 0)
            {
                out = (Algorithm)i;
                return true;
            }
        }

        return false;
    }

    bool TryGetAlgorithmByID(uint32 id, Algorithm &out)
    {
        if(id == 0)
            return false;

        for(size_t i = 0; i < generated::kAlgorithmCount; ++i)
        {
            if(ALGORITHM_INFO.items[i].id == id)
            {
                out = (Algorithm)i;
                return true;
            }
        }

        return false;
    }

    bool Hash(Algorithm algo, const void *data, size_t size, void *hash_code)
    {
        if(!IsValid(algo) || !hash_code)
            return false;

        // 空输入是合法输入：MD5/SHA/BLAKE3 的空串摘要都有标准值。
        // data 允许为 nullptr，但必须 size==0。
        if(!data && size != 0)
            return false;

        Info(algo).compute(data, size, hash_code);
        return true;
    }

    void DigestToLowerHex(const void *digest, size_t digest_size, char *out_hex)
    {
        static constexpr char kHex[] = "0123456789abcdef";

        if(!digest || !out_hex)
            return;

        const uint8 *src = static_cast<const uint8 *>(digest);

        for(size_t i = 0; i < digest_size; ++i)
        {
            out_hex[i * 2]     = kHex[(src[i] >> 4) & 0x0F];
            out_hex[i * 2 + 1] = kHex[src[i] & 0x0F];
        }

        out_hex[digest_size * 2] = 0;
    }

    Hasher *CreateHasher(Algorithm algo)
    {
        if(!IsValid(algo))
            return nullptr;

        if(!Info(algo).create_hasher)
            return nullptr;

        return Info(algo).create_hasher();
    }

    void DestroyHasher(Hasher *hasher)
    {
        delete hasher;
    }
}//namespace hgl::util::hash
