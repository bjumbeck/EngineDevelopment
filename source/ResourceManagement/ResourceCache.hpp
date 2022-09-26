#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <map>
#include <regex>

class ResourceHandle;
class IResourceFileLoader;
class IResourceContainer;

class ResourceCache
{
    public:
        // TODO: Create copy/move functions and operators
        ResourceCache(const uint32_t sizeInMb, IResourceContainer* resourceContainer);
        ~ResourceCache();

        bool Initialize();
        void ClearCache();

        template <typename LoaderClass, typename = std::enable_if_t<std::is_base_of_v<IResourceFileLoader, LoaderClass>, LoaderClass>> // TODO: Switch to using concepts here
        void RegisterResourceFileLoader();

        std::shared_ptr<ResourceHandle> GetResourceHandle(const std::string& fileName);
        int32_t PreloadResources(const std::regex& regexPattern, const std::function<void(int, bool&)> &progressCallback);
        std::vector<std::string> GetResourceNamesByRegex(const std::regex& pattern) const;

    protected:
        char* AllocateMemory(const uint32_t size);
        bool MakeRoomInMemory(const uint32_t size);

        void FreeResourceHandle(const std::shared_ptr<ResourceHandle>& handle);
        std::shared_ptr<ResourceHandle> LoadResourceHandle(const std::string& fileName);
        std::shared_ptr<ResourceHandle> FindResourceHandle(const std::string& fileName);
        void UpdateResourceHandle(const std::shared_ptr<ResourceHandle>& handle);

        void FreeOneResource();
        void MemoryHasBeenFreed(uint32_t size);
        
    private:
        // TODO: Not sure we need the overhead of std::shared_ptr here. Can't we just use std::unique_ptr and just return raw none owning pointers since the cache should be solely in charge of managing lifetime of resources.
        std::list<std::shared_ptr<ResourceHandle>> mostRecentlyUsedList;        // Least used resources are at the back.
        std::map<std::string, std::shared_ptr<ResourceHandle>> resourceHandles;
        std::list<std::shared_ptr<IResourceFileLoader>> resourceFileLoaders;    // Most specific are first and most common are last.

        IResourceContainer* resourceContainer;

        uint32_t maxMemorySize;
        uint32_t allocatedMemory;

        friend class ResourceHandle;
};

template <typename LoaderClass, typename>
void ResourceCache::RegisterResourceFileLoader()
{
    resourceFileLoaders.push_front(std::make_shared<LoaderClass>());
}
