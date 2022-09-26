#include "ResourceContainers.hpp"

#include <fstream>
#include <ZipFile.h>




ZipResourceContainer::ZipResourceContainer(const std::string& path)
    : IResourceContainer(path)
    , zipContainer{nullptr}
{
    
}

bool ZipResourceContainer::Open()
{
    if (zipContainer == nullptr)
    {
        // Naked pointer with new, this is fine since the archive will
        // take ownership over it when we pass it into the ZipArchive::Create function later.
        const auto zipArchive = new std::ifstream();
        zipArchive->open(containerFilePath, std::ios::binary);

        if (zipArchive->is_open())
        {
            zipContainer = ZipArchive::Create(zipArchive, true);

            return true;
        }

        // TODO: Log and throw that we have failed to open the zip file which will more than likely be unrecoverable.
    }

    // TODO: Log that we already have a zip file open and that we are attempting to open another using the same object which isn't supported.
    return false;
}

size_t ZipResourceContainer::GetResourceSize(const std::string& fileName)
{
    if (const ZipArchiveEntry::Ptr rawResource = zipContainer->GetEntry(fileName))
    {
        return rawResource->GetSize();
    }
    else
    {
        // TODO: Log that we were unable to find the resource (Usually due to bad filename) and possibly throw.
        return 0;
    }
}

size_t ZipResourceContainer::GetRawResource(const std::string& fileName, char* buffer)
{
    const ZipArchiveEntry::Ptr rawResource = zipContainer->GetEntry(fileName);
    std::istream* resourceStream = rawResource->GetDecompressionStream();
    std::size_t size = 0;

    if (resourceStream)
    {
        size = GetResourceSize(fileName);
        memset(buffer, 0x00, size);
        resourceStream->read(buffer, size); // FIXME: Possible narrowing error here which could cause some nasty bugs in the future.
    }
    else
    {
        // TODO: Log that we were unable to find the resource (Usually due to bad filename), or that the file was encrypted and the password was incorrect.
    }

    return size;
}

size_t ZipResourceContainer::NumResourcesInArchive() const
{
    return zipContainer->GetEntriesCount();
}

std::string ZipResourceContainer::GetResourceName(const int number) const
{
    if (const std::shared_ptr<ZipArchiveEntry> entity = zipContainer->GetEntry(number))
    {
        return entity->GetName();
    }


    // TODO: Probably good to do some logging here on failure to get the resource
    return "";
}

