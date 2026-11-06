# CMHash —— 引擎唯一的散列实现模块

CMHash 收拢了原先散落在 `CMCoreType`（wyhash / FNV1a / QuickHash）与
`CMUtil`（MD5 / SHA / BLAKE3 / xxHash / CityHash …）中的全部散列实现，
并提供由 **Clang AST 派生** 的算法注册表。

## 边界

```
CMCoreType  ←  CMHash  ←  CMCore  ←  CMUtil / CMAssetsManage / ...
```

* 依赖只有 **CMCoreType**：类型别名、`ENUM_CLASS_RANGE`、内存与十六进制工具。
* **没有**文件系统、没有流、没有 `hgl::String`。
* 文件/流的哈希封装在 `CMCore`：
  * `hgl/util/hash/HashFile.h` —— `HashFile()` / `HashFileHex()`
  * `hgl/io/HashInputStream.h` / `HashOutputStream.h` —— 边读写边算

这样分层的好处是：想给某个不依赖 CMCore 的小工具算哈希，直接链 CMHash 即可。

## 算法清单的唯一真源

`inc/hgl/util/hash/Algorithm.h` 里的 `enum class Algorithm` 是**唯一真源**。

`CMHash/inc/hgl/util/hash/generated/AlgorithmRegistry.gen.h` 由

```
scripts/run-hashgen.ps1          # 生成
scripts/run-hashgen.ps1 -Check   # 只校验（CI）
```

从 Clang AST 派生，产物提交进仓库（MSVC-only 构建不需要 Clang）。

生成物包含：

| 内容 | 用途 |
|------|------|
| `generated::kAlgorithmCount` | 算法总数，并与 `Algorithm::RANGE_SIZE` 做 `static_assert` |
| `generated::kAlgorithmNames[]` | 规范名称（对外显示名，如 `XXH3-64`） |
| `generated::ALGORITHM_INDEX_*` | 按枚举项键控的下标，供信息表初始化 |

**任何模块都不允许再手写算法名表。** 清单工具（`AssetManifestTool`）的
`--algo` 校验、`--help` 列表、错误提示全部直接遍历这份注册表。

## 不可从 AST 推断的信息

摘要长度、是否支持分块、计算函数、分块器工厂写在
`src/HashAlgorithmTable.cpp`，用生成的下标初始化：

```cpp
table.items[generated::ALGORITHM_INDEX_MD5] = { ID::MD5, 16, true,
                                               ComputeHash_MD5,
                                               CreateStreamHasher_MD5 };
```

`ValidateAlgorithmTable()` 是 `consteval` 的，并在 `static_assert` 中使用，
因此以下情况在**编译期**就会失败：

* 新增了枚举项却没有补信息表 → 条目为空；
* 声称 `streamable = true` 却没有分块器 → 两者不一致；
* 摘要长度为 0。

## 已修复的历史缺陷

| 问题 | 处理 |
|------|------|
| `FromName()` 未知名称静默返回 MD5 | 删除，改为 `TryParseAlgorithm()`，未命中返回 `false` |
| Murmur3 声明 16 字节却只写 4 字节 | 改为真实的 4 字节，并标记为不可分块 |
| xxHash `*_createState()` 从不释放 | RAII 释放，且检查分配失败 |
| 空输入直接返回 false | 空输入合法（各算法有确定的空串摘要） |
| `HashFile()` 只有声明、恒返回 false | 在 CMCore 实现，可分块算法 64KB 流式累加 |
| `Hash()` 的 `size` 是 32 位（>4GB 截断） | 全线改为 `size_t` |
| SHA1/SHA512 长度计数在超大输入下溢出 | SHA1 用 64 位位计数；SHA512 写完整 128 位长度 |
| CityHash128 丢弃返回值（恒为全 0） | 接住返回值，并改用规范入口 `CityHash128()` |
| CityHash64 使用带种子变体 | 改用规范入口 `CityHash64()` |

## 字节序约定

整数型摘要（Adler32 / CRC32 / FNV1a / xxHash / CityHash / Murmur3）
按机器整数的内存布局写出。`HashAlgorithmTable.cpp` 用
`static_assert(HGL_ENDIAN == HGL_LITTLE_ENDIAN, ...)` 把它钉成编译期约束：
接大端平台之前必须先补显式字节序转换，而不是让它悄悄产生不同摘要。

## 示例

```
CMHash/examples/WyHashTest           # 只依赖 CMCoreType，证明边界
CMCore/examples/hash/HashFileTest    # 算法清单来自注册表 + HashFileHex
CMCore/examples/hash/HashStreamTest  # HashInputStream 分块累加
```
