#include<hgl/util/hash/Hash.h>

namespace hgl::util::hash
{
    namespace
    {
        /**
         * MurmurHash3 x86_32（seed 由调用方给出）。
         *
         * 这是 32 位变体，**摘要长度就是 4 字节**；历史上本文件把它声明成
         * 16 字节，导致 12 字节从未被写入（读的人拿到的是未初始化数据）。
         * 这里按真实长度声明，需要 128 位请改用 XXH3-128 或 CityHash128。
         *
         * 同时提醒：该变体在收尾时会把总长度异或进状态，因此**不可分块累加**，
         * 只能一次性计算（能力表里 streamable=false）。
         */
        uint32 CountMurmurHash3(const void *key, size_t len, uint32 seed)
        {
            const uint8 *data = static_cast<const uint8 *>(key);
            const size_t nblocks = len / 4;
            uint32 h1 = seed;

            constexpr uint32 c1 = 0xcc9e2d51;
            constexpr uint32 c2 = 0x1b873593;

            // Body
            for(size_t i = 0; i < nblocks; ++i)
            {
                uint32 k1;

                memcpy(&k1, data + i * 4, 4);

                k1 *= c1;
                k1 = (k1 << 15) | (k1 >> (32 - 15));
                k1 *= c2;

                h1 ^= k1;
                h1 = (h1 << 13) | (h1 >> (32 - 13));
                h1 = h1 * 5 + 0xe6546b64;
            }

            // Tail
            const uint8 *tail = data + nblocks * 4;
            uint32 k1 = 0;

            switch(len & 3)
            {
                case 3:
                    k1 ^= (uint32)tail[2] << 16;
                    [[fallthrough]];
                case 2:
                    k1 ^= (uint32)tail[1] << 8;
                    [[fallthrough]];
                case 1:
                    k1 ^= tail[0];
                    k1 *= c1;
                    k1 = (k1 << 15) | (k1 >> (32 - 15));
                    k1 *= c2;
                    h1 ^= k1;
                    break;
                default:
                    break;
            }

            // Finalization（长度按规范取低 32 位）
            h1 ^= (uint32)len;
            h1 ^= h1 >> 16;
            h1 *= 0x85ebca6b;
            h1 ^= h1 >> 13;
            h1 *= 0xc2b2ae35;
            h1 ^= h1 >> 16;

            return h1;
        }
    }//namespace

    class MurmurHash3:public hash::Base<MurmurHash3, 4>
    {
        uint32 result;

    public:

        MurmurHash3() = default;

        void Init()
        {
            result=0;
        }

        void Update(const void *input,size_t inputLen)
        {
            // 一次性覆盖：本算法不支持分块累加，Base::Hash() 只会调用一次 Update。
            result=CountMurmurHash3(input,inputLen,result);
        }

        void Final(void *digest)
        {
            memcpy(digest,&result,4);
        }
    };//class MurmurHash3

    void ComputeHash_MurmurHash3(const void* data, size_t size, void* result)
    {
        MurmurHash3 h;
        h.Init();
        h.Update(data, size);
        h.Final(result);
    }

}//namespace hgl::util::hash
