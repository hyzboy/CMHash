#pragma once

#include<hgl/CoreType.h>
#include<hgl/platform/Platform.h>
#include<hgl/type/EnumUtil.h>

namespace hgl::util::hash
{
    /**
     * Hash 算法标识常量。
     *
     * 供清单/序列化等需要把算法写进二进制或文本的场合使用，保证跨版本稳定。
     * 名称由 Clang AST 派生（见 src/Tools/ASTTooling），此处不重复维护文本名。
     */
    namespace ID
    {
        constexpr uint32 Adler32       = HGL_MERGE32('A','d','l','r');
        constexpr uint32 CRC32         = HGL_MERGE32('C','R','C','3');
        constexpr uint32 MD4           = HGL_MERGE32('M','D','4',' ');
        constexpr uint32 MD5           = HGL_MERGE32('M','D','5',' ');
        constexpr uint32 SHA1          = HGL_MERGE32('S','H','A','1');
        constexpr uint32 SHA1LE        = HGL_MERGE32('S','H','L','E');
        constexpr uint32 SHA256        = HGL_MERGE32('S','H','2','5');
        constexpr uint32 SHA512        = HGL_MERGE32('S','H','5','1');
        constexpr uint32 BLAKE3        = HGL_MERGE32('B','L','K','3');
        constexpr uint32 XXH32         = HGL_MERGE32('X','X','H','2');
        constexpr uint32 XXH64         = HGL_MERGE32('X','X','H','6');
        constexpr uint32 XXH3_64       = HGL_MERGE32('X','3','6','4');
        constexpr uint32 XXH3_128      = HGL_MERGE32('X','3','1','2');
        constexpr uint32 FNV1a         = HGL_MERGE32('F','N','V','1');
        constexpr uint32 Murmur3       = HGL_MERGE32('M','u','r','m');
        constexpr uint32 City32        = HGL_MERGE32('C','i','3','2');
        constexpr uint32 City64        = HGL_MERGE32('C','i','6','4');
        constexpr uint32 City128       = HGL_MERGE32('C','i','1','2');
    }//namespace ID

    /**
     * Hash 算法枚举 —— **唯一真源**。
     *
     * 规则：
     *   · 本枚举的完整成员列表由 Clang AST 扫描得出，工具生成
     *     `hgl/util/hash/generated/AlgorithmRegistry.gen.h`；
     *   · 名称、数量、严格解析全部来自生成物，任何模块不得再手写算法名表；
     *   · 新增算法只需在此处添加枚举项（及 ID 常量），随后运行
     *     `scripts/run-hashgen.ps1` 重新生成并补齐算法信息表。
     *
     * `ENUM_CLASS_RANGE` 引入的 `BEGIN_RANGE` / `END_RANGE` / `RANGE_SIZE`
     * 属于哨兵，生成器会跳过它们，但会用 `RANGE_SIZE` 与真实成员数量做
     * 编译期一致性断言。
     */
    enum class Algorithm            ///Hash算法枚举
    {
        Adler32,                ///<一种较CRC32更为安全的快速HASH算法
        CRC32,                  ///<最快速的HASH算法，但最不安全，仅用于低安全性的简单验证
        MD4,                    ///<较MD5在安全性下稍弱。但仍未被攻破，且较MD5更快
        MD5,                    ///<最常用的HASH算法

        SHA1,                   ///<较MD5更为安全，但计算较慢
        SHA1LE,                 ///<SHA1改版
        SHA256,
        SHA512,

        BLAKE3,                 ///<BLAKE3 (256-bit)

        XXH32,
        XXH64,
        XXH3_64,
        XXH3_128,

        FNV1a,
        Murmur3,

        City32,
        City64,
        City128,

        ENUM_CLASS_RANGE(Adler32,City128)
    };//enum class Algorithm

    // 向后兼容别名
    using HASH = Algorithm;
}//namespace hgl::util::hash
