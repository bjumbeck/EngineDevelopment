/*
 * Copyright (C) 2012-2014 Alec Thomas <alec@swapoff.org>
 * All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.
 *
 * Author: Alec Thomas <alec@swapoff.org>
 */

#pragma once

#include <cstddef>
#include <cassert>
#include <vector>

/**
 * Provides a resizable, semi-contiguous pool of memory for constructing
 * objects in. Pointers into the pool will be invalided only when the pool is
 * destroyed.
 *
 * The semi-contiguous nature aims to provide cache-friendly iteration.
 *
 * Lookups are O(1).
 * Appends are amortized O(1).
 */
class BasePool
{
    public:
        explicit BasePool(std::size_t elementSize, std::size_t chunkSize = 8192)
            : elementSize(elementSize), chunkSize(chunkSize), totalCapacity(0) 
        {}
            
        virtual ~BasePool()
        {
            for (char* ptr : blocks)
                delete[] ptr;
        }

        std::size_t size() const { return totalSize; }
        std::size_t capacity() const { return totalCapacity; }
        std::size_t chunks() const { return blocks.size(); }

        /// Ensure at least expandSize elements will fit in the pool.
        inline void expand(std::size_t expandSize)
        {
            if (expandSize >= totalSize)
            {
                if (expandSize >= totalCapacity)
                    reserve(expandSize);
                
                totalSize = expandSize;
            }
        }

        inline void reserve(std::size_t reserveSize)
        {
            while (totalCapacity < reserveSize)
            {
                char* chunk = new char[elementSize * chunkSize];
                blocks.push_back(chunk);
                totalCapacity += chunkSize;
            }
        }

        inline void* get(std::size_t n)
        {
            assert(n < totalSize);
            return blocks[n / chunkSize] + (n % chunkSize) * elementSize;
        }

        inline const void* get(std::size_t n) const
        {
            assert(n < totalSize);
            return blocks[n / chunkSize] + (n % chunkSize) * elementSize;
        }

        virtual void destroy(std::size_t n) = 0;

    protected:
        std::vector<char*> blocks;
        std::size_t elementSize;
        std::size_t chunkSize;
        std::size_t totalSize = 0;
        std::size_t totalCapacity;
};

/**
* Implementation of BasePool that provides type-"safe" deconstruction of
* elements in the pool.
*/
template <typename T, std::size_t ChunkSize = 8192>
class Pool : public BasePool
{
    public:
        Pool() : BasePool(sizeof(T), ChunkSize) {}
        virtual ~Pool()
        {
            // Component destructors *must* be called by owner.
        }

        virtual void destroy(std::size_t n) override
        {
            assert(n < size());
            T* ptr = static_cast<T*>(get(n));
            ptr->~T();
        }
};
