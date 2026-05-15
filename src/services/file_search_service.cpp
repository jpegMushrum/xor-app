#include "file_search_service.h"

std::unordered_map<std::string, std::string> FileSearchService::searchFiles(
    const QString &sourceDirectory,
    const QString &resultDirectory,
    const QString &fileMask)
{
    std::unordered_map<std::string, std::string> result;
    return result;
}

bool FileSearchService::directoryExists(const QString &directory) const
{
    return false;
}

bool FileSearchService::ensureDirectoryExists(const QString &directory)
{
    return false;
}

bool FileSearchService::matchesMask(const QString &filename, const QString &mask) const
{
    return false;
}
