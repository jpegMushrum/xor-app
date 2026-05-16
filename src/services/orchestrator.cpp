#include "orchestrator.h"
#include "ifile_search_service.h"
#include <QDebug>

Orchestrator::Orchestrator(
    IFileSearchService *searchService,
    IFileProcessingService *processingService,
    FileProcessor *fileProcessor,
    QObject *parent)
    : QObject(parent), m_searchService(searchService),
      m_processingService(processingService),
      m_fileProcessor(fileProcessor)
{
}

void Orchestrator::setSearchParameters(
    const QString &sourceDirectory,
    const QString &resultDirectory,
    const QString &fileMask)
{
    m_sourceDirectory = sourceDirectory;
    m_resultDirectory = resultDirectory;
    m_fileMask = fileMask;
}

bool Orchestrator::validateParameters()
{
    m_validationErrors.clear();

    if (m_sourceDirectory.isEmpty())
    {
        m_validationErrors = "Исходная директория не установлена";
        return false;
    }

    if (m_resultDirectory.isEmpty())
    {
        m_validationErrors = "Результирующая директория не установлена";
        return false;
    }

    if (m_fileMask.isEmpty())
    {
        m_validationErrors = "Маска файлов не установлена";
        return false;
    }

    if (!m_searchService)
    {
        m_validationErrors = "Search Service не инициализирован";
        return false;
    }

    return true;
}

QString Orchestrator::getValidationErrors() const
{
    return m_validationErrors;
}

bool Orchestrator::startProcessing()
{
    if (!validateParameters())
    {
        emit processingError(m_validationErrors);
        return false;
    }

    m_isProcessing = true;
    emit processingStarted();

    // Поиск файлов
    std::unordered_map<std::string, std::string> fileMapping =
        m_searchService->searchFiles(m_sourceDirectory, m_resultDirectory, m_fileMask);

    qDebug() << "=== FOUND FILES ===";
    qDebug() << "Total files:" << static_cast<int>(fileMapping.size());

    for (const auto &pair : fileMapping)
    {
        qDebug() << "Source:" << QString::fromStdString(pair.first);
        qDebug() << "Target:" << QString::fromStdString(pair.second);
        qDebug() << "---";
    }

    emit filesMapped(static_cast<int>(fileMapping.size()));

    // TODO: Обработка файлов (когда будет реализован FileProcessingService и FileProcessor)
    // processFilesSequentially(fileMapping);

    m_isProcessing = false;
    emit processingFinished();

    return true;
}

void Orchestrator::pauseProcessing()
{
}

void Orchestrator::cancelProcessing()
{
}

bool Orchestrator::resumeProcessing()
{
    return false;
}

int Orchestrator::getProgress() const
{
    return 0;
}

bool Orchestrator::isProcessing() const
{
    return m_isProcessing;
}

void Orchestrator::processFilesSequentially(
    const std::unordered_map<std::string, std::string> &fileMapping)
{
}
