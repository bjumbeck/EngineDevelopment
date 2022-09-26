#include "ResourceHandle.hpp"

#include <string>

#include "ResourceCache.hpp"

ResourceHandle::ResourceHandle(std::string fileName,
                               char* buffer,
                               const uint32_t size,
                               ResourceCache* resourceCache) : resourceFileName{std::move(fileName)}
                                                               , buffer{buffer}
                                                               , size{size}
                                                               , resourceCache{resourceCache}
{
}

ResourceHandle::~ResourceHandle()
{
    if (buffer)
    {
        delete[] buffer;
        buffer = nullptr;
    }

    resourceCache->MemoryHasBeenFreed(size);
}
