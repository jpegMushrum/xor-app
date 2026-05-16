#include "main_window_controller.h"
#include "../application.h"
#include "../services/orchestrator.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindowController::MainWindowController(Application *mainWindow, QObject *parent)
    : QObject(parent), m_mainWindow(mainWindow), m_orchestrator(nullptr), m_currentState(WorkingState::Idle)
{
}

MainWindowController::~MainWindowController()
{
}

void MainWindowController::initialize()
{
    m_orchestrator = m_mainWindow->getOrchestrator();

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
        connect(m_orchestrator, &Orchestrator::processingStarted,
                this, [this]()
                {
                    m_currentState = WorkingState::Running;
                    m_mainWindow->updateStatusBar(m_currentState);
        });
        connect(m_orchestrator, &Orchestrator::filesMapped,
                this, [this](int fileCount)
                { qDebug() << "Files mapped:" << fileCount; });
        connect(m_orchestrator, &Orchestrator::processingFinished,
                this, [this]()
                {
                    m_currentState = WorkingState::Idle;
                    m_mainWindow->updateStatusBar(m_currentState);
        });
        connect(m_orchestrator, &Orchestrator::processingError,
                this, &MainWindowController::showValidationError);
    }

    m_mainWindow->updateStatusBar(m_currentState);
}

void MainWindowController::onStartButtonClicked()
{
    if (!validateInputs())
        return;

    if (m_currentState == WorkingState::Paused)
    {
        if (m_orchestrator && m_orchestrator->resumeProcessing())
        {
            m_currentState = WorkingState::Running;
            m_mainWindow->updateStatusBar(m_currentState);
        }
    }
    else
    {
        // Установить параметры поиска в оркестратор
        if (m_orchestrator)
        {
            m_orchestrator->setSearchParameters(
                m_mainWindow->getSourceDirectory(),
                m_mainWindow->getTargetDirectory(),
                m_mainWindow->getFileMask());

            // Запустить обработку
            m_orchestrator->startProcessing();
        }
    }
}

void MainWindowController::onPauseButtonClicked()
{
    if (m_currentState == WorkingState::Running)
    {
        m_currentState = WorkingState::Paused;
        m_mainWindow->updateStatusBar(m_currentState);
        if (m_orchestrator)
            m_orchestrator->pauseProcessing();
    }
}

void MainWindowController::onCancelButtonClicked()
{
    if (m_currentState == WorkingState::Running || m_currentState == WorkingState::Paused)
    {
        m_currentState = WorkingState::Cancelled;
        m_mainWindow->updateStatusBar(m_currentState);
        if (m_orchestrator)
            m_orchestrator->cancelProcessing();

        m_currentState = WorkingState::Idle;
        m_mainWindow->updateStatusBar(m_currentState);
    }
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

    if (!errorMessages.isEmpty())
    {
        showValidationError(errorMessages);
        return false;
    }

    return true;
}

void MainWindowController::showValidationError(const QString &message)
{
    QMessageBox::warning(m_mainWindow, "Ошибка валидации", message);
}
