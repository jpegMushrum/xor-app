#include "main_window_controller.h"

MainWindowController::MainWindowController(Application *mainWindow, QObject *parent)
    : QObject(parent), m_mainWindow(mainWindow), m_orchestrator(nullptr)
{
}

MainWindowController::~MainWindowController()
{
}

void MainWindowController::initialize()
{
}

void MainWindowController::onStartButtonClicked()
{
}

void MainWindowController::onPauseButtonClicked()
{
}

void MainWindowController::onCancelButtonClicked()
{
}

void MainWindowController::onSourceDirectorySelected(const QString &directory)
{
}

void MainWindowController::onResultDirectorySelected(const QString &directory)
{
}

void MainWindowController::onFileMaskChanged(const QString &mask)
{
}

void MainWindowController::onDeleteSourceFilesToggled(bool checked)
{
}

void MainWindowController::updateProgressUI()
{
}

void MainWindowController::showErrorMessage(const QString &message)
{
}

void MainWindowController::showSuccessMessage(const QString &message)
{
}
