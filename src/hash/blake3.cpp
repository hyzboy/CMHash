#include<hgl/util/hash/Hash.h>
#include<hgl/util/hash/Hasher.h>
#include<blake3.h>

namespace hgl::util::hash
{
    class Blake3:public Base<Blake3, 32>
    {
        blake3_hasher hasher{};

    public:

        Blake3() = default;

        void Init()
        {
            blake3_hasher_init(&hasher);
        }

        void Update(const void *input,size_t inputLen)
        {
            if(!input || inputLen==0) return;
            blake3_hasher_update(&hasher, input, inputLen);
        }

        void Final(void *digest)
        {
            if(!digest) return;
            blake3_hasher_finalize(&hasher, static_cast<uint8_t *>(digest), 32);
        }
    };//class Blake3

    void ComputeHash_Blake3(const void* data, size_t size, void* result)
    {
        Blake3 h;
        h.Init();
        h.Update(data, size);
        h.Final(result);
    }
}//namespace hgl::util::hash

namespace hgl::util::hash
{
    Hasher *CreateStreamHasher_BLAKE3()
    {
        return new detail::HasherImpl<Blake3>(Algorithm::BLAKE3);
    }
}//namespace hgl::util::hash
