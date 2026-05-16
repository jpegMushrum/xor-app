#include "file_processor.h"

#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QDir>

FileProcessor::FileProcessor(QObject *parent)
    : QObject(parent)
{
}

bool FileProcessor::fileExists(const QString &filePath) const
{
    return QFileInfo::exists(filePath);
}

QString FileProcessor::createTemporaryFile(const QString &originalPath) const
{
    QFileInfo fileInfo(originalPath);

    QString tempTemplate =
        QDir::tempPath() +
        "/" +
        fileInfo.completeBaseName() +
        "_XXXXXX.tmp";

    QTemporaryFile tempFile(tempTemplate);

    tempFile.setAutoRemove(false);

    if (!tempFile.open())
    {
        return QString();
    }

    return tempFile.fileName();
}

bool FileProcessor::deleteFile(const QString &filePath)
{
    if (!QFileInfo::exists(filePath))
    {
        return true;
    }

    QFile file(filePath);

    return file.remove();
}

bool FileProcessor::moveFile(const QString &sourcePath, const QString &destinationPath)
{
    if (!QFileInfo::exists(sourcePath))
    {
        return false;
    }

    // Удаляем target если существует
    if (QFileInfo::exists(destinationPath))
    {
        if (!QFile::remove(destinationPath))
        {
            return false;
        }
    }

    QFile file(sourcePath);

    return file.rename(destinationPath);
}

qint64 FileProcessor::getFileSize(const QString &filePath) const
{
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists())
    {
        return -1;
    }

    return fileInfo.size();
}

bool FileProcessor::canReadFile(const QString &filePath) const
{
    QFileInfo fileInfo(filePath);

    return fileInfo.exists() &&
           fileInfo.isFile() &&
           fileInfo.isReadable();
}

bool FileProcessor::canWriteFile(const QString &filePath) const
{
    QFileInfo fileInfo(filePath);

    // Если файл существует
    if (fileInfo.exists())
    {
        return fileInfo.isWritable();
    }

    // Если файла нет — проверяем директорию
    QFileInfo dirInfo(fileInfo.absolutePath());

    return dirInfo.exists() &&
           dirInfo.isDir() &&
           dirInfo.isWritable();
}

bool FileProcessor::commitFile(const QString &tempFilePath, const QString &resultFilePath)
{
    if (!QFileInfo::exists(tempFilePath))
    {
        return false;
    }

    // Удаляем target если уже существует
    if (QFileInfo::exists(resultFilePath))
    {
        if (!QFile::remove(resultFilePath))
        {
            return false;
        }
    }

    QFile tempFile(tempFilePath);

    return tempFile.rename(resultFilePath);
}

bool FileProcessor::rollbackFile(const QString &tempFilePath)
{
    return deleteFile(tempFilePath);
}