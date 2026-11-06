#include<hgl/CoreType.h>
#include<hgl/util/hash/QuickHash.h>

#include<iostream>
#include<vector>
#include<string>
#include<array>
#include<cassert>
#include<cstring>

using namespace hgl;
using namespace std;

namespace
{
    uint64 HashBytes(const void *data, size_t len, uint64 seed = 0)
    {
        return wyhash(data, len, seed, _wyp);
    }

    void ExpectEqual(uint64 a, uint64 b, const char *hint)
    {
        if (a != b)
        {
            cerr << "[FAIL] " << hint << " expected equal" << endl;
            assert(a == b);
        }
    }

    void ExpectNotEqual(uint64 a, uint64 b, const char *hint)
    {
        if (a == b)
        {
            cerr << "[FAIL] " << hint << " expected not equal" << endl;
            assert(a != b);
        }
    }

    void TestDeterminism()
    {
        const char *s = "wyhash-determinism";
        uint64 h1 = HashBytes(s, strlen(s));
        uint64 h2 = HashBytes(s, strlen(s));
        ExpectEqual(h1, h2, "Determinism");
    }

    void TestStringVectors()
    {
        const std::array<std::string, 8> samples = {
            "",
            "a",
            "abc",
            "message digest",
            "abcdefghijklmnopqrstuvwxyz",
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
            "The quick brown fox jumps over the lazy dog",
            "The quick brown fox jumps over the lazy dog."
        };

        for (size_t i = 0; i < samples.size(); ++i)
        {
            uint64 h1 = HashBytes(samples[i].data(), samples[i].size());
            uint64 h2 = HashBytes(samples[i].data(), samples[i].size());
            ExpectEqual(h1, h2, "String determinism");

            if (i > 0)
            {
                uint64 prev = HashBytes(samples[i - 1].data(), samples[i - 1].size());
                ExpectNotEqual(h1, prev, "Different strings");
            }
        }
    }

    void TestLengthSensitivity()
    {
        std::array<uint8_t, 256> buf{};
        for (size_t i = 0; i < buf.size(); ++i)
            buf[i] = static_cast<uint8_t>(i);

        uint64 h16 = HashBytes(buf.data(), 16);
        uint64 h17 = HashBytes(buf.data(), 17);
        uint64 h31 = HashBytes(buf.data(), 31);
        uint64 h32 = HashBytes(buf.data(), 32);
        uint64 h63 = HashBytes(buf.data(), 63);
        uint64 h64 = HashBytes(buf.data(), 64);

        ExpectNotEqual(h16, h17, "Length 16 vs 17");
        ExpectNotEqual(h31, h32, "Length 31 vs 32");
        ExpectNotEqual(h63, h64, "Length 63 vs 64");
    }

    void TestPrefixIndependence()
    {
        std::array<uint8_t, 64> a{};
        std::array<uint8_t, 64> b{};
        for (size_t i = 0; i < a.size(); ++i)
            a[i] = b[i] = static_cast<uint8_t>(i * 3 + 1);

        // Change suffix
        for (size_t i = 32; i < b.size(); ++i)
            b[i] ^= 0x5a;

        uint64 h1 = HashBytes(a.data(), 32);
        uint64 h2 = HashBytes(b.data(), 32);

        ExpectEqual(h1, h2, "Prefix hash should ignore suffix");
    }

    void TestSeedVariation()
    {
        const char *s = "seed-variation";
        uint64 h0 = HashBytes(s, strlen(s), 0);
        uint64 h1 = HashBytes(s, strlen(s), 1);
        ExpectNotEqual(h0, h1, "Seed variation");
    }

    void TestUnalignedAccess()
    {
        std::array<uint8_t, 64> buf{};
        for (size_t i = 0; i < buf.size(); ++i)
            buf[i] = static_cast<uint8_t>(0xA5 ^ i);

        uint64 h1 = HashBytes(buf.data() + 1, 31);
        uint64 h2 = HashBytes(buf.data() + 1, 31);
        ExpectEqual(h1, h2, "Unaligned access determinism");

        uint64 h3 = HashBytes(buf.data() + 2, 31);
        ExpectNotEqual(h1, h3, "Unaligned different offset");
    }

    void TestComputeOptimalHash()
    {
        int v = 12345;
        uint64 hv = ComputeOptimalHash(v);
        ExpectEqual(hv, static_cast<uint64>(v), "Integral identity");

        void *p = reinterpret_cast<void *>(0x12345678);
        uint64 hp = ComputeOptimalHash(p);
        ExpectEqual(hp, reinterpret_cast<uint64>(p), "Pointer identity");

        struct POD
        {
            int a;
            uint64 b;
        } pod{42, 0x1122334455667788ull};

        uint64 h1 = ComputeOptimalHash(pod);
        uint64 h2 = ComputeOptimalHash(pod);
        ExpectEqual(h1, h2, "POD wyhash");
    }

    void TestBulkData()
    {
        std::vector<uint8_t> data(1024);
        for (size_t i = 0; i < data.size(); ++i)
            data[i] = static_cast<uint8_t>((i * 131) & 0xFF);

        uint64 h1 = HashBytes(data.data(), data.size());
        uint64 h2 = HashBytes(data.data(), data.size());
        ExpectEqual(h1, h2, "Large buffer determinism");

        data[511] ^= 0xFF;
        uint64 h3 = HashBytes(data.data(), data.size());
        ExpectNotEqual(h1, h3, "Large buffer change");
    }
}

int main(int, char **)
{
    cout << "[QuickHashTest] WYHASH test start" << endl;

    TestDeterminism();
    TestStringVectors();
    TestLengthSensitivity();
    TestPrefixIndependence();
    TestSeedVariation();
    TestUnalignedAccess();
    TestComputeOptimalHash();
    TestBulkData();

    cout << "[QuickHashTest] All tests passed" << endl;
    return 0;
}
