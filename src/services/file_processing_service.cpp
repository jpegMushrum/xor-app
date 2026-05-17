#include "file_processing_service.h"
#include <QFile>
#include <QDebug>

FileProcessingService::FileProcessingService(QObject *parent)
    : IFileProcessingService(parent)
{
}

void FileProcessingService::processFile(
    const QString &sourceFile,
    const QVector<quint8> &xorMask,
    const QString &tempFile
) {
    processFileWithOffset(sourceFile, xorMask, tempFile, 0);
}

// TODO: перевести ошибки на русский, а то по привычке написал на англе
void FileProcessingService::processFileWithOffset(
    const QString &sourceFile,
    const QVector<quint8> &xorMask,
    const QString &tempFile,
    qint64 offset)
{
    {
        QMutexLocker lock(&m_mutex);

        m_state = ServiceState::Running;
    }

    if (xorMask.empty())
    {
        qWarning() << "XOR mask is empty";
        emit processingError("XOR mask is empty");
        return;
    }

    QFile sourceFileObj(sourceFile);
    if (!sourceFileObj.open(QIODevice::ReadOnly))
    {
        qWarning() << "Cannot open source file:" << sourceFile;
        emit processingError("Cannot open source file: " + sourceFile);
        return;
    }

    QFile tempFileObj(tempFile);
    if (!tempFileObj.open(QIODevice::WriteOnly))
    {
        qWarning() << "Cannot open temp file for writing:" << tempFile;
        sourceFileObj.close();
        emit processingError("Cannot open source file: " + sourceFile);
        return;
    }

    if (offset > 0 && !sourceFileObj.seek(offset))
    {
        qWarning() << "Cannot seek to offset" << offset;
        sourceFileObj.close();
        tempFileObj.close();
        emit processingError(QString("Cannot seek to offset %1").arg(offset));
        return;
    }

    // Обрабатываем файл блоками
    QByteArray buffer;
    size_t maskSize = xorMask.size();
    size_t maskIndex = 0;

    maskIndex = offset % maskSize;


    while (!sourceFileObj.atEnd())
    {
        {
            QMutexLocker lock(&m_mutex);

            while (m_state == ServiceState::Paused)
            {
                m_pauseCondition.wait(&m_mutex);
            }

            if (m_state == ServiceState::Stopped)
            {
                sourceFileObj.close();
                tempFileObj.close();

                return;
            }
        }

        buffer = sourceFileObj.read(m_bufferSize);
        if (buffer.isEmpty())
        {
            break;
        }

        for (int i = 0; i < buffer.size(); ++i)
        {
            buffer[i] = buffer[i] ^ xorMask[maskIndex];
            maskIndex = (maskIndex + 1) % maskSize;
        }

        qint64 written = tempFileObj.write(buffer);
        if (written != buffer.size())
        {
            qWarning() << "Write error: written" << written << "bytes of" << buffer.size();
            sourceFileObj.close();
            tempFileObj.close();
            emit processingError(QString("Write error: written %1 bytes of %2").arg(written).arg(buffer.size()));
            return;
        }
    }

    sourceFileObj.close();
    tempFileObj.close();

    qDebug() << "File processed successfully:" << sourceFile << "->" << tempFile;
    emit fileProcessed(tempFile);
}

void FileProcessingService::setBufferSize(size_t bufferSize)
{
    m_bufferSize = bufferSize;
}

size_t FileProcessingService::bufferSize() const
{
    return m_bufferSize;
}

void FileProcessingService::pause()
{
    QMutexLocker lock(&m_mutex);

    if (m_state == ServiceState::Running)
    {
        m_state = ServiceState::Paused;
    }
}

void FileProcessingService::resume()
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

void FileProcessingService::stop()
{
    {
        QMutexLocker lock(&m_mutex);

        m_state = ServiceState::Stopped;
    }

    m_pauseCondition.wakeAll();
}
