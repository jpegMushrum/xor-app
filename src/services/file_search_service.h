#ifndef FILE_SEARCH_SERVICE_H
#define FILE_SEARCH_SERVICE_H

#include <QString>
#include <unordered_map>
#include <string>

// Сервис для поиска файлов - составляет маппу для обработки
class FileSearchService
{
public:
    FileSearchService() = default;
    ~FileSearchService() = default;

    // Выполнить поиск файлов по маске
    // Возвращает карту (исходный файл -> результирующий файл)
    std::unordered_map<std::string, std::string> searchFiles(
        const QString &sourceDirectory,
        const QString &resultDirectory,
        const QString &fileMask);

    // Проверить существование директории
    bool directoryExists(const QString &directory) const;

    // Создать директорию если её нет
    bool ensureDirectoryExists(const QString &directory);

private:
    // Проверить соответствует ли файл маске
    bool matchesMask(const QString &filename, const QString &mask) const;
};

#endif // FILE_SEARCH_SERVICE_H
