#include<hgl/util/hash/Hash.h>
#include<hgl/util/hash/Hasher.h>
#include<hgl/util/hash/FNV1a.h>

namespace hgl::util::hash
{
    namespace
    {
        /**
         * FNV-1a 32 位。
         *
         * 摘要按 32 位整数写出（整数型摘要统一为小端，见
         * HashAlgorithmTable.cpp 的 HGL_ENDIAN 断言）。
         */
        class FNV1a32:public Base<FNV1a32, 4>
        {
            hgl::hash::FNV1aHasher32 hasher;

        public:

            FNV1a32() = default;

            void Init()
            {
                hasher.Reset();
            }

            void Update(const void *input, size_t inputLen)
            {
                hasher.AppendBytes(input, inputLen);
            }

            void Final(void *digest)
            {
                const uint32 value = hasher.Result();

                memcpy(digest, &value, 4);
            }
        };//class FNV1a32
    }//namespace

    void ComputeHash_FNV1a(const void* data, size_t size, void* result)
    {
        FNV1a32 h;

        h.Init();
        h.Update(data, size);
        h.Final(result);
    }

    Hasher *CreateStreamHasher_FNV1a()
    {
        return new detail::HasherImpl<FNV1a32>(Algorithm::FNV1a);
    }
}//namespace hgl::util::hash
