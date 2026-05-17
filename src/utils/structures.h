#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <QString>

// Аргументы таймера
struct TimerArgs
{
    bool enabled;
    int period;
};

// Правило для повторяющихся имен файлов
enum class FileDuplicationRule
{
    Overwrite,
    CreateCopy,
    Skip
};

// Состояние работы приложения
enum class WorkingState
{
    Idle,
    Running,
    Paused,
    Cancelled,
    OnTimer
};

// Структура для сохранения задач (вместо мапы)
struct FileTask
{
    QString source;
    QString target;
};

#endif // STRUCTURES_H
