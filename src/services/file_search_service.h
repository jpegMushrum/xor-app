#ifndef FILE_SEARCH_SERVICE_H
#define FILE_SEARCH_SERVICE_H

#include "ifile_search_service.h"
#include <QString>
#include <unordered_map>
#include <string>

// Сервис для поиска файлов - составляет маппу для обработки
class FileSearchService : public IFileSearchService
{
public:
    FileSearchService() = default;
    ~FileSearchService() override = default;

    // Выполнить поиск файлов по маске
    // Возвращает мапу (исходный файл -> результирующий файл)
    // TODO: Сделать абстракцию для мапы
    std::unordered_map<std::string, std::string> searchFiles(
        const QString &sourceDirectory,
        const QString &resultDirectory,
        const QString &fileMask) override;

    // Проверить существование директории
    bool directoryExists(const QString &directory) const override;

    // Создать директорию если её нет
    bool ensureDirectoryExists(const QString &directory) override;

private:
    // Проверить соответствует ли файл маске
    bool matchesMask(const QString &filename, const QString &mask) const;
};

#endif // FILE_SEARCH_SERVICE_H
