#ifndef IFILE_SEARCH_SERVICE_H
#define IFILE_SEARCH_SERVICE_H

#include <QObject>
#include <QString>
#include <unordered_map>
#include <string>

// Интерфейс для сервиса поиска файлов
class IFileSearchService : public QObject
{
    Q_OBJECT

public:
    explicit IFileSearchService(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IFileSearchService() override = default;

    // Выполнить поиск файлов по маске
    // Возвращает мапу (исходный файл -> результирующий файл)
    // TODO: Сделать абстракцию для мапы
    virtual std::unordered_map<std::string, std::string> searchFiles(
        const QString &sourceDirectory,
        const QString &resultDirectory,
        const QString &fileMask) = 0;

    // Проверить существование директории
    virtual bool directoryExists(const QString &directory) const = 0;

    // Создать директорию если её нет
    virtual bool ensureDirectoryExists(const QString &directory) = 0;
};

#endif // IFILE_SEARCH_SERVICE_H
