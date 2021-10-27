#pragma once
#ifndef FW_ALLOCATOR_ALLOCATOR_H
#define FW_ALLOCATOR_ALLOCATOR_H

#include <algorithm>
#include <functional>
#include <mutex>
#include <set>
#include <array>
#include <atomic>
#include <unordered_map>
#include <cassert>
#include <iostream>

#define ASSERT(condition)                                    \
    if (!(condition))                                        \
    {                                                        \
        std::cout << "Error of assertion for " << std::endl; \
        exit(-1);                                            \
    }

#undef min

namespace memory
{
    static const size_t kAlignment = 512;
    class BfcAllocator
    {
        friend BfcAllocator &Allocator();

    private:
        using BinIndex = int;
        static const int kInvalidBinInedx = -1;
        static const int kNumBins = 21;
        std::atomic_size_t currCached_{0};
        std::atomic_size_t peakCached_{0};
        std::atomic_size_t currAllocated_{0};
        std::atomic_size_t peakAllocated_{0};
        const size_t limit_;
        std::unordered_map<void *, size_t> allocatedMap_;

        struct Chunk
        {
            void *ptr = nullptr;
            size_t size = 0;

            mutable bool allocated = false;
            mutable const Chunk *prev = nullptr;
            mutable const Chunk *next = nullptr;
            mutable BinIndex binIdx = kInvalidBinInedx;

            explicit Chunk(void *ptr) :
                ptr(ptr)
            {
            }
            Chunk(void *ptr, size_t size) :
                ptr(ptr), size(size)
            {
            }

            bool inUse() const
            {
                return allocated;
            }

            bool isMonoblock() const
            {
                return !prev && !next;
            }

            bool operator<(const Chunk &c) const
            {
                return ptr < c.ptr;
            }
        };

        struct Bin
        {
            struct ChunkComparator
            {
                constexpr bool operator()(const Chunk *pa, const Chunk *pb) const
                {
                    return pa->size == pb->size ? pa->ptr < pb->ptr : pa->size < pb->size;
                }
            };
            using FreeChunkSet = std::set<const Chunk *, ChunkComparator>;
            FreeChunkSet freeChunks;
        };

        static const size_t kMinAllocationSize = kAlignment;
        static const size_t kExtendThreshold = kMinAllocationSize << kNumBins;

        size_t totalReservedBytes_;
        size_t currExtendBytes_;

        std::array<Bin, kNumBins> bins_;

        std::set<Chunk> chunkRegister_;

        using mutex_t = std::mutex;
        mutable mutex_t mut_;

    private:
        BfcAllocator(size_t limit = 8000000000) :
            limit_(limit)
        {
            totalReservedBytes_ = 0;
            currExtendBytes_ = roundBytes(0x100000);
        }

    private:
        size_t available() const noexcept
        {
            return limit_ - currCached_;
        }

        size_t roundBytes(size_t bytes) const noexcept
        {
            return ((bytes - 1) | (kMinAllocationSize - 1)) + 1;
        }

        void *allocateOnDevice(size_t nbytes)
        {
            void *ptr = (void *)malloc(nbytes);
            if (ptr == NULL)
            {
                std::cout << "Fatal error of allocate " << nbytes << " bytes" << std::endl;
                exit(0);
            }
            currCached_ += nbytes;
            if (currCached_ > peakCached_)
            {
                peakCached_.store(currCached_.load());
            }
            return ptr;
        }

        void releaseOnDevice(void *ptr, size_t nbytes)
        {
            currCached_ -= nbytes;

            free(ptr);
            ptr = NULL;
        }

        // try to allocate 'rounded_bytes' more bytes from devProxy_
        bool extend(size_t rounded_bytes);

        // release all unoccupied monoblocks
        void shrink();

        // find a free chunk in Bin `binIdx` to accommodate `rounded_bytes` bytes
        // return the raw pointer to the underlying memory
        void *findChunkPtr(BinIndex binIdx, size_t rounded_bytes);

        void insertChunkIntoBin(const Chunk *chunk);

        void removeChunkFromBin(const Chunk *chunk);

        void freeAndCoalesce(const Chunk *chunk);

        // merge two continuous free chunk into one
        // return the result chunk
        const Chunk *merge(const Chunk *c1, const Chunk *c2);

        // split one free chunk into two, the first chunk's size is `num_bytes`
        // insert the second chunk into bins_
        // return const pointer to the first chunk
        const Chunk *splitChunk(const Chunk *chunk, size_t num_bytes);

        BinIndex binIdxForSize(size_t bytes) const
        {
            uint64_t v = std::max<size_t>(bytes / kMinAllocationSize, 1);
            int b = std::min(kNumBins - 1, static_cast<int>(log2(v)));
            return b;
        }

        void *allocateRaw(size_t nbytes);

        void releaseRaw(void *ptr, size_t nbytes);

    public:
        void *allocate(size_t nbytes)
        {
            ASSERT(nbytes > 0);
            void *ptr = nullptr;
            ptr = allocateRaw(nbytes);
            ASSERT(nullptr != ptr);
            currAllocated_ += nbytes;
            if (currAllocated_ > peakAllocated_)
            {
                peakAllocated_.store(currAllocated_.load());
            }
            allocatedMap_.emplace(ptr, nbytes);
            return ptr;
        }

        void release(void *ptr)
        {
            ASSERT(ptr != nullptr);
            auto res = allocatedMap_.find(ptr);
            ASSERT(res != allocatedMap_.end());
            size_t nbytes = res->second;
            currAllocated_ -= nbytes;
            releaseRaw(ptr, nbytes);
        }

        size_t cachedBytes() const noexcept
        {
            return currCached_;
        }

        size_t allocatedBytes() const noexcept
        {
            return currAllocated_;
        }

        ~BfcAllocator();
    };

    inline BfcAllocator &Allocator()
    {
        static BfcAllocator a;
        return a;
    }

} // namespace memory

#endif // FW_ALLOCATOR_ALLOCATOR_H
