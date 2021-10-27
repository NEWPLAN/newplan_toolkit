/*************************************************************************
implementation of BFCAllocator
*************************************************************************/
#include "bfc_allocator.h"
#include <algorithm>
#include <cassert>

#undef min
#undef max

namespace memory
{

    BfcAllocator::~BfcAllocator()
    {
        shrink();
        // ASSERT(chunkRegister_.empty());
    }

    void *BfcAllocator::allocateRaw(size_t nbytes)
    {
        if (!nbytes)
            return nullptr;
        std::lock_guard<mutex_t> lk(mut_);
        size_t roundedBytes = roundBytes(nbytes);
        BinIndex binIdx = binIdxForSize(roundedBytes);
        void *ptr = findChunkPtr(binIdx, roundedBytes);

        if (ptr)
        {
            return ptr;
        }

        // Try to extend
        if (extend(roundedBytes))
        {
            ptr = findChunkPtr(binIdx, roundedBytes);
            if (ptr)
            {
                return ptr;
            }
        }

        std::cout << "BfcAllocator::allocate: memory allocation failed. Try to allocate "
                  << roundedBytes << " bytes" << std::endl;
        return nullptr;
    }

    bool BfcAllocator::extend(size_t roundedBytes)
    {
        bool increased = false;
        while (currExtendBytes_ < roundedBytes && currExtendBytes_ < kExtendThreshold)
        {
            currExtendBytes_ *= 2;
            increased = true;
        }

        size_t nbytes = std::max(roundedBytes, currExtendBytes_);

        // release unoccupied monoblocks to increase available size.
        if (available() < nbytes)
        {
            shrink();
        }

        nbytes = std::min(nbytes, available());
        // try allocating.
        void *mem_addr = nullptr;
        while (nbytes >= roundedBytes)
        {
            try
            {
                mem_addr = allocateOnDevice(nbytes);
            }
            // catch (const FwException<ErrorCode::AllocFailed> &)
            // {
            // 	mem_addr = nullptr;
            // }
            catch (...)
            {
                std::cout << "catch (...)" << std::endl;
                mem_addr = nullptr;
            }
            if (mem_addr)
                break;
            // try allocating less memory.
            constexpr double kBackpedalFactor = .9;
            size_t lessBytes = roundBytes(nbytes * kBackpedalFactor);
            if (lessBytes >= nbytes)
            {
                break;
            }
            else
            {
                nbytes = lessBytes;
            }
        }

        if (!mem_addr)
        {
            return false;
        }

        if (!increased && currExtendBytes_ < kExtendThreshold)
        {
            // increase the size of the next required allocation.
            currExtendBytes_ *= 2;
        }

        totalReservedBytes_ += nbytes;
        auto res = chunkRegister_.emplace(mem_addr, nbytes);
        ASSERT(res.second);
        const Chunk *chunk = &*res.first;
        insertChunkIntoBin(chunk);
        return true;
    }

    void BfcAllocator::shrink()
    {
        for (auto it = chunkRegister_.begin(); it != chunkRegister_.end();)
        {
            if (!it->inUse() && it->isMonoblock())
            {
                ASSERT(it->binIdx != kInvalidBinInedx);
                removeChunkFromBin(&*it);
                releaseOnDevice(it->ptr, it->size);
                totalReservedBytes_ -= it->size;
                it = chunkRegister_.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void BfcAllocator::releaseRaw(void *ptr, size_t nbytes)
    {
        if (!ptr)
            return;
        std::lock_guard<mutex_t> lk(mut_);
        auto it = chunkRegister_.find(Chunk(ptr, nbytes));
        ASSERT(it != chunkRegister_.end());
        freeAndCoalesce(&*it);
    }

    void *BfcAllocator::findChunkPtr(BinIndex binIdx, size_t roundedBytes)
    {
        for (; binIdx < kNumBins; ++binIdx)
        {
            Bin &bin = bins_[binIdx];

            const Chunk key = Chunk(nullptr, roundedBytes);
            auto it = bin.freeChunks.lower_bound(&key);
            if (it != bin.freeChunks.end())
            {
                const Chunk *chunk = *it;
                removeChunkFromBin(chunk);

                constexpr uint64_t kMaxInternalFragmentation = 128 << 20; // 128Mib
                if (chunk->size >= roundedBytes * 2 || chunk->size >= roundedBytes + kMaxInternalFragmentation)
                {
                    chunk = splitChunk(chunk, roundedBytes);
                }

                chunk->allocated = true;
                return chunk->ptr;
            }
        }

        return nullptr;
    }

    const BfcAllocator::Chunk *BfcAllocator::splitChunk(const Chunk *chunk,
                                                        size_t num_bytes)
    {
        ASSERT(!chunk->inUse());
        ASSERT(chunk->binIdx == kInvalidBinInedx);
        ASSERT(0 < num_bytes && num_bytes <= chunk->size);

        const Chunk *prev = chunk->prev;
        const Chunk *next = chunk->next;

        void *const p1 = chunk->ptr;
        void *const p2 = static_cast<char *>(chunk->ptr) + num_bytes;
        size_t const s1 = num_bytes;
        size_t const s2 = chunk->size - num_bytes;
        ASSERT(chunkRegister_.erase(*chunk));

        auto res = chunkRegister_.emplace(p1, s1);
        ASSERT(res.second);
        const Chunk *c1 = &*res.first;
        res = chunkRegister_.emplace(p2, s2);
        ASSERT(res.second);
        const Chunk *c2 = &*res.first;

        c1->next = c2;
        c2->prev = c1;
        if ((c1->prev = prev))
            prev->next = c1;
        if ((c2->next = next))
            next->prev = c2;

        insertChunkIntoBin(c2);

        return c1;
    }

    void BfcAllocator::insertChunkIntoBin(const Chunk *chunk)
    {
        ASSERT(!chunk->inUse() && (chunk->binIdx == kInvalidBinInedx));
        chunk->binIdx = binIdxForSize(chunk->size);
        ASSERT(bins_[chunk->binIdx].freeChunks.insert(chunk).second);
    }

    void BfcAllocator::removeChunkFromBin(const Chunk *chunk)
    {
        ASSERT(!chunk->inUse() && (chunk->binIdx != kInvalidBinInedx));
        ASSERT(bins_[chunk->binIdx].freeChunks.erase(chunk));
        chunk->binIdx = kInvalidBinInedx;
    }

    void BfcAllocator::freeAndCoalesce(const Chunk *chunk)
    {
        ASSERT(chunk->inUse() && (chunk->binIdx == kInvalidBinInedx));

        chunk->allocated = false;

        // If the next chunk is free, coalesce with it
        const Chunk *next = chunk->next;
        if (next && !next->inUse())
        {
            removeChunkFromBin(next);
            chunk = merge(chunk, next);
        }

        // If the previous chunk is free, coalesce with it
        const Chunk *prev = chunk->prev;
        if (prev && !prev->inUse())
        {
            removeChunkFromBin(prev);
            chunk = merge(prev, chunk);
        }

        insertChunkIntoBin(chunk);
    }

    const BfcAllocator::Chunk *BfcAllocator::merge(const Chunk *c1,
                                                   const Chunk *c2)
    {
        ASSERT(!c1->inUse() && !c2->inUse());
        ASSERT(c1 == c2->prev && c2 == c1->next);
        const Chunk *prev = c1->prev;
        const Chunk *next = c2->next;
        void *const p = c1->ptr;
        size_t const s = c1->size + c2->size;
        ASSERT(chunkRegister_.erase(*c1));
        ASSERT(chunkRegister_.erase(*c2));

        auto res = chunkRegister_.emplace(p, s);
        ASSERT(res.second);
        const Chunk *c3 = &*res.first;
        if ((c3->prev = prev))
            prev->next = c3;
        if ((c3->next = next))
            next->prev = c3;

        return c3;
    }

} // namespace memory
