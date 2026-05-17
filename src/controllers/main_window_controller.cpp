#include "main_window_controller.h"
#include "../application.h"
#include "../services/orchestrator.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QRegularExpression>

MainWindowController::MainWindowController(Application *mainWindow, QObject *parent)
    : QObject(parent), m_mainWindow(mainWindow), m_orchestrator(nullptr)
{
}

MainWindowController::~MainWindowController()
{
}

void MainWindowController::initialize(Orchestrator *orchestrator)
{
    if (!orchestrator) {
        qWarning() << "nullptr orchestartor";
        return;
    }

    m_orchestrator = orchestrator;

    // Подключить сигналы от UI
    connect(m_mainWindow, &Application::browseSourceDirectoryRequested,
            this, &MainWindowController::onBrowseSourceDirectory);
    connect(m_mainWindow, &Application::browseTargetDirectoryRequested,
            this, &MainWindowController::onBrowseTargetDirectory);
    connect(m_mainWindow, &Application::startProcessing,
            this, &MainWindowController::onStartButtonClicked);
    connect(m_mainWindow, &Application::pauseProcessing,
            this, &MainWindowController::onPauseButtonClicked);
    connect(m_mainWindow, &Application::cancelProcessing,
            this, &MainWindowController::onCancelButtonClicked);

    // Подключить сигналы от оркестратора
    if (m_orchestrator)
    {
        connect(m_orchestrator, &Orchestrator::filesMapped,
                this, [this](int fileCount)
                { qDebug() << "Files mapped:" << fileCount; });

        connect(m_orchestrator, &Orchestrator::workingStateChanged,
                this, [this](WorkingState state) {
            m_mainWindow->updateStatusBar(state);
        });

        connect(m_orchestrator, &Orchestrator::processingError,
                this, [this](const QString& msg) {
            showRuntimeError(msg);
        });

        connect(m_orchestrator, &Orchestrator::processingFinished,
                this, [this]() {
            qDebug() << "Processing finished";
        });
    }

    m_mainWindow->updateStatusBar(WorkingState::Idle);
}

void MainWindowController::onStartButtonClicked()
{
    if (!validateInputs())
        return;

    if (m_orchestrator)
    {
        FileDuplicationRule duplicationRule = parseDuplicationRule(m_mainWindow->getDuplicationRule());

        QVector<quint8> xorMask = parseXorMask(m_mainWindow->getXorMask());

        m_orchestrator->setSearchParameters(
            m_mainWindow->getSourceDirectory(),
            m_mainWindow->getTargetDirectory(),
            m_mainWindow->getFileMask(),
            duplicationRule,
            xorMask,
            m_mainWindow->isDeleteSourceFilesChecked());

        m_orchestrator->startProcessing();
    }
}

void MainWindowController::onPauseButtonClicked()
{
    if (m_orchestrator)
        m_orchestrator->pauseProcessing();
}

void MainWindowController::onCancelButtonClicked()
{
    if (m_orchestrator)
        m_orchestrator->cancelProcessing();
}

void MainWindowController::onBrowseSourceDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(
        m_mainWindow,
        "Выберите исходную папку",
        m_mainWindow->getSourceDirectory(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        m_mainWindow->setSourceDirectory(dir);
    }
}

void MainWindowController::onBrowseTargetDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(
        m_mainWindow,
        "Выберите папку для результатов",
        m_mainWindow->getTargetDirectory(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty())
    {
        m_mainWindow->setTargetDirectory(dir);
    }
}

bool MainWindowController::validateInputs()
{
    QString sourceDir = m_mainWindow->getSourceDirectory();
    QString targetDir = m_mainWindow->getTargetDirectory();
    QString fileMask = m_mainWindow->getFileMask();
    QString duplicationRule = m_mainWindow->getDuplicationRule();
    QString xorMask = m_mainWindow->getXorMask();
    QString errorMessages = "";

    if (sourceDir.isEmpty())
    {
        errorMessages = "Укажите исходную папку";
    }

    if (targetDir.isEmpty())
    {
        errorMessages += "\nУкажите папку для результатов";
    }

    if (fileMask.isEmpty())
    {
        errorMessages += "\nУкажите маску файлов";
    }

    if (!(duplicationRule == "Перезаписывать" ||
          duplicationRule == "Добавлять суффикс с номером" ||
          duplicationRule == "Пропускать")) {
        errorMessages += "\nУкажите политику повторяющихся файлов из списка";
    }

    if (!isValidXorMask(xorMask)) {
        errorMessages += "\nXor-маска не валидна";
    }

    if (!errorMessages.isEmpty())
    {
        showValidationError(errorMessages);
        return false;
    }

    return true;
}

FileDuplicationRule MainWindowController::parseDuplicationRule(const QString &rule)
{
    if (rule == "Перезаписывать")
    {
        return FileDuplicationRule::Overwrite;
    }

    if (rule == "Добавлять суффикс с номером")
    {
        return FileDuplicationRule::CreateCopy;
    }

    if (rule == "Пропускать")
    {
        return FileDuplicationRule::Skip;
    }

    throw std::runtime_error(QString("Unaccessible combo box value: " + rule).toStdString());
}

QVector<quint8> MainWindowController::parseXorMask(const QString &mask)
{
    QString normalized = mask.trimmed();
    if (normalized.length() != 16)
    {
        throw std::runtime_error(QString("Unaccessible xor mask value: " + mask).toStdString());
    }

    QByteArray bytes =
        QByteArray::fromHex(
            normalized.toUtf8());

    if (bytes.size() != 8)
    {
        throw std::runtime_error(QString("Unaccessible xor mask value: " + mask).toStdString());
    }

    QVector<quint8> result;
    result.reserve(8);

    for (char byte : bytes)
    {
        result.push_back(
            static_cast<quint8>(byte));
    }

    return result;
}

bool MainWindowController::isValidXorMask(const QString &mask) const
{
    static const QRegularExpression regex("^[0-9A-Fa-f]{16}$");

    return regex.match(mask).hasMatch();
}

void MainWindowController::showValidationError(const QString &message)
{
    QMessageBox::warning(m_mainWindow, "Ошибка валидации", message);
}

void MainWindowController::showRuntimeError(const QString &message)
{
    QMessageBox::warning(m_mainWindow, "Ошибка во время исполнения", message);
}
