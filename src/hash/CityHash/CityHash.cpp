// source codes repository: https://github.com/google/cityhash.git

// vcpkg： cityhash, cityhash[sse]

#include<hgl/util/hash/Hash.h>

#include"city.h"

namespace hgl::util::hash
{
    class GoogleCityHash32:public Base<GoogleCityHash32, 4>
    {
        uint32 result;

    public:

        GoogleCityHash32() = default;
        void Init()
        {
            result=0;
        }
        void Update(const void *input,size_t inputLen)
        {
            result=CityHash32((const char *)input,inputLen);
        }
        void Final(void *digest)
        {
            *(uint32 *)digest=result;
        }
    };

    void ComputeHash_GoogleCityHash32(const void* data, size_t size, void* result)
    {
        GoogleCityHash32 h;
        h.Init();
        h.Update(data, size);
        h.Final(result);
    }

    class GoogleCityHash64:public Base<GoogleCityHash64, 8>
    {
        uint64 result;

    public:

        GoogleCityHash64() = default;
        void Init()
        {
            result=0;
        }
        void Update(const void *input,size_t inputLen)
        {
            // CityHash has no incremental API: compute in one shot with the
            // canonical CityHash64 entry point so values match upstream vectors.
            result=::CityHash64((const char *)input,inputLen);
        }
        void Final(void *digest)
        {
            *(uint64 *)digest=result;
        }
    };

    void ComputeHash_GoogleCityHash64(const void* data, size_t size, void* result)
    {
        GoogleCityHash64 h;
        h.Init();
        h.Update(data, size);
        h.Final(result);
    }

    class GoogleCityHash128:public Base<GoogleCityHash128, 16>
    {
        uint128 result;

    public:

        GoogleCityHash128() = default;
        void Init()
        {
            result.first=0;
            result.second=0;
        }
        void Update(const void *input,size_t inputLen)
        {
            // The old implementation discarded the return value, so the result
            // was always all zeros; the value must be captured here.
            result=::CityHash128((const char *)input,inputLen);
        }
        void Final(void *digest)
        {
            ((uint64 *)digest)[0]=result.first;
            ((uint64 *)digest)[1]=result.second;
        }
    };//class CityHash128:public Hash

    void ComputeHash_GoogleCityHash128(const void* data, size_t size, void* result)
    {
        GoogleCityHash128 h;
        h.Init();
        h.Update(data, size);
        h.Final(result);
    }

}//namespace hgl::util::hash
