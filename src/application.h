#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>
#include "utils/structures.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Application;
}
QT_END_NAMESPACE

class QLineEdit;
class QCheckBox;
class QComboBox;
class QPushButton;
class QToolButton;
class QStatusBar;
class MainWindowController;
class IFileSearchService;
class IFileProcessingService;
class FileProcessor;
class Orchestrator;
class QThread;
class QProgressBar;
class QLabel;
class QCloseEvent;

class Application : public QMainWindow
{
    Q_OBJECT

public:
    explicit Application(QWidget *parent = nullptr);
    ~Application() override;

    // Получить путь к исходной папке
    QString getSourceDirectory() const;

    // Получить путь к папке результатов
    QString getTargetDirectory() const;

    // Получить маску файлов
    QString getFileMask() const;

    // Получить правило из выпадающего списка
    QString getDuplicationRule() const;

    // Получить xor-маску
    QString getXorMask() const;

    // Получить состояние флага удаления исходных файлов
    bool isDeleteSourceFilesChecked() const;

    // Получить состояние флага работы по таймеру
    bool isRestartByTimerChecked() const;

    // Получить значение таймера в секундах
    int getTimerSeconds() const;

    // Установить состояние исходной папки
    void setSourceDirectory(const QString &path);

    // Установить состояние папки результатов
    void setTargetDirectory(const QString &path);

    // Обновить состояние приложения
    void updateState(WorkingState state);

    // Включить/отключить кнопку старта
    void setStartButtonEnabled(bool enabled);

    // Включить/отключить кнопку паузы
    void setPauseButtonEnabled(bool enabled);

    // Включить/отключить кнопку отмены
    void setCancelButtonEnabled(bool enabled);

    // Включить/отключить кнопки выбора папок
    void setBrowseButtonsEnabled(bool enabled);

    // Обновить статус бар
    void updateStatusBar(WorkingState state);

    // Ограничить доступность полей для ввода
    void setInputsEnabled(bool enabled);

    // Отобразить текущий прогресс
    void updateProgress(int current, int total, const QString& filename);

signals:
    // Сигналы для контроллера
    void browseSourceDirectoryRequested();
    void browseTargetDirectoryRequested();
    void startProcessing();
    void pauseProcessing();
    void cancelProcessing();

private:
    void setUi();
    void setController();
    void setDependencies();
    void setStyle();
    void connectUISignals();

    // Обработка правильного завершения программы
    void closeEvent(QCloseEvent *event) override;

    // Зависимости
    QThread *m_workerThread;
    IFileSearchService *m_searchService;
    IFileProcessingService *m_processingService;
    FileProcessor *m_fileProcessor;
    Orchestrator *m_orchestrator;

    // Виджеты
    QLineEdit *m_filesMask;
    QCheckBox *m_removeSourceFilesCheck;
    QLineEdit *m_sourceDir;
    QLineEdit *m_targetDir;
    QToolButton *m_browseSourceButton;
    QToolButton *m_browseTargetButton;
    QComboBox *m_similarFilesCombo;
    QCheckBox *m_restartByTimerCheck;
    QLineEdit *m_timerSeconds;
    QLineEdit *m_xorMask;
    QPushButton *m_startButton;
    QToolButton *m_pauseButton;
    QToolButton *m_cancelButton;
    QProgressBar *m_progressBar;
    QLabel *m_progressLabel;

    MainWindowController *m_controller;
};
#endif // APPLICATION_H
