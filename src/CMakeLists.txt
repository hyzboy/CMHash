#################################################################################################### CMHash
#
# 本模块是引擎唯一的散列实现来源，只依赖 CMCoreType。
# 文件/流的哈希封装在 CMCore（hgl/util/hash/HashFile.h），不在这里。

option(CM_HASH_SUPPORT_BLAKE3 "Build BLAKE3 support" ON)

SET(HASH_INCLUDE_PATH ${CMHASH_ROOT_INCLUDE_PATH}/hgl/util/hash)

SET(HASH_PUBLIC_HEADERS
        ${HASH_INCLUDE_PATH}/Algorithm.h
        ${HASH_INCLUDE_PATH}/Hash.h
        ${HASH_INCLUDE_PATH}/FNV1a.h
        ${HASH_INCLUDE_PATH}/QuickHash.h
        ${HASH_INCLUDE_PATH}/SecureHash.h
        ${HASH_INCLUDE_PATH}/HashMergeGolden64.h
        ${HASH_INCLUDE_PATH}/SHA1LE.h
        ${HASH_INCLUDE_PATH}/wyhash.h
        ${HASH_INCLUDE_PATH}/wyhash32.h
        ${HASH_INCLUDE_PATH}/generated/AlgorithmRegistry.gen.h
)

SOURCE_GROUP("Hash\\Headers" FILES ${HASH_PUBLIC_HEADERS})

SET(HASH_XXHASH_FILES   hash/xxHash/xxh3.h
                        hash/xxHash/xxhash.h
                        hash/xxHash/xxhash.c)

SET(HASH_CITYHASH_FILES hash/CityHash/CityHashConfig.h
                        hash/CityHash/city.h
                        hash/CityHash/citycrc.h
                        hash/CityHash/city.cc
                        hash/CityHash/CityHash.cpp)

SET(HASH_SOURCE_FILES   HashAlgorithmTable.cpp

                        hash/adler32.cpp
                        hash/crc32.cpp
                        hash/FNV1a.cpp

                        hash/md4.cpp
                        hash/md5.cpp

                        hash/sha1.cpp
                        hash/sha1le.cpp
                        hash/sha256.cpp
                        hash/sha512.cpp

                        hash/xxHash3.cpp
                        hash/MurmurHash3.cpp)

IF(CM_HASH_SUPPORT_BLAKE3)
    SET(HASH_SOURCE_FILES ${HASH_SOURCE_FILES} hash/blake3.cpp)
ENDIF(CM_HASH_SUPPORT_BLAKE3)

SOURCE_GROUP("Hash\\Basic"        FILES HashAlgorithmTable.cpp hash/adler32.cpp hash/crc32.cpp hash/FNV1a.cpp)
SOURCE_GROUP("Hash\\Algorithms\\MD"    FILES hash/md4.cpp hash/md5.cpp)
SOURCE_GROUP("Hash\\Algorithms\\SHA"   FILES hash/sha1.cpp hash/sha1le.cpp hash/sha256.cpp hash/sha512.cpp)
SOURCE_GROUP("Hash\\Algorithms\\xxHash" FILES hash/xxHash3.cpp ${HASH_XXHASH_FILES})
SOURCE_GROUP("Hash\\Algorithms\\CityHash" FILES ${HASH_CITYHASH_FILES})
SOURCE_GROUP("Hash\\Algorithms\\Murmur" FILES hash/MurmurHash3.cpp)

add_cm_library(CMHash "CM"  ${CMHASH_ROOT_INCLUDE_PATH}
                            ${HASH_PUBLIC_HEADERS}
                            ${HASH_SOURCE_FILES}
                            ${HASH_XXHASH_FILES}
                            ${HASH_CITYHASH_FILES}
                           )

set_target_properties(CMHash PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON)

# CMCoreType 只提供头文件（类型/枚举工具），这里显式链接以固定构建顺序。
target_link_libraries(CMHash PUBLIC CMCoreType)

IF(CM_HASH_SUPPORT_BLAKE3)
    find_package(blake3 CONFIG REQUIRED)
    target_link_libraries(CMHash PRIVATE BLAKE3::blake3)
ENDIF(CM_HASH_SUPPORT_BLAKE3)
