#include "file_search_service.h"
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QDebug>
#include <QMutexLocker>

FileSearchService::FileSearchService(QObject *parent)
    : IFileSearchService(parent)
{
}

// TODO: перевести ошибки на русский, а то по привычке написал на англе
void FileSearchService::searchFiles(
    const QString &sourceDirectory,
    const QString &resultDirectory,
    const QString &fileMask,
    FileDuplicationRule duplicationRule)
{
    {
        QMutexLocker lock(&m_mutex);

        m_state = ServiceState::Running;
    }

    QVector<FileTask> result;

    if (!directoryExists(sourceDirectory))
    {
        emit searchError("Source directory does not exist: " + sourceDirectory);
        return;
    }

    if (!ensureDirectoryExists(resultDirectory))
    {
        emit searchError("Cannot create result directory: " + resultDirectory);
        return;
    }

    QDirIterator it(sourceDirectory, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        {
            QMutexLocker lock(&m_mutex);

            while (m_state == ServiceState::Paused)
            {
                m_pauseCondition.wait(&m_mutex);
            }

            if (m_state == ServiceState::Stopped)
            {
                return;
            }
        }

        it.next();

        if (it.fileInfo().isDir())
        {
            continue;
        }

        QString filename = it.fileInfo().fileName();

        if (matchesMask(filename, fileMask))
        {
            QString sourceFile = it.filePath();
            QString resultFile = getResultFilePath(resultDirectory, filename, duplicationRule);

            if (duplicationRule == FileDuplicationRule::CreateCopy && resultFile.isEmpty())
            {
                emit searchError("Can't create copy for file: " + sourceFile);
                return;
            }

            if (duplicationRule == FileDuplicationRule::Skip && resultFile.isEmpty()) {
                continue;
            }

            result.append(FileTask{.source =  sourceFile, .target = resultFile});
        }
    }

    qDebug() << "Search completed. Found" << static_cast<int>(result.size()) << "files";

    emit filesFound(result);
}

bool FileSearchService::directoryExists(const QString &directory) const
{
    QDir dir(directory);
    return dir.exists();
}

bool FileSearchService::ensureDirectoryExists(const QString &directory)
{
    QDir dir(directory);
    if (dir.exists())
    {
        return true;
    }
    return dir.mkpath(".");
}

bool FileSearchService::matchesMask(const QString &filename, const QString &mask) const
{
    if (!mask.contains('*'))
    {
        return filename == mask;
    }

    QStringList parts = mask.split('*');

    // Проверяем, что имя файла начинается с первой части
    int pos = 0;
    if (!parts[0].isEmpty())
    {
        if (!filename.startsWith(parts[0]))
        {
            return false;
        }
        pos = parts[0].length();
    }

    // Проверяем все промежуточные части
    for (int i = 1; i < parts.length() - 1; ++i)
    {
        if (parts[i].isEmpty())
        {
            continue;
        }

        int index = filename.indexOf(parts[i], pos);
        if (index == -1)
        {
            return false;
        }
        pos = index + parts[i].length();
    }

    // Проверяем, что имя файла заканчивается с последней части
    if (!parts[parts.length() - 1].isEmpty())
    {
        if (!filename.endsWith(parts[parts.length() - 1]))
        {
            return false;
        }

        int lastIndex = filename.lastIndexOf(parts[parts.length() - 1]);
        if (lastIndex < pos)
        {
            return false;
        }
    }

    return true;
}

QString FileSearchService::getResultFilePath(
    const QString &resultDirectory,
    const QString &filename,
    FileDuplicationRule duplicationRule) const
{
    QString resultFilePath = resultDirectory + "/" + filename;

    switch (duplicationRule)
    {
    case FileDuplicationRule::Overwrite:
        return resultFilePath;

    case FileDuplicationRule::Skip:
        if (QFileInfo::exists(resultFilePath))
        {
            qDebug() << "File already exists, skipping:" << resultFilePath;
            return QString(); // Возвращаем пустую строку для пропуска
        }
        return resultFilePath;

    case FileDuplicationRule::CreateCopy:
        if (QFileInfo::exists(resultFilePath))
        {
            qDebug() << "File already exists, creating copy with counter:" << resultFilePath;
            return getFileNameWithCounter(resultDirectory, filename); // Возвращает пустую строку, если не получается сделать копию
        }
        return resultFilePath;

    default:
        return resultFilePath;
    }
}

QString FileSearchService::getFileNameWithCounter(
    const QString &resultDirectory,
    const QString &filename) const
{
    QFileInfo fileInfo(filename);
    QString baseName = fileInfo.baseName();
    QString suffix = fileInfo.suffix();

    // Ищем первый доступный номер
    for (int i = 1; i < 10000; ++i)
    {
        QString newFilename;
        if (suffix.isEmpty())
        {
            newFilename = QString("%1_%2").arg(baseName).arg(i);
        }
        else
        {
            newFilename = QString("%1_%2.%3").arg(baseName).arg(i).arg(suffix);
        }

        QString resultFilePath = resultDirectory + "/" + newFilename;
        if (!QFileInfo::exists(resultFilePath))
        {
            qDebug() << "Generated new filename:" << newFilename;
            return resultFilePath;
        }
    }

    // Если не найдено свободное имя, возвращаем пустую строку
    qDebug() << "Could not find available filename:" << filename;
    return QString();
}

void FileSearchService::pause()
{
    QMutexLocker lock(&m_mutex);

    if (m_state == ServiceState::Running)
    {
        m_state = ServiceState::Paused;
    }
}

void FileSearchService::resume()
{
    {
        QMutexLocker lock(&m_mutex);

        if (m_state == ServiceState::Paused)
        {
            m_state = ServiceState::Running;
        }
    }

    m_pauseCondition.wakeAll();
}

void FileSearchService::stop()
{
    {
        QMutexLocker lock(&m_mutex);

        m_state = ServiceState::Stopped;
    }

    m_pauseCondition.wakeAll();
}


