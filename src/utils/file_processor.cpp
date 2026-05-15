#include "file_processor.h"

bool FileProcessor::fileExists(const QString &filePath) const
{
    return false;
}

QString FileProcessor::createTemporaryFile(const QString &originalPath) const
{
    return QString();
}

bool FileProcessor::deleteFile(const QString &filePath)
{
    return false;
}

bool FileProcessor::moveFile(const QString &sourcePath, const QString &destinationPath)
{
    return false;
}

qint64 FileProcessor::getFileSize(const QString &filePath) const
{
    return -1;
}

bool FileProcessor::canReadFile(const QString &filePath) const
{
    return false;
}

bool FileProcessor::canWriteFile(const QString &filePath) const
{
    return false;
}

bool FileProcessor::commitFile(const QString &tempFilePath, const QString &resultFilePath)
{
    return false;
}

bool FileProcessor::rollbackFile(const QString &tempFilePath)
{
    return false;
}
