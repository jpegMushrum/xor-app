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

void FileProcessor::createTemporaryFile(const QString &originalPath)
{
    QFileInfo fileInfo(originalPath);

    QString tempTemplate = QDir::tempPath() + "/" + fileInfo.completeBaseName() + "_XXXXXX.tmp";

    QTemporaryFile tempFile(tempTemplate);

    tempFile.setAutoRemove(false);

    if (!tempFile.open())
    {
        emit temporaryFileCreateFailed();
        return;
    }

    emit temporaryFileCreated(tempFile.fileName());
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

    return fileInfo.exists() && fileInfo.isFile() && fileInfo.isReadable();
}

bool FileProcessor::canWriteFile(const QString &filePath) const
{
    QFileInfo fileInfo(filePath);

    if (fileInfo.exists())
    {
        return fileInfo.isWritable();
    }

    QFileInfo dirInfo(fileInfo.absolutePath());

    return dirInfo.exists() && dirInfo.isDir() && dirInfo.isWritable();
}

void FileProcessor::commitFile(const QString &tempFilePath, const QString &resultFilePath)
{
    if (!QFileInfo::exists(tempFilePath))
    {
        emit commitFailed();
        return;
    }

    // Удаляем target если уже существует
    if (QFileInfo::exists(resultFilePath))
    {
        if (!QFile::remove(resultFilePath))
        {
            emit commitFailed();
            return;
        }
    }

    QFile tempFile(tempFilePath);

    if (tempFile.rename(resultFilePath))
    {
        emit commitFinished(resultFilePath);
    }
    else
    {
        emit commitFailed();
    }
}

void FileProcessor::rollbackFile(const QString &tempFilePath)
{
    deleteFile(tempFilePath);
    emit rollbackFinished();
}