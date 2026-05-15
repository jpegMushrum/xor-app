#include "orchestrator.h"

Orchestrator::Orchestrator(
    FileSearchService *searchService,
    FileProcessingService *processingService,
    FileProcessor *fileProcessor)
    : m_searchService(searchService),
      m_processingService(processingService),
      m_fileProcessor(fileProcessor)
{
}

bool Orchestrator::validateParameters()
{
    return false;
}

QString Orchestrator::getValidationErrors() const
{
    return m_validationErrors;
}

bool Orchestrator::startProcessing()
{
    return false;
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
