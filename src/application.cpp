#include "application.h"
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
#include <QStatusBar>
#include <QFile>
#include <QApplication>
#include <QResource>
#include <QDirIterator>

Application::Application(QWidget *parent)
    : QMainWindow(parent)
{
    setUi();
    setStyle();
    setController();
}

void Application::setUi()
{
    setFixedSize(600, 260);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(15);

    QHBoxLayout *firstRowLayout = new QHBoxLayout(central);
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

    QHBoxLayout *secondRowLayout = new QHBoxLayout(central);
    secondRowLayout->setSpacing(2);

    m_sourceDir = new QLineEdit(central);
    m_sourceDir->setPlaceholderText("Исходная папка");
    secondRowLayout->addWidget(m_sourceDir);

    m_browseSourceButton = new QToolButton(central);
    m_browseSourceButton->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    secondRowLayout->addWidget(m_browseSourceButton);

    secondRowLayout->addSpacing(10);

    m_targetDir = new QLineEdit(central);
    m_targetDir->setPlaceholderText("Папка для результатов");
    secondRowLayout->addWidget(m_targetDir);

    m_browseTargetButton = new QToolButton(central);
    m_browseTargetButton->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));
    secondRowLayout->addWidget(m_browseTargetButton);

    mainLayout->addLayout(secondRowLayout);

    QHBoxLayout *thirdRowLayout = new QHBoxLayout(central);

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

    QHBoxLayout *fourthRowLayout = new QHBoxLayout(central);
    fourthRowLayout->setSpacing(3);

    QLabel *restartByTimerLabel = new QLabel(central);
    restartByTimerLabel->setText("Работа по таймеру:");
    fourthRowLayout->addWidget(restartByTimerLabel);

    m_restartByTimerCheck = new QCheckBox(central);
    fourthRowLayout->addWidget(m_restartByTimerCheck);

    fourthRowLayout->addSpacing(15);

    m_timerSeconds = new QLineEdit(central);
    m_timerSeconds->setText("600");
    m_timerSeconds->setMaximumWidth(100);
    m_timerSeconds->setDisabled(true);
    connect(m_restartByTimerCheck, &QCheckBox::toggled, m_timerSeconds, &QLineEdit::setEnabled);
    fourthRowLayout->addWidget(m_timerSeconds);

    QLabel *secondsLabel = new QLabel(central);
    secondsLabel->setText("cекунд");
    fourthRowLayout->addWidget(secondsLabel);

    fourthRowLayout->addStretch();
    mainLayout->addLayout(fourthRowLayout);

    QHBoxLayout *fifthRowLayout = new QHBoxLayout(central);
    fifthRowLayout->setSpacing(3);

    QLabel *editMaskLabel = new QLabel(central);
    editMaskLabel->setText("8-байтная маска:");
    fifthRowLayout->addWidget(editMaskLabel);

    m_editMask = new QLineEdit(central);
    m_editMask->setPlaceholderText("FFFFFFFF");
    m_editMask->setMaximumWidth(100);
    fifthRowLayout->addWidget(m_editMask);

    fifthRowLayout->addStretch();

    m_startButton = new QPushButton(central);
    m_startButton->setMinimumWidth(150);
    m_startButton->setText("Старт");
    fifthRowLayout->addWidget(m_startButton);

    mainLayout->addLayout(fifthRowLayout);

    QHBoxLayout *sixthRowLayout = new QHBoxLayout(central);
    sixthRowLayout->setSpacing(3);

    m_statusBar = new QStatusBar(central);
    m_statusBar->setSizeGripEnabled(false);
    sixthRowLayout->addWidget(m_statusBar);

    m_pauseButton = new QToolButton(central);
    m_pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    m_pauseButton->setDisabled(true);
    sixthRowLayout->addWidget(m_pauseButton);

    m_cancelButton = new QToolButton(central);
    m_cancelButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    m_cancelButton->setDisabled(true);
    sixthRowLayout->addWidget(m_cancelButton);

    mainLayout->addLayout(sixthRowLayout);

    central->setLayout(mainLayout);
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

void Application::setController()
{
}

Application::~Application()
{
}
