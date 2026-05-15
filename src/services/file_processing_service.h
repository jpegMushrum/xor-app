#ifndef FILE_PROCESSING_SERVICE_H
#define FILE_PROCESSING_SERVICE_H

#include <QString>
#include <vector>
#include <cstdint>

// Сервис для обработки файлов с XOR маской
class FileProcessingService
{
public:
    FileProcessingService() = default;
    ~FileProcessingService() = default;

    // Обработать файл с применением XOR маски
    bool processFile(
        const QString &sourceFile,
        const std::vector<uint8_t> &xorMask,
        const QString &tempFile);

    // Обработать файл с учетом оффсета (для продолжения процесса)
    bool processFileWithOffset(
        const QString &sourceFile,
        const std::vector<uint8_t> &xorMask,
        const QString &tempFile,
        qint64 offset);

    // Установить размер буфера для обработки
    void setBufferSize(size_t bufferSize);

    // Получить текущий размер буфера
    size_t bufferSize() const;

private:
    size_t m_bufferSize = 1024 * 1024; // 1 MB по умолчанию
};

#endif // FILE_PROCESSING_SERVICE_H
