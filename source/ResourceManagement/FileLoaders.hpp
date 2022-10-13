#pragma once

#include <regex>
#include <memory>

#include "ResourceHandle.hpp"

/// @brief Interface for specific resource types. Override this class to define how different resource file types
/// are processed by the resource cache. 
class IResourceFileLoader
{
    public:
        virtual ~IResourceFileLoader() = default;

        virtual bool UseRawFile() = 0;
        virtual bool LoadResource(char* rawBuffer, uint32_t rawSize, std::shared_ptr<ResourceHandle> handle) = 0;
        virtual bool DiscardRawBufferAfterLoad() = 0;
        virtual bool AddNullZero() = 0;
        virtual std::regex GetFilePattern() = 0;
        virtual uint32_t GetLoadedResourceSize(char* rawBuffer, uint32_t rawSize) = 0;
};

/// @brief A development file loader to use as a coverall for all files that don't need any specialized processing. Ideally you should never use this outside development.
class DevelopmentFileLoader : public IResourceFileLoader
{
    public:
        std::regex GetFilePattern() override { return std::regex("^[A-z-/\\]+.[A-z]+$"); }
        bool UseRawFile() override { return true; }
        bool DiscardRawBufferAfterLoad() override { return true; }
        bool AddNullZero() override { return false; }
        uint32_t GetLoadedResourceSize(char* rawBuffer, const uint32_t rawSize) override { return rawSize; }
        bool LoadResource(char* rawBuffer, uint32_t rawSize, std::shared_ptr<ResourceHandle> handle) override { return true; }
};