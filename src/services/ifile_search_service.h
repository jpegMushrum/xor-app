#ifndef IFILE_SEARCH_SERVICE_H
#define IFILE_SEARCH_SERVICE_H

#include <QObject>
#include <QString>
#include "../utils/structures.h"

// Интерфейс для сервиса поиска файлов
class IFileSearchService : public QObject
{
    Q_OBJECT

public:
    explicit IFileSearchService(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IFileSearchService() override = default;

    // Проверить существование директории
    virtual bool directoryExists(const QString &directory) const = 0;

    // Создать директорию если её нет
    virtual bool ensureDirectoryExists(const QString &directory) = 0;

public slots:
    // Выполнить поиск файлов по маске асинхронно
    virtual void searchFiles(
        const QString &sourceDirectory,
        const QString &resultDirectory,
        const QString &fileMask,
        FileDuplicationRule duplicationRule = FileDuplicationRule::Overwrite) = 0;

signals:
    // Сигнал при успешном поиске
    void filesFound(const QVector<FileTask> &fileMapping);

    // Сигнал при ошибке
    void searchError(const QString &message);
};

#endif // IFILE_SEARCH_SERVICE_H
