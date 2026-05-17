#ifndef IFILE_PROCESSING_SERVICE_H
#define IFILE_PROCESSING_SERVICE_H

#include <QObject>
#include <QString>

// Интерфейс для сервиса обработки файлов
class IFileProcessingService : public QObject
{
    Q_OBJECT

public:
    explicit IFileProcessingService(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IFileProcessingService() override = default;

    // Установить размер буфера для обработки
    virtual void setBufferSize(size_t bufferSize) = 0;

    // Получить текущий размер буфера
    virtual size_t bufferSize() const = 0;

    // Пауза и остановка
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void stop() = 0;

public slots:
    // Обработать файл с применением XOR маски асинхронно
    virtual void processFile(
        const QString &sourceFile,
        const QVector<quint8> &xorMask,
        const QString &tempFile) = 0;

    virtual void processFileWithOffset(
        const QString &sourceFile,
        const QVector<quint8> &xorMask,
        const QString &tempFile,
        qint64 offset) = 0;

signals:
    // Сигнал при успешной обработке файла
    void fileProcessed(const QString &resultFile);

    // Сигнал при ошибке обработки
    void processingError(const QString &message);
};

#endif // IFILE_PROCESSING_SERVICE_H
