#ifndef FILE_PROCESSOR_H
#define FILE_PROCESSOR_H

#include <QString>
#include <QObject>

// Обработка файлов - изменение/удаление исходного и запись в результирующий
class FileProcessor : public QObject
{
    Q_OBJECT

public:
    explicit FileProcessor(QObject *parent = nullptr);
    ~FileProcessor() = default;

    // Проверить существование файла
    bool fileExists(const QString &filePath) const;

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

public slots:
    // Асинхронное создание временного файла
    void createTemporaryFile(const QString &originalPath);

    // Асинхронная фиксация результата (переместить временный файл на место результирующего)
    void commitFile(const QString &tempFilePath, const QString &resultFilePath);

    // Асинхронный откат временного файла (удалить его)
    void rollbackFile(const QString &tempFilePath);

signals:
    // Результаты асинхронных операций
    void temporaryFileCreated(const QString &tempFilePath);
    void temporaryFileCreateFailed();

    void commitFinished(const QString &resultFilePath);
    void commitFailed();

    void rollbackFinished();
};

#endif // FILE_PROCESSOR_H
