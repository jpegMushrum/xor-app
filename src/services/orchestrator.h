#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include <QString>
#include <QObject>
#include "../utils/structures.h"

class IFileProcessingService;
class IFileSearchService;
class FileProcessor;

// Оркестратор - управляет процессом обработки файлов
class Orchestrator : public QObject
{
    Q_OBJECT

public:
    explicit Orchestrator(
        IFileSearchService *searchService,
        IFileProcessingService *processingService,
        FileProcessor *fileProcessor,
        QObject *parent = nullptr);

    ~Orchestrator() = default;

    // Установить параметры поиска
    void setSearchParameters(
        const QString &sourceDirectory,
        const QString &resultDirectory,
        const QString &fileMask,
        FileDuplicationRule duplicationRule,
        const QVector<quint8> &xorMask,
        bool deleteSourceFiles = false);

    // Проверить корректность параметров
    bool validateParameters();

    // Получить список ошибок валидации
    QString getValidationErrors() const;

    // Запустить процесс обработки файлов
    void startProcessing();

    // Остановить процесс (пауза)
    void pauseProcessing();

    // Отменить процесс
    void cancelProcessing();

    // Возобновить процесс после паузы
    bool resumeProcessing();

    // Получить текущий прогресс обработки (0-100)
    int getProgress() const;

    // Проверить, выполняется ли процесс
    bool isProcessing() const;

signals:
    // Сигналы для UI
    void processingStarted();
    void filesMapped(int fileCount);
    void processingProgress(int current, int total);
    void processingFinished();
    void processingError(const QString &error);
    void workingStateChanged(WorkingState state);

    // Сигналы для сервисов
    void startSearchFiles(const QString &sourceDirectory,
                          const QString &resultDirectory,
                          const QString &fileMask,
                          FileDuplicationRule duplicationRule);

    void startProcessingFile(
        const QString &sourceFile,
        const QVector<quint8> &xorMask,
        const QString &tempFile);

    // Сигналы для FileProcessor
    void createTemporaryFileRequested(const QString &originalPath);

    void commitFileRequested(
        const QString &tempFilePath,
        const QString &resultFilePath);

    void rollbackFileRequested(
        const QString &tempFilePath);

private slots:
    void onSearchError(const QString &error);
    void onFilesFound(const QVector<FileTask> &tasks);
    void onProcessingError(const QString &error);
    void onFileProcessed(const QString &resultFile);

    // Слоты для асинхронных операций FileProcessor
    void onTemporaryFileCreated(const QString &tempFilePath);
    void onTemporaryFileCreateFailed();
    void onCommitFinished(const QString &resultFilePath);
    void onCommitFailed();
    void onRollbackFinished();

private:
    // Обработать следующий файл
    void processNextFile();

    // Параметры запуска
    QString m_sourceDirectory;
    QString m_resultDirectory;
    QString m_fileMask;
    FileDuplicationRule m_duplicationRule = FileDuplicationRule::Overwrite;
    QVector<quint8> m_xorMask;
    bool m_deleteSourceFiles = false;

    IFileSearchService *m_searchService;
    IFileProcessingService *m_processingService;
    FileProcessor *m_fileProcessor;

    QString m_validationErrors;
    WorkingState m_workingState = WorkingState::Idle;

    QVector<FileTask> m_tasks;
    int m_currentTaskIndex = 0;
    FileTask m_currentTask;
    QString m_currentTempFile;
};

#endif // ORCHESTRATOR_H
