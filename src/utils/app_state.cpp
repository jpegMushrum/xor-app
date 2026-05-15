#include "app_state.h"

AppState &AppState::instance()
{
    static AppState instance;
    return instance;
}

void AppState::setFileMask(const QString &mask)
{
    m_fileMask = mask;
}

void AppState::setSourceDirectory(const QString &dir)
{
    m_sourceDirectory = dir;
}

void AppState::setResultDirectory(const QString &dir)
{
    m_resultDirectory = dir;
}

void AppState::setDeleteSourceFiles(bool deleteSource)
{
    m_deleteSourceFiles = deleteSource;
}

void AppState::setTimerArgs(const TimerArgs &args)
{
    m_timerArgs = args;
}

void AppState::setDuplicationRule(FileDuplicationRule rule)
{
    m_duplicationRule = rule;
}

void AppState::setXorMask(const std::vector<uint8_t> &mask)
{
    m_xorMask = mask;
}

void AppState::setWorkingState(WorkingState state)
{
    m_workingState = state;
}

void AppState::setFoundFilesCount(int count)
{
    m_foundFilesCount = count;
}

void AppState::setCurrentFileIndex(int index)
{
    m_currentFileIndex = index;
}

void AppState::setProcessedFilesCount(int count)
{
    m_processedFilesCount = count;
}

void AppState::setCurrentFileOffset(int offset)
{
    m_currentFileOffset = offset;
}

QString AppState::fileMask() const
{
    return m_fileMask;
}

QString AppState::sourceDirectory() const
{
    return m_sourceDirectory;
}

QString AppState::resultDirectory() const
{
    return m_resultDirectory;
}

bool AppState::isDeleteSourceFiles() const
{
    return m_deleteSourceFiles;
}

TimerArgs AppState::timerArgs() const
{
    return m_timerArgs;
}

FileDuplicationRule AppState::duplicationRule() const
{
    return m_duplicationRule;
}

std::vector<uint8_t> AppState::xorMask() const
{
    return m_xorMask;
}

WorkingState AppState::workingState() const
{
    return m_workingState;
}

int AppState::foundFilesCount() const
{
    return m_foundFilesCount;
}

int AppState::currentFileIndex() const
{
    return m_currentFileIndex;
}

int AppState::processedFilesCount() const
{
    return m_processedFilesCount;
}

int AppState::currentFileOffset() const
{
    return m_currentFileOffset;
}

void AppState::reset()
{
    m_fileMask.clear();
    m_sourceDirectory.clear();
    m_resultDirectory.clear();
    m_deleteSourceFiles = false;
    m_timerArgs = {false, 0};
    m_duplicationRule = FileDuplicationRule::CreateCopy;
    m_xorMask.clear();
    m_workingState = WorkingState::Idle;
    m_foundFilesCount = 0;
    m_currentFileIndex = 0;
    m_processedFilesCount = 0;
    m_currentFileOffset = 0;
}
