#ifndef APP_STATE_H
#define APP_STATE_H

#include <QString>
#include <vector>
#include <cstdint>
#include "structures.h"

// Синглтон для хранения состояния приложения
class AppState
{
public:
    // Получить единственный экземпляр AppState
    static AppState &instance();

    // Удаляем копирование и перемещение
    AppState(const AppState &) = delete;
    AppState &operator=(const AppState &) = delete;

    // Setters
    void setFileMask(const QString &mask);
    void setSourceDirectory(const QString &dir);
    void setResultDirectory(const QString &dir);
    void setDeleteSourceFiles(bool deleteSource);
    void setTimerArgs(const TimerArgs &args);
    void setDuplicationRule(FileDuplicationRule rule);
    void setXorMask(const std::vector<uint8_t> &mask);
    void setWorkingState(WorkingState state);
    void setFoundFilesCount(int count);
    void setCurrentFileIndex(int index);
    void setProcessedFilesCount(int count);
    void setCurrentFileOffset(int offset);

    // Getters
    QString fileMask() const;
    QString sourceDirectory() const;
    QString resultDirectory() const;
    bool isDeleteSourceFiles() const;
    TimerArgs timerArgs() const;
    FileDuplicationRule duplicationRule() const;
    std::vector<uint8_t> xorMask() const;
    WorkingState workingState() const;
    int foundFilesCount() const;
    int currentFileIndex() const;
    int processedFilesCount() const;
    int currentFileOffset() const;

    // Управление состоянием
    void reset();

private:
    AppState() = default;

    QString m_fileMask;
    QString m_sourceDirectory;
    QString m_resultDirectory;
    bool m_deleteSourceFiles = false;
    TimerArgs m_timerArgs = {false, 0};
    FileDuplicationRule m_duplicationRule = FileDuplicationRule::CreateCopy;
    std::vector<uint8_t> m_xorMask;
    WorkingState m_workingState = WorkingState::Idle;
    int m_foundFilesCount = 0;
    int m_currentFileIndex = 0;
    int m_processedFilesCount = 0;
    int m_currentFileOffset = 0;
};

#endif // APP_STATE_H
