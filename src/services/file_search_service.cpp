#include "file_search_service.h"
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>

FileSearchService::FileSearchService(QObject *parent)
    : IFileSearchService(parent)
{
}

std::unordered_map<std::string, std::string> FileSearchService::searchFiles(
    const QString &sourceDirectory,
    const QString &resultDirectory,
    const QString &fileMask)
{
    std::unordered_map<std::string, std::string> result;

    if (!directoryExists(sourceDirectory))
    {
        return result;
    }

    if (!ensureDirectoryExists(resultDirectory))
    {
        return result;
    }

    QDirIterator it(sourceDirectory, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        it.next();

        if (it.fileInfo().isDir())
        {
            continue;
        }

        QString filename = it.fileInfo().fileName();

        if (matchesMask(filename, fileMask))
        {
            QString sourceFile = it.filePath();
            QString resultFile = resultDirectory + "/" + filename;

            result[sourceFile.toStdString()] = resultFile.toStdString();
        }
    }

    return result;
}

bool FileSearchService::directoryExists(const QString &directory) const
{
    QDir dir(directory);
    return dir.exists();
}

bool FileSearchService::ensureDirectoryExists(const QString &directory)
{
    QDir dir(directory);
    if (dir.exists())
    {
        return true;
    }
    return dir.mkpath(".");
}

bool FileSearchService::matchesMask(const QString &filename, const QString &mask) const
{
    if (!mask.contains('*'))
    {
        return filename == mask;
    }

    QStringList parts = mask.split('*');

    // Проверяем, что имя файла начинается с первой части (если она не пуста)
    int pos = 0;
    if (!parts[0].isEmpty())
    {
        if (!filename.startsWith(parts[0]))
        {
            return false;
        }
        pos = parts[0].length();
    }

    // Проверяем все промежуточные части
    for (int i = 1; i < parts.length() - 1; ++i)
    {
        if (parts[i].isEmpty())
        {
            continue;
        }

        int index = filename.indexOf(parts[i], pos);
        if (index == -1)
        {
            return false;
        }
        pos = index + parts[i].length();
    }

    // Проверяем, что имя файла заканчивается с последней части (если она не пуста)
    if (!parts[parts.length() - 1].isEmpty())
    {
        if (!filename.endsWith(parts[parts.length() - 1]))
        {
            return false;
        }

        int lastIndex = filename.lastIndexOf(parts[parts.length() - 1]);
        if (lastIndex < pos)
        {
            return false;
        }
    }

    return true;
}
