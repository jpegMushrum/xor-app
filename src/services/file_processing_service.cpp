#include "file_processing_service.h"

FileProcessingService::FileProcessingService(QObject *parent)
    : IFileProcessingService(parent)
{
}

bool FileProcessingService::processFile(
    const QString &sourceFile,
    const std::vector<uint8_t> &xorMask,
    const QString &tempFile)
{
    return false;
}

bool FileProcessingService::processFileWithOffset(
    const QString &sourceFile,
    const std::vector<uint8_t> &xorMask,
    const QString &tempFile,
    qint64 offset)
{
    return false;
}

void FileProcessingService::setBufferSize(size_t bufferSize)
{
    m_bufferSize = bufferSize;
}

size_t FileProcessingService::bufferSize() const
{
    return m_bufferSize;
}
