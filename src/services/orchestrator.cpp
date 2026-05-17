#include "orchestrator.h"
#include "ifile_search_service.h"
#include "ifile_processing_service.h"
#include "../utils/file_processor.h"
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
    // Сигналы сервисов к слотам оркестратора
    connect(m_searchService, &IFileSearchService::searchError, this, &Orchestrator::onSearchError);
    connect(m_searchService, &IFileSearchService::filesFound, this, &Orchestrator::onFilesFound);
    connect(m_processingService, &IFileProcessingService::fileProcessed, this, &Orchestrator::onFileProcessed);
    connect(m_processingService, &IFileProcessingService::processingError, this, &Orchestrator::onProcessingError);

    // Сигналы оркестратора к слотам сервисов
    connect(this, &Orchestrator::startSearchFiles, m_searchService, &IFileSearchService::searchFiles);
    connect(this, &Orchestrator::startProcessingFile, m_processingService, &IFileProcessingService::processFile);
}

void Orchestrator::setSearchParameters(
    const QString &sourceDirectory,
    const QString &resultDirectory,
    const QString &fileMask,
    FileDuplicationRule duplicationRule,
    const QVector<quint8> &xorMask)
{
    m_sourceDirectory = sourceDirectory;
    m_resultDirectory = resultDirectory;
    m_fileMask = fileMask;
    m_duplicationRule = duplicationRule;
    m_xorMask = xorMask;
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

    if (m_xorMask.empty())
    {
        m_validationErrors = "XOR маска не установлена";
        return false;
    }

    if (!m_searchService)
    {
        m_validationErrors = "Search service не инициализирован";
        return false;
    }

    if (!m_processingService)
    {
        m_validationErrors = "Processing service не инициализирован";
        return false;
    }

    return true;
}

QString Orchestrator::getValidationErrors() const
{
    return m_validationErrors;
}

void Orchestrator::startProcessing()
{
    if (m_workingState != WorkingState::Idle)
    {
        return;
    }

    if (!validateParameters())
    {
        emit processingError(m_validationErrors);
        return;
    }

    m_workingState = WorkingState::Running;

    m_currentTaskIndex = 0;
    m_tasks.clear();

    emit processingStarted();

    emit startSearchFiles(m_sourceDirectory, m_resultDirectory, m_fileMask, m_duplicationRule);
}

void Orchestrator::onSearchError(const QString &error)
{
    m_workingState = WorkingState::Cancelled;

    emit processingError(error);
}

void Orchestrator::onFilesFound(const QVector<FileTask> &tasks)
{
    if (tasks.isEmpty())
    {
        m_workingState = WorkingState::Cancelled;

        emit processingError("Файлы не найдены");

        return;
    }

    m_tasks = tasks;

    emit filesMapped(m_tasks.size());

    processNextFile();
}

void Orchestrator::processNextFile()
{
    if (m_workingState == WorkingState::Cancelled)
    {
        emit processingFinished();

        return;
    }

    if (m_workingState == WorkingState::Paused)
    {
        return;
    }

    if (m_currentTaskIndex >= m_tasks.size())
    {
        m_workingState = WorkingState::Idle;

        emit processingFinished();
        return;
    }

    const FileTask &task = m_tasks[m_currentTaskIndex];
    m_currentTask = task;

    QString tempFile = m_fileProcessor->createTemporaryFile(task.source);

    if (tempFile.isEmpty())
    {
        m_workingState = WorkingState::Cancelled;

        emit processingError("Не удалось создать временный файл");
        return;
    }

    m_currentTempFile = tempFile;

    emit startProcessingFile(task.source, m_xorMask, tempFile);
}

void Orchestrator::onProcessingError(const QString &error)
{
    if (!m_currentTempFile.isEmpty())
    {
        m_fileProcessor->rollbackFile(
            m_currentTempFile);

        m_currentTempFile.clear();
    }

    m_workingState = WorkingState::Cancelled;
    emit processingError(error);
}

void Orchestrator::onFileProcessed(const QString &tempFile)
{
    bool committed = m_fileProcessor->commitFile(tempFile, m_currentTask.target);

    if (!committed)
    {
        m_fileProcessor->rollbackFile(tempFile);

        m_workingState = WorkingState::Cancelled;

        emit processingError("Не удалось сохранить результирующий файл");

        return;
    }

    m_currentTaskIndex++;

    emit processingProgress(m_currentTaskIndex, m_tasks.size());

    m_currentTempFile.clear();
    processNextFile();
}

void Orchestrator::pauseProcessing()
{
    // TODO
}

void Orchestrator::cancelProcessing()
{
    // TODO
}

bool Orchestrator::resumeProcessing()
{
    // TODO
    return false;
}

int Orchestrator::getProgress() const
{
    // TODO
    return 0;
}
