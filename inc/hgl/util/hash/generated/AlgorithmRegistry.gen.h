// 本文件由 Clang AST 自动生成，请勿手工编辑。
//
//   生成器：src/Tools/ASTTooling/enum_gen.py
//   重新生成：scripts/run-hashgen.ps1
//   校验一致：scripts/run-hashgen.ps1 -Check
//
// 真源：CMHash/inc/hgl/util/hash/Algorithm.h 中的 enum Algorithm
// 成员列表、数量与顺序全部直接来自 AST，不存在第二份手工清单。
#pragma once

#include<hgl/CoreType.h>
#include<hgl/util/hash/Algorithm.h>

namespace hgl::util::hash
{
    namespace generated
    {
        /// enum Algorithm 的真实成员数量（已剔除 RANGE 哨兵）
        constexpr size_t kAlgorithmCount = 18;

        /// 与枚举声明顺序一一对应的规范名称
        constexpr const char *const kAlgorithmNames[kAlgorithmCount] =
        {
            "Adler32",
            "CRC32",
            "MD4",
            "MD5",
            "SHA1",
            "SHA1LE",
            "SHA256",
            "SHA512",
            "BLAKE3",
            "XXH32",
            "XXH64",
            "XXH3-64",
            "XXH3-128",
            "FNV1a",
            "MurmurHash3",
            "CityHash32",
            "CityHash64",
            "CityHash128",
        };

        /// 每个枚举项的稳定索引，供按索引初始化的信息表使用
        enum AlgorithmIndex : size_t
        {
            ALGORITHM_INDEX_Adler32 = 0,
            ALGORITHM_INDEX_CRC32 = 1,
            ALGORITHM_INDEX_MD4 = 2,
            ALGORITHM_INDEX_MD5 = 3,
            ALGORITHM_INDEX_SHA1 = 4,
            ALGORITHM_INDEX_SHA1LE = 5,
            ALGORITHM_INDEX_SHA256 = 6,
            ALGORITHM_INDEX_SHA512 = 7,
            ALGORITHM_INDEX_BLAKE3 = 8,
            ALGORITHM_INDEX_XXH32 = 9,
            ALGORITHM_INDEX_XXH64 = 10,
            ALGORITHM_INDEX_XXH3_64 = 11,
            ALGORITHM_INDEX_XXH3_128 = 12,
            ALGORITHM_INDEX_FNV1a = 13,
            ALGORITHM_INDEX_Murmur3 = 14,
            ALGORITHM_INDEX_City32 = 15,
            ALGORITHM_INDEX_City64 = 16,
            ALGORITHM_INDEX_City128 = 17,
        };
    }//namespace generated

    static_assert(size_t(Algorithm::RANGE_SIZE) == generated::kAlgorithmCount,
                  "Algorithm 的 RANGE_SIZE 与 Clang AST 扫描到的成员数量不一致：枚举声明与生成物已经脱节，请重新运行 scripts/run-hashgen.ps1");

}//namespace hgl::util::hash
