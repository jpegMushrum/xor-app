#ifndef FILE_SEARCH_SERVICE_H
#define FILE_SEARCH_SERVICE_H

#include "ifile_search_service.h"

#include <QMutex>
#include <QString>
#include <QWaitCondition>

// Сервис для поиска файлов - составляет маппу для обработки
class FileSearchService : public IFileSearchService
{
    Q_OBJECT

public:
    explicit FileSearchService(QObject *parent = nullptr);
    ~FileSearchService() override = default;

    // Проверить существование директории
    bool directoryExists(const QString &directory) const override;

    // Создать директорию если её нет
    bool ensureDirectoryExists(const QString &directory) override;

    // Пауза и остановка
    void pause() override;
    void resume() override;
    void stop() override;

public slots:
    // Выполнить поиск файлов асинхронно
    void searchFiles(
        const QString &sourceDirectory,
        const QString &resultDirectory,
        const QString &fileMask,
        FileDuplicationRule duplicationRule = FileDuplicationRule::Overwrite) override;

private:
    // Проверить соответствует ли файл маске
    bool matchesMask(const QString &filename, const QString &mask) const;

    // Получить результирующий путь с учетом политики дублирования
    QString getResultFilePath(
        const QString &resultDirectory,
        const QString &filename,
        FileDuplicationRule duplicationRule) const;

    // Получить имя файла с суффиксом номера (для политики CreateCopy)
    QString getFileNameWithCounter(
        const QString &resultDirectory,
        const QString &filename) const;


    // Состояние меняем синхронно, из разных потоков
    QMutex m_mutex;
    QWaitCondition m_pauseCondition;
    ServiceState m_state = ServiceState::Stopped;
};

#endif // FILE_SEARCH_SERVICE_H
