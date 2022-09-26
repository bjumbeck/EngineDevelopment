#pragma once

#include <string>

class ResourceCache;

/// @brief Encapsulates a single resource/asset. You can obtain a ResourceHandle by requesting one from the ResourceCache through
/// ResourceCache::GetResourceHandle(const std::string& fileName).
class ResourceHandle
{
    public:
        ResourceHandle(std::string fileName,
                           char* buffer,
                           const uint32_t size,
                           ResourceCache* resourceCache);
        virtual ~ResourceHandle();

        uint32_t GetSize() const { return size; }
        const char* GetBuffer() const { return buffer; }
        char* GetWritableBuffer() const { return buffer; }
        

    protected:
        std::string resourceFileName;
        char* buffer;
        uint32_t size;
        ResourceCache* resourceCache;

    private:
        // TODO: Ideally we should break the connection to ResourceCache to allow for easier refactoring in the future.
        friend class ResourceCache;
};