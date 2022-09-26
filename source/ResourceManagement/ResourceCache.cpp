#include "ResourceCache.hpp"

#include <cassert>
#include <spdlog/spdlog.h>

#include "ResourceContainers.hpp"
#include "FileLoaders.hpp"
#include "ResourceHandle.hpp"


ResourceCache::ResourceCache(const uint32_t sizeInMb, IResourceContainer* resourceContainer)
    : resourceContainer{resourceContainer}
      , maxMemorySize{sizeInMb * 1024 * 1024}
      , allocatedMemory{0}
{
}

ResourceCache::~ResourceCache()
{
    while (!mostRecentlyUsedList.empty())
    {
        FreeOneResource();
    }

    delete resourceContainer;
    resourceContainer = nullptr;
}

bool ResourceCache::Initialize()
{
    if (resourceContainer->Open())
    {
        RegisterResourceFileLoader<DevelopmentFileLoader>();
        return true;
    }

    return false;
}

void ResourceCache::ClearCache()
{
    while (!mostRecentlyUsedList.empty())
    {
        FreeResourceHandle(*mostRecentlyUsedList.begin());
        mostRecentlyUsedList.pop_front();
    }
}

std::shared_ptr<ResourceHandle> ResourceCache::GetResourceHandle(const std::string& fileName)
{
    std::shared_ptr<ResourceHandle> resHandle = FindResourceHandle(fileName);
    if (resHandle == nullptr)
    {
        // TODO: Add in some logging to track how many cache misses we are getting and on what resources. This is nothing game breaking, just helpful for performance.
        resHandle = LoadResourceHandle(fileName);
    }
    else
    {
        UpdateResourceHandle(resHandle);
    }

    return resHandle;
}

int32_t ResourceCache::PreloadResources(const std::regex& regexPattern,
                                        const std::function<void(int, bool&)>& progressCallback)
{
    if (!resourceContainer)
    {
        return 0;
    }

    int32_t loadedFiles = 0;
    bool cancelLoad = false;
    for (int i = 0; i != resourceContainer->NumResourcesInArchive(); ++i)
    {
        if (std::string fileName(resourceContainer->GetResourceName(i)); std::regex_match(fileName, regexPattern))
        {
            std::shared_ptr<ResourceHandle> handle = GetResourceHandle(fileName);
            ++loadedFiles;
        }

        if (progressCallback)
        {
            progressCallback(i * 100 / resourceContainer->NumResourcesInArchive(), cancelLoad);
        }
    }

    return loadedFiles;
}

std::vector<std::string> ResourceCache::GetResourceNamesByRegex(const std::regex& pattern) const
{
    std::vector<std::string> matchingNames;

    for (int i = 0; i != resourceContainer->NumResourcesInArchive(); ++i)
    {
        if (std::string name = resourceContainer->GetResourceName(i); std::regex_match(name, pattern))
        {
            matchingNames.push_back(name);
        }
    }

    return matchingNames;
}

char* ResourceCache::AllocateMemory(const uint32_t size)
{
    if (!MakeRoomInMemory(size))
    {
        return nullptr;
    }

    const auto memoryBuffer = new char[size];
    allocatedMemory += size;

    return memoryBuffer;
}

bool ResourceCache::MakeRoomInMemory(const uint32_t size)
{
    if (size > maxMemorySize)
    {
        return false;
    }

    while (size > maxMemorySize - allocatedMemory)
    {
        if (mostRecentlyUsedList.empty())
        {
            return false;
        }

        FreeOneResource();
    }

    return true;
}

void ResourceCache::FreeResourceHandle(const std::shared_ptr<ResourceHandle>& handle)
{
    mostRecentlyUsedList.remove(handle);
    resourceHandles.erase(handle->resourceFileName);
}

std::shared_ptr<ResourceHandle> ResourceCache::LoadResourceHandle(const std::string& fileName)
{
    // Grabs the correct file loader to load the file
    std::shared_ptr<IResourceFileLoader> fileLoader = nullptr;
    for (const auto& resourceFileLoader : resourceFileLoaders)
    {
        // TODO: Might want to get away from regex here for the pattern matching since it could cause unneeded performance issues. Profile this to see if necessary.
        if (std::regex_match(fileName, resourceFileLoader->GetFilePattern()))
        {
            fileLoader = resourceFileLoader;
            break;
        }
    }

    if (fileLoader == nullptr)
    {
        // TODO: Log that we are unable to find the a correct fileloader to handle this type.

        return {nullptr};
    }

    // Early out for unable to find the resource associated with the filename
    std::size_t rawSize = resourceContainer->GetResourceSize(fileName);
    if (resourceContainer->GetResourceSize(fileName) < 0)
    {
        // TODO: Log that we are unable to find the the resource. Don't need to throw since this can possibly be recovered from and han

        return {nullptr};
    }

    // Determine if we need to add a null zero to the file and if we can use the raw file or not.
    const int32_t allocSize = rawSize + (fileLoader->AddNullZero() ? 1 : 0);
    char* rawBuffer = fileLoader->UseRawFile() ? AllocateMemory(allocSize) : new char[allocSize];
    memset(rawBuffer, 0, allocSize);

    // Resource Cache out of Memory or something went wrong with grabbing the resource (Was encrypted and no password was provided for example)
    if (rawBuffer == nullptr || resourceContainer->GetRawResource(fileName, rawBuffer) == 0)
    {
        // TODO: Log problems
        return {nullptr};
    }

    char* buffer;
    std::shared_ptr<ResourceHandle> resHandle;

    // Raw file usage
    if (fileLoader->UseRawFile())
    {
        buffer = rawBuffer;
        resHandle = std::make_shared<ResourceHandle>(fileName, buffer, rawSize, this);
    }

    // Unable to use raw file, so need to process the file first.
    else
    {
        uint32_t size = fileLoader->GetLoadedResourceSize(rawBuffer, rawSize);
        buffer = AllocateMemory(size);

        if (buffer == nullptr)
        {
            // TODO: Need to log that we ran out of memory for the cache (Doesn't support automatic unloading at the moment)
            return {nullptr};
        }

        resHandle = std::make_shared<ResourceHandle>(fileName, buffer, size, this);
        if (fileLoader->LoadResource(rawBuffer, rawSize, resHandle))
        {
            if (fileLoader->DiscardRawBufferAfterLoad())
            {
                if (rawBuffer)
                {
                    delete[] rawBuffer;
                    rawBuffer = nullptr;
                }
            }
        }
        else
        {
            return {nullptr};
        }
    }

    if (resHandle)
    {
        mostRecentlyUsedList.push_front(resHandle);
        resourceHandles[fileName] = resHandle;
    }

    return resHandle;
}

std::shared_ptr<ResourceHandle> ResourceCache::FindResourceHandle(const std::string& fileName)
{
    const auto iter = resourceHandles.find(fileName);
    if (iter == resourceHandles.end())
    {
        return {nullptr};
    }

    return iter->second;
}

void ResourceCache::UpdateResourceHandle(const std::shared_ptr<ResourceHandle>& handle)
{
    mostRecentlyUsedList.remove(handle);
    mostRecentlyUsedList.push_front(handle);
}

void ResourceCache::FreeOneResource()
{
    auto iterToFree = mostRecentlyUsedList.end();
    --iterToFree;

    const std::shared_ptr<ResourceHandle> handle = *iterToFree;
    mostRecentlyUsedList.pop_back();
    resourceHandles.erase(handle->resourceFileName);
}

void ResourceCache::MemoryHasBeenFreed(const uint32_t size)
{
    allocatedMemory -= size;
}
