#include"xxHash/xxh3.h"
#include<hgl/util/hash/Hash.h>
#include<hgl/util/hash/Hasher.h>

namespace hgl::util::hash
{
    // ── xxHash 家族 ─────────────────────────────────────────────────────────
    //
    // 历史实现有两个确定缺陷：
    //   · XXH*_createState() 分配的 state 从未释放（每算一次泄漏一块）；
    //   · 构造函数默认产生的 state 为空，Init() 失败后 Update 会解引用空指针。
    //
    // 这里用 RAII 持有 state，并在 Init 中检查分配结果；对象不可拷贝，
    // 避免同一块 state 被两个对象重复释放。

    class xxHash32:public hash::Base<xxHash32, 4>
    {
        XXH32_state_t *state=nullptr;

    public:

        xxHash32()=default;
        ~xxHash32(){ if(state) XXH32_freeState(state); }

        xxHash32(const xxHash32 &)=delete;
        xxHash32 &operator=(const xxHash32 &)=delete;

        bool IsValid() const { return state!=nullptr; }

        void Init()
        {
            if(!state)
                state=XXH32_createState();

            if(state)
                XXH32_reset(state,0);
        }

        void Update(const void *input,size_t inputLen)
        {
            if(state && inputLen>0)
                XXH32_update(state,input,inputLen);
        }

        void Final(void *digest)
        {
            const XXH32_hash_t value = state ? XXH32_digest(state) : 0;

            memcpy(digest,&value,4);
        }
    };//class xxHash32

    void ComputeHash_XXH32(const void* data, size_t size, void* result)
    {
        xxHash32 h;
        h.Init();
        h.Update(data, size);
        h.Final(result);
    }

    class xxHash64:public hash::Base<xxHash64, 8>
    {
        XXH64_state_t *state=nullptr;

    public:

        xxHash64()=default;
        ~xxHash64(){ if(state) XXH64_freeState(state); }

        xxHash64(const xxHash64 &)=delete;
        xxHash64 &operator=(const xxHash64 &)=delete;

        bool IsValid() const { return state!=nullptr; }

        void Init()
        {
            if(!state)
                state=XXH64_createState();

            if(state)
                XXH64_reset(state,0);
        }

        void Update(const void *input,size_t inputLen)
        {
            if(state && inputLen>0)
                XXH64_update(state,input,inputLen);
        }

        void Final(void *digest)
        {
            const XXH64_hash_t value = state ? XXH64_digest(state) : 0;

            memcpy(digest,&value,8);
        }
    };//class xxHash64

    void ComputeHash_XXH64(const void* data, size_t size, void* result)
    {
        xxHash64 h;
        h.Init();
        h.Update(data, size);
        h.Final(result);
    }

    class xxHash3_64:public hash::Base<xxHash3_64, 8>
    {
        XXH3_state_t *state=nullptr;

    public:

        xxHash3_64()=default;
        ~xxHash3_64(){ if(state) XXH3_freeState(state); }

        xxHash3_64(const xxHash3_64 &)=delete;
        xxHash3_64 &operator=(const xxHash3_64 &)=delete;

        bool IsValid() const { return state!=nullptr; }

        void Init()
        {
            if(!state)
                state=XXH3_createState();

            if(state)
                XXH3_64bits_reset_withSeed(state,0);
        }

        void Update(const void *input,size_t inputLen)
        {
            if(state && inputLen>0)
                XXH3_64bits_update(state,input,inputLen);
        }

        void Final(void *digest)
        {
            const XXH64_hash_t value = state ? XXH3_64bits_digest(state) : 0;

            memcpy(digest,&value,8);
        }
    };//class xxHash3_64

    void ComputeHash_XXH3_64(const void* data, size_t size, void* result)
    {
        xxHash3_64 h;
        h.Init();
        h.Update(data, size);
        h.Final(result);
    }

    class xxHash3_128:public hash::Base<xxHash3_128, 16>
    {
        XXH3_state_t *state=nullptr;

    public:

        xxHash3_128()=default;
        ~xxHash3_128(){ if(state) XXH3_freeState(state); }

        xxHash3_128(const xxHash3_128 &)=delete;
        xxHash3_128 &operator=(const xxHash3_128 &)=delete;

        bool IsValid() const { return state!=nullptr; }

        void Init()
        {
            if(!state)
                state=XXH3_createState();

            if(state)
                XXH3_128bits_reset_withSeed(state,0);
        }

        void Update(const void *input,size_t inputLen)
        {
            if(state && inputLen>0)
                XXH3_128bits_update(state,input,inputLen);
        }

        void Final(void *digest)
        {
            // XXH128_hash_t 就是 16 字节（low64 + high64），按内存写出。
            XXH128_hash_t value;

            if(state)
                value = XXH3_128bits_digest(state);
            else
            {
                value.low64  = 0;
                value.high64 = 0;
            }

            memcpy(digest,&value,16);
        }
    };//class xxHash3_128

    void ComputeHash_XXH3_128(const void* data, size_t size, void* result)
    {
        xxHash3_128 h;
        h.Init();
        h.Update(data, size);
        h.Final(result);
    }

}//namespace hgl::util::hash

namespace hgl::util::hash
{
    Hasher *CreateStreamHasher_XXH32()
    {
        return new detail::HasherImpl<xxHash32>(Algorithm::XXH32);
    }

    Hasher *CreateStreamHasher_XXH64()
    {
        return new detail::HasherImpl<xxHash64>(Algorithm::XXH64);
    }

    Hasher *CreateStreamHasher_XXH3_64()
    {
        return new detail::HasherImpl<xxHash3_64>(Algorithm::XXH3_64);
    }

    Hasher *CreateStreamHasher_XXH3_128()
    {
        return new detail::HasherImpl<xxHash3_128>(Algorithm::XXH3_128);
    }
}//namespace hgl::util::hash
