#pragma once

#include <string>
#include <ZipArchive.h>


/// @brief An interface to represent the container (Zip file for example) where our resources are. We would override
/// this to create functionality for opening different types of containers and requesting certain resources from it.
/// The end user should not be initializing any of these classes on their own, instead 
class IResourceContainer
{
    public:
        explicit IResourceContainer(std::string path) : containerFilePath(std::move(path)) {}
        virtual ~IResourceContainer() = default;

        virtual bool Open() = 0;
        virtual size_t GetResourceSize(const std::string& fileName) = 0;
        virtual size_t GetRawResource(const std::string& fileName, char* buffer) = 0;
        virtual size_t NumResourcesInArchive() const = 0;
        virtual std::string GetResourceName(const int number) const = 0;

    protected:
        std::string containerFilePath;
};

/// @brief Represents a zip archive where resources live. Able to open the archive, get resources from it,
/// the number of resources, sizes/names of resources, etc.
class ZipResourceContainer : public IResourceContainer
{
    public:
        explicit ZipResourceContainer(const std::string& path);
        ~ZipResourceContainer() override = default;

        bool Open() override;
        size_t GetResourceSize(const std::string& fileName) override;
        size_t GetRawResource(const std::string& fileName, char* buffer) override;
        size_t NumResourcesInArchive() const override;
        std::string GetResourceName(const int number) const override;

    private:
        ZipArchive::Ptr zipContainer;
};
