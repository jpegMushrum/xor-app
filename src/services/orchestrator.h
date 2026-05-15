#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include <QString>
#include <unordered_map>
#include <string>

class FileSearchService;
class FileProcessingService;
class FileProcessor;

// Оркестратор - управляет процессом обработки файлов
class Orchestrator
{
public:
    explicit Orchestrator(
        FileSearchService *searchService,
        FileProcessingService *processingService,
        FileProcessor *fileProcessor);
    ~Orchestrator() = default;

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

private:
    // Обработать файлы последовательно
    void processFilesSequentially(
        const std::unordered_map<std::string, std::string> &fileMapping);

    FileSearchService *m_searchService;
    FileProcessingService *m_processingService;
    FileProcessor *m_fileProcessor;
    QString m_validationErrors;
    bool m_isProcessing = false;
};

#endif // ORCHESTRATOR_H
