#ifndef IFILE_PROCESSING_SERVICE_H
#define IFILE_PROCESSING_SERVICE_H

#include <QString>
#include <vector>
#include <cstdint>

// Интерфейс для сервиса обработки файлов
class IFileProcessingService
{
public:
    virtual ~IFileProcessingService() = default;

    // Обработать файл с применением XOR маски
    virtual bool processFile(
        const QString &sourceFile,
        const std::vector<uint8_t> &xorMask,
        const QString &tempFile) = 0;

    // Обработать файл с учетом оффсета (для продолжения процесса)
    virtual bool processFileWithOffset(
        const QString &sourceFile,
        const std::vector<uint8_t> &xorMask,
        const QString &tempFile,
        qint64 offset) = 0;

    // Установить размер буфера для обработки
    virtual void setBufferSize(size_t bufferSize) = 0;

    // Получить текущий размер буфера
    virtual size_t bufferSize() const = 0;
};

#endif // IFILE_PROCESSING_SERVICE_H
