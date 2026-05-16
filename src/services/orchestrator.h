#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include <QString>
#include <unordered_map>
#include <string>
#include <QObject>

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
        const QString &fileMask);

    // Проверить корректность параметров
    bool validateParameters();

    // Получить список ошибок валидации
    QString getValidationErrors() const;

    // Запустить процесс обработки файлов
    bool startProcessing();

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

private:
    // Обработать файлы последовательно
    void processFilesSequentially(
        const std::unordered_map<std::string, std::string> &fileMapping);

    // Параметры поиска
    QString m_sourceDirectory;
    QString m_resultDirectory;
    QString m_fileMask;

    IFileSearchService *m_searchService;
    IFileProcessingService *m_processingService;
    FileProcessor *m_fileProcessor;
    QString m_validationErrors;
    bool m_isProcessing = false;
};

#endif // ORCHESTRATOR_H
