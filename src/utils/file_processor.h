#ifndef FILE_PROCESSOR_H
#define FILE_PROCESSOR_H

#include <QString>

// Обработка файлов - изменение/удаление исходного и запись в результирующий
class FileProcessor
{
public:
    FileProcessor() = default;
    ~FileProcessor() = default;

    // Проверить существование файла
    bool fileExists(const QString &filePath) const;

    // Создать временный файл
    QString createTemporaryFile(const QString &originalPath) const;

    // Удалить файл
    bool deleteFile(const QString &filePath);

    // Переместить/переименовать файл
    bool moveFile(const QString &sourcePath, const QString &destinationPath);

    // Получить размер файла
    qint64 getFileSize(const QString &filePath) const;

    // Проверить доступ для чтения
    bool canReadFile(const QString &filePath) const;

    // Проверить доступ для записи
    bool canWriteFile(const QString &filePath) const;

    // Закоммитить результат (переместить временный файл на место результирующего)
    bool commitFile(const QString &tempFilePath, const QString &resultFilePath);

    // Откатить временный файл (удалить его)
    bool rollbackFile(const QString &tempFilePath);
};

#endif // FILE_PROCESSOR_H
