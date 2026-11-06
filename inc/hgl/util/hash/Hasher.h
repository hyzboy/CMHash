#pragma once

#include<hgl/util/hash/Hash.h>

namespace hgl::util::hash
{
    /**
     * 运行期多态的散列器。
     *
     * 用途只有一个：让上层（文件/网络的流式哈希）在算法由运行时决定时，
     * 仍能分块喂数据，而不是把整个文件读进内存。
     *
     * 只有 `IsStreamable(algo) == true` 的算法才会产出 Hasher；对一次性算法
     * `CreateHasher()` 返回 nullptr，调用方应改用 `Hash()` 整块计算。
     */
    class Hasher
    {
    public:

        virtual ~Hasher() = default;

        virtual Algorithm GetAlgorithm() const = 0;
        virtual size_t    GetDigestSize() const = 0;

        virtual void Init() = 0;
        virtual void Update(const void *data, size_t size) = 0;
        virtual void Final(void *digest) = 0;
    };

    /// 创建分块散列器；算法非法、不支持分块或内存不足时返回 nullptr
    Hasher *CreateHasher(Algorithm algo);

    /// 销毁 CreateHasher() 返回的对象（可安全传入 nullptr）
    void DestroyHasher(Hasher *hasher);

    namespace detail
    {
        /**
         * 把 CMHash 内部的具体算法类包装成 Hasher。
         *
         * 各算法的实现类保持定义在各自的 .cpp 内（不污染公共头），
         * 只通过 `CreateStreamHasher_<Algo>()` 这个工厂函数暴露。
         */
        template<typename H>
        class HasherImpl final : public Hasher
        {
            H impl;

        public:

            explicit HasherImpl(Algorithm algo):algo(algo) {}

            Algorithm GetAlgorithm() const override { return algo; }
            size_t    GetDigestSize() const override { return H::DigestSize; }

            void Init() override { impl.Init(); }
            void Update(const void *data, size_t size) override { impl.Update(data, size); }
            void Final(void *digest) override { impl.Final(digest); }

        private:

            Algorithm algo;
        };
    }//namespace detail
}//namespace hgl::util::hash
