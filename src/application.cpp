#include "application.h"
#include "controllers/main_window_controller.h"
#include <QWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QFileDialog>
#include <QToolButton>
#include <QStyle>
#include <QComboBox>
#include <QPushButton>
#include <QFile>
#include <QApplication>
#include <QResource>
#include <QDirIterator>
#include <QThread>
#include <QProgressBar>
#include <QCloseEvent>

#include "services/orchestrator.h"
#include "services/file_processing_service.h"
#include "services/file_search_service.h"
#include "utils/file_processor.h"

Application::Application(QWidget *parent)
    : QMainWindow(parent), m_controller(nullptr), m_workerThread(nullptr),
      m_searchService(nullptr), m_processingService(nullptr),
      m_fileProcessor(nullptr), m_orchestrator(nullptr)
{
    setUi();
    setStyle();
    connectUISignals();
    setDependencies();
    setController();
}

void Application::setUi()
{
    setFixedSize(600, 280);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setAlignment(Qt::AlignCenter);

    QHBoxLayout *firstRowLayout = new QHBoxLayout();
    firstRowLayout->setSpacing(3);

    m_filesMask = new QLineEdit(central);
    m_filesMask->setPlaceholderText("Маска входных файлов");
    m_filesMask->setClearButtonEnabled(true);
    firstRowLayout->addWidget(m_filesMask);

    firstRowLayout->addSpacing(15);

    QLabel *removeSourceFilesLabel = new QLabel(central);
    removeSourceFilesLabel->setText("Удалять исходные файлы:");
    firstRowLayout->addWidget(removeSourceFilesLabel);

    m_removeSourceFilesCheck = new QCheckBox(central);
    firstRowLayout->addWidget(m_removeSourceFilesCheck);

    mainLayout->addLayout(firstRowLayout);

    QHBoxLayout *secondRowLayout = new QHBoxLayout();
    secondRowLayout->setSpacing(3);

    m_sourceDir = new QLineEdit(central);
    m_sourceDir->setPlaceholderText("Исходная папка");
    secondRowLayout->addWidget(m_sourceDir);

    m_browseSourceButton = new QToolButton(central);
    m_browseSourceButton->setIcon(QIcon(":/style/icons/folder.png"));
    secondRowLayout->addWidget(m_browseSourceButton);

    secondRowLayout->addSpacing(10);

    m_targetDir = new QLineEdit(central);
    m_targetDir->setPlaceholderText("Папка для результатов");
    secondRowLayout->addWidget(m_targetDir);

    m_browseTargetButton = new QToolButton(central);
    m_browseTargetButton->setIcon(QIcon(":/style/icons/folder.png"));
    secondRowLayout->addWidget(m_browseTargetButton);

    mainLayout->addLayout(secondRowLayout);

    QHBoxLayout *thirdRowLayout = new QHBoxLayout();
    thirdRowLayout->setSpacing(3);

    QLabel *similarFilesText = new QLabel(central);
    similarFilesText->setText("При повторении имен файлов:");
    thirdRowLayout->addWidget(similarFilesText);

    m_similarFilesCombo = new QComboBox(central);
    m_similarFilesCombo->addItem("Перезаписывать");
    m_similarFilesCombo->addItem("Добавлять суффикс с номером");
    m_similarFilesCombo->addItem("Пропускать");
    thirdRowLayout->addWidget(m_similarFilesCombo);

    thirdRowLayout->addStretch();
    mainLayout->addLayout(thirdRowLayout);

    QHBoxLayout *fourthRowLayout = new QHBoxLayout();
    fourthRowLayout->setSpacing(3);

    QLabel *restartByTimerLabel = new QLabel(central);
    restartByTimerLabel->setText("Работа по таймеру:");
    fourthRowLayout->addWidget(restartByTimerLabel);

    m_restartByTimerCheck = new QCheckBox(central);
    fourthRowLayout->addWidget(m_restartByTimerCheck);

    fourthRowLayout->addSpacing(15);

    m_timerSeconds = new QLineEdit(central);
    m_timerSeconds->setText("600");
    m_timerSeconds->setPlaceholderText("600");
    m_timerSeconds->setMaximumWidth(100);
    m_timerSeconds->setDisabled(true);
    connect(m_restartByTimerCheck, &QCheckBox::toggled, m_timerSeconds, &QLineEdit::setEnabled);
    fourthRowLayout->addWidget(m_timerSeconds);

    QLabel *secondsLabel = new QLabel(central);
    secondsLabel->setText("cекунд");
    fourthRowLayout->addWidget(secondsLabel);

    fourthRowLayout->addStretch();
    mainLayout->addLayout(fourthRowLayout);

    QHBoxLayout *fifthRowLayout = new QHBoxLayout();
    fifthRowLayout->setSpacing(3);

    QLabel *editMaskLabel = new QLabel(central);
    editMaskLabel->setText("8-байтная маска:");
    fifthRowLayout->addWidget(editMaskLabel);

    m_xorMask = new QLineEdit(central);
    m_xorMask->setPlaceholderText("0123456789ABCDEF");
    m_xorMask->setMaximumWidth(130);
    fifthRowLayout->addWidget(m_xorMask);

    fifthRowLayout->addStretch();

    m_startButton = new QPushButton(central);
    m_startButton->setMinimumWidth(150);
    m_startButton->setText("Старт");
    fifthRowLayout->addWidget(m_startButton);

    mainLayout->addLayout(fifthRowLayout);

    QHBoxLayout *sixthRowLayout = new QHBoxLayout();
    sixthRowLayout->setSpacing(3);

    QHBoxLayout *progressLayout = new QHBoxLayout();

    m_progressLabel = new QLabel(central);
    m_progressLabel->setText("Нет активной обработки");
    progressLayout->addWidget(m_progressLabel);

    m_progressBar = new QProgressBar(central);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);

    progressLayout->addWidget(m_progressBar);
    sixthRowLayout->addLayout(progressLayout);

    m_pauseButton = new QToolButton(central);
    m_pauseButton->setIcon(QIcon(":/style/icons/pause.png"));
    m_pauseButton->setDisabled(true);
    sixthRowLayout->addWidget(m_pauseButton);

    m_cancelButton = new QToolButton(central);
    m_cancelButton->setIcon(QIcon(":/style/icons/stop-button.png"));
    m_cancelButton->setDisabled(true);
    sixthRowLayout->addWidget(m_cancelButton);

    mainLayout->addLayout(sixthRowLayout);
}

void Application::setStyle()
{
    QFile styleFile(":/style/application.qss");
    if (styleFile.open(QFile::ReadOnly))
    {
        QString style = QLatin1String(styleFile.readAll());
        qApp->setStyleSheet(style);
        styleFile.close();
    }
}

void Application::setDependencies()
{
    m_workerThread = new QThread(this);

    m_processingService = new FileProcessingService();
    m_searchService = new FileSearchService();
    m_fileProcessor = new FileProcessor();

    m_processingService->moveToThread(m_workerThread);
    m_searchService->moveToThread(m_workerThread);
    m_fileProcessor->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::finished, m_processingService, &QObject::deleteLater);
    connect(m_workerThread, &QThread::finished, m_searchService, &QObject::deleteLater);
    connect(m_workerThread, &QThread::finished, m_fileProcessor, &QObject::deleteLater);

    m_orchestrator = new Orchestrator(m_searchService, m_processingService, m_fileProcessor, this);
    m_workerThread->start();
}

void Application::setController()
{
    m_controller = new MainWindowController(this, this);
    m_controller->initialize(m_orchestrator);
}

Application::~Application()
{
    if (m_workerThread && m_workerThread->isRunning())
    {
        m_workerThread->quit();
        m_workerThread->wait();
    }
}

QString Application::getSourceDirectory() const
{
    return m_sourceDir->text();
}

QString Application::getTargetDirectory() const
{
    return m_targetDir->text();
}

QString Application::getDuplicationRule() const
{
    return m_similarFilesCombo->currentText();
}

QString Application::getXorMask() const
{
    return m_xorMask->text();
}

QString Application::getFileMask() const
{
    return m_filesMask->text();
}

bool Application::isDeleteSourceFilesChecked() const
{
    return m_removeSourceFilesCheck->isChecked();
}

bool Application::isRestartByTimerChecked() const
{
    return m_restartByTimerCheck->isChecked();
}

int Application::getTimerSeconds() const
{
    bool ok = false;
    int value = m_timerSeconds->text().toInt(&ok);
    return ok ? value : 0;
}

void Application::setSourceDirectory(const QString &path)
{
    m_sourceDir->setText(path);
}

void Application::setTargetDirectory(const QString &path)
{
    m_targetDir->setText(path);
}

void Application::updateState(WorkingState state)
{
    QString statusText;
    QString startButtonText;

    switch (state)
    {
    case WorkingState::Idle:
    {
        statusText = "";
        startButtonText = "Старт";

        setInputsEnabled(true);

        setStartButtonEnabled(true);
        setPauseButtonEnabled(false);
        setCancelButtonEnabled(false);

        break;
    }

    case WorkingState::Running:
    {
        statusText = "Обработка...";
        startButtonText = "Старт";

        setInputsEnabled(false);

        setStartButtonEnabled(false);
        setPauseButtonEnabled(true);
        setCancelButtonEnabled(true);

        break;
    }

    case WorkingState::Paused:
    {
        statusText = "Приостановлено";
        startButtonText = "Продолжить";

        setInputsEnabled(false);

        setStartButtonEnabled(true);
        setPauseButtonEnabled(false);
        setCancelButtonEnabled(true);

        break;
    }

    case WorkingState::Cancelled:
    {
        statusText = "Отменено";
        startButtonText = "Старт";

        setInputsEnabled(true);

        setStartButtonEnabled(true);
        setPauseButtonEnabled(false);
        setCancelButtonEnabled(false);

        break;
    }

    case WorkingState::OnTimer:
    {
        statusText = "Перезапуск по таймеру";
        startButtonText = "Старт";

        setInputsEnabled(false);

        setStartButtonEnabled(true);
        setPauseButtonEnabled(false);
        setCancelButtonEnabled(true);

        break;
    }
    }

    m_progressLabel->setText(statusText);
    m_startButton->setText(startButtonText);
}

void Application::setInputsEnabled(bool enabled)
{
    m_filesMask->setEnabled(enabled);

    m_sourceDir->setEnabled(enabled);
    m_targetDir->setEnabled(enabled);

    m_browseSourceButton->setEnabled(enabled);
    m_browseTargetButton->setEnabled(enabled);

    m_similarFilesCombo->setEnabled(enabled);

    m_removeSourceFilesCheck->setEnabled(enabled);

    m_restartByTimerCheck->setEnabled(enabled);

    // timer seconds зависит ещё и от checkbox
    m_timerSeconds->setEnabled(enabled && m_restartByTimerCheck->isChecked());

    m_xorMask->setEnabled(enabled);
}

void Application::setStartButtonEnabled(bool enabled)
{
    m_startButton->setEnabled(enabled);
}

void Application::setPauseButtonEnabled(bool enabled)
{
    m_pauseButton->setEnabled(enabled);
}

void Application::setCancelButtonEnabled(bool enabled)
{
    m_cancelButton->setEnabled(enabled);
}

void Application::setBrowseButtonsEnabled(bool enabled)
{
    m_browseSourceButton->setEnabled(enabled);
    m_browseTargetButton->setEnabled(enabled);
}

void Application::updateProgress(
    int current,
    int total,
    const QString& filename)
{
    if (total <= 0)
    {
        m_progressBar->setValue(0);
        m_progressLabel->setText("Нет файлов");
        return;
    }

    int percent = static_cast<int>((static_cast<double>(current) / total) * 100.0);

    m_progressBar->setValue(percent);

    m_progressLabel->setText(QString("%1 / %2 : %3").arg(current).arg(total).arg(filename));
}

void Application::connectUISignals()
{
    connect(m_browseSourceButton, &QToolButton::clicked,
            this, &Application::browseSourceDirectoryRequested);
    connect(m_browseTargetButton, &QToolButton::clicked,
            this, &Application::browseTargetDirectoryRequested);
    connect(m_startButton, &QPushButton::clicked,
            this, &Application::startProcessing);
    connect(m_pauseButton, &QToolButton::clicked,
            this, &Application::pauseProcessing);
    connect(m_cancelButton, &QToolButton::clicked,
            this, &Application::cancelProcessing);
}

void Application::closeEvent(QCloseEvent *event)
{
    if (!m_orchestrator)
    {
        event->accept();
        return;
    }

    WorkingState state = m_orchestrator->getWorkingState();

    switch (state)
    {
    case WorkingState::Running:
    case WorkingState::Paused:
    case WorkingState::OnTimer:
    {
        m_orchestrator->cancelProcessing();
        break;
    }

    case WorkingState::Idle:
    case WorkingState::Cancelled:
    {
        break;
    }
    }

    event->accept();
}
