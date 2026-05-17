#include "orchestrator.h"
#include "ifile_search_service.h"
#include "ifile_processing_service.h"
#include "../utils/file_processor.h"
#include <QDebug>
#include <QTimer>
#include <QFileInfo>

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

    // Сигналы FileProcessor к слотам оркестратора
    connect(m_fileProcessor, &FileProcessor::temporaryFileCreated, this, &Orchestrator::onTemporaryFileCreated);
    connect(m_fileProcessor, &FileProcessor::temporaryFileCreateFailed, this, &Orchestrator::onTemporaryFileCreateFailed);
    connect(m_fileProcessor, &FileProcessor::commitFinished, this, &Orchestrator::onCommitFinished);
    connect(m_fileProcessor, &FileProcessor::commitFailed, this, &Orchestrator::onCommitFailed);
    connect(m_fileProcessor, &FileProcessor::rollbackFinished, this, &Orchestrator::onRollbackFinished);

    // Сигналы оркестратора к слотам сервисов
    connect(this, &Orchestrator::startSearchFiles, m_searchService, &IFileSearchService::searchFiles);
    connect(this, &Orchestrator::startProcessingFile, m_processingService, &IFileProcessingService::processFile);

    // Сигналы оркестратора к слотам FileProcessor
    connect(this, &Orchestrator::createTemporaryFileRequested, m_fileProcessor, &FileProcessor::createTemporaryFile);
    connect(this, &Orchestrator::commitFileRequested, m_fileProcessor, &FileProcessor::commitFile);
    connect(this, &Orchestrator::rollbackFileRequested, m_fileProcessor, &FileProcessor::rollbackFile);

    // Инициализировать таймер перезапуска
    m_restartTimer = new QTimer(this);
    connect(m_restartTimer, &QTimer::timeout, this, &Orchestrator::onTimerTimeout);
}

void Orchestrator::setSearchParameters(
    const QString &sourceDirectory,
    const QString &resultDirectory,
    const QString &fileMask,
    FileDuplicationRule duplicationRule,
    const QVector<quint8> &xorMask,
    bool deleteSourceFiles)
{
    m_sourceDirectory = sourceDirectory;
    m_resultDirectory = resultDirectory;
    m_fileMask = fileMask;
    m_duplicationRule = duplicationRule;
    m_xorMask = xorMask;
    m_deleteSourceFiles = deleteSourceFiles;
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
    if (m_workingState == WorkingState::OnTimer)
    {
        m_restartTimer->stop();

        m_workingState = WorkingState::Idle;

        emit workingStateChanged(m_workingState);
    }

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
    emit workingStateChanged(m_workingState);

    m_currentTaskIndex = 0;
    m_tasks.clear();

    emit processingStarted();

    qDebug() << "Orchestrator starting search" << m_sourceDirectory << m_resultDirectory << m_fileMask;
    emit startSearchFiles(m_sourceDirectory, m_resultDirectory, m_fileMask, m_duplicationRule);
}

void Orchestrator::onSearchError(const QString &error)
{
    m_workingState = WorkingState::Idle;
    emit workingStateChanged(m_workingState);

    emit processingError(error);
}

void Orchestrator::onFilesFound(const QVector<FileTask> &tasks)
{
    if (tasks.isEmpty())
    {
        m_workingState = WorkingState::Idle;
        emit workingStateChanged(m_workingState);

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
        m_workingState = WorkingState::Idle;
        emit workingStateChanged(m_workingState);
        emit processingFinished();
        return;
    }

    if (m_workingState == WorkingState::Paused)
    {
        return;
    }

    if (m_currentTaskIndex >= m_tasks.size())
    {
        qDebug() << "Processing completed";

        emit processingFinished();

        if (m_restartByTimer && m_timerIntervalMs > 0)
        {
            m_workingState = WorkingState::OnTimer;

            emit workingStateChanged(m_workingState);

            qDebug() << "Next restart scheduled in" << (m_timerIntervalMs / 1000) << "seconds";

            m_restartTimer->start(m_timerIntervalMs);
        }
        else
        {
            m_workingState = WorkingState::Idle;

            emit workingStateChanged(m_workingState);
        }

        return;
    }

    const FileTask &task = m_tasks[m_currentTaskIndex];
    m_currentTask = task;

    emit processingProgress(m_currentTaskIndex, m_tasks.size(), QFileInfo(task.source).fileName());
    emit createTemporaryFileRequested(task.source);
}

void Orchestrator::onProcessingError(const QString &error)
{
    if (!m_currentTempFile.isEmpty())
    {
        emit rollbackFileRequested(m_currentTempFile);
        m_currentTempFile.clear();
    }

    m_workingState = WorkingState::Idle;
    emit workingStateChanged(m_workingState);
    emit processingError(error);
}

void Orchestrator::onFileProcessed(const QString &tempFile)
{
    emit commitFileRequested(tempFile, m_currentTask.target);
}

void Orchestrator::pauseProcessing()
{
    if (m_workingState != WorkingState::Running)
    {
        return;
    }

    m_workingState = WorkingState::Paused;

    emit workingStateChanged(m_workingState);

    if (m_searchService)
    {
        m_searchService->pause();
    }

    if (m_processingService)
    {
        m_processingService->pause();
    }
}

void Orchestrator::cancelProcessing()
{
    if (m_workingState == WorkingState::Idle)
    {
        return;
    }

    if (m_restartTimer->isActive())
    {
        m_restartTimer->stop();
    }

    if (m_searchService)
    {
        m_searchService->stop();
    }

    if (m_processingService)
    {
        m_processingService->stop();
    }

    if (!m_currentTempFile.isEmpty())
    {
        m_workingState = WorkingState::Cancelled;

        emit workingStateChanged(m_workingState);

        emit rollbackFileRequested(m_currentTempFile);

        return;
    }

    m_workingState = WorkingState::Idle;

    emit workingStateChanged(m_workingState);

    emit processingFinished();
}

void Orchestrator::resumeProcessing()
{
    if (m_workingState != WorkingState::Paused)
    {
        return;
    }

    m_workingState = WorkingState::Running;

    emit workingStateChanged(m_workingState);

    if (m_searchService)
    {
        m_searchService->resume();
    }

    if (m_processingService)
    {
        m_processingService->resume();
    }
}

int Orchestrator::getProgress() const
{
    // TODO
    return 0;
}

void Orchestrator::onTemporaryFileCreated(const QString &tempFilePath)
{
    m_currentTempFile = tempFilePath;
    emit startProcessingFile(m_currentTask.source, m_xorMask, tempFilePath);
}

void Orchestrator::onTemporaryFileCreateFailed()
{
    m_workingState = WorkingState::Idle;
    emit workingStateChanged(m_workingState);
    emit processingError("Не удалось создать временный файл");
}

void Orchestrator::onCommitFinished(const QString &resultFilePath)
{
    if (m_deleteSourceFiles && !m_currentTask.source.isEmpty() &&
        !(m_duplicationRule == FileDuplicationRule::Overwrite && m_currentTask.source == m_currentTask.target)) // Чтобы не удалить результат
    {
        m_fileProcessor->deleteFile(m_currentTask.source);
    }

    m_currentTaskIndex++;

    emit processingProgress(m_currentTaskIndex, m_tasks.size(), QFileInfo(resultFilePath).fileName());

    m_currentTempFile.clear();
    processNextFile();
}

void Orchestrator::onCommitFailed()
{
    if (!m_currentTempFile.isEmpty())
    {
        emit rollbackFileRequested(m_currentTempFile);
    }

    m_workingState = WorkingState::Idle;
    emit workingStateChanged(m_workingState);
    emit processingError("Не удалось сохранить результирующий файл");
}

void Orchestrator::onRollbackFinished()
{
    m_workingState = WorkingState::Idle;
    emit workingStateChanged(m_workingState);
}

void Orchestrator::setRestartTimer(bool enabled, int seconds)
{
    m_restartByTimer = enabled;
    m_timerIntervalMs = seconds * 1000;
}

void Orchestrator::onTimerTimeout()
{
    if (m_workingState != WorkingState::OnTimer)
    {
        return;
    }

    qDebug() << "Restart timer triggered";

    m_workingState = WorkingState::Idle;

    emit workingStateChanged(m_workingState);

    startProcessing();
}

WorkingState Orchestrator::getWorkingState() const {
    return m_workingState;
}
