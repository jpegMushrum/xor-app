#ifndef FILE_PROCESSING_SERVICE_H
#define FILE_PROCESSING_SERVICE_H

#include "ifile_processing_service.h"
#include "../utils/structures.h"

#include <QString>
#include <QMutex>
#include <QWaitCondition>

// Сервис для обработки файлов с XOR маской
class FileProcessingService : public IFileProcessingService
{
    Q_OBJECT

public:
    explicit FileProcessingService(QObject *parent = nullptr);
    ~FileProcessingService() override = default;

    // Установить размер буфера для обработки
    void setBufferSize(size_t bufferSize) override;

    // Получить текущий размер буфера
    size_t bufferSize() const override;

    // Пауза и остановка
    void pause() override;
    void resume() override;
    void stop() override;

public slots:
    // Обработать файл с применением XOR маски асинхронно
    void processFile(
        const QString &sourceFile,
        const QVector<quint8> &xorMask,
        const QString &tempFile) override;

    void processFileWithOffset(
        const QString &sourceFile,
        const QVector<quint8> &xorMask,
        const QString &tempFile,
        qint64 offset) override;

private:
    // Состояние меняем синхронно, из разных потоков
    QMutex m_mutex;
    QWaitCondition m_pauseCondition;
    ServiceState m_state = ServiceState::Stopped;

    size_t m_bufferSize = 1024 * 1024; // 1 MB по умолчанию
};

#endif // FILE_PROCESSING_SERVICE_H
