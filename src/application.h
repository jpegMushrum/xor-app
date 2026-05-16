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

    // Получить состояние флага удаления исходных файлов
    bool isDeleteSourceFilesChecked() const;

    // Установить состояние исходной папки
    void setSourceDirectory(const QString &path);

    // Установить состояние папки результатов
    void setTargetDirectory(const QString &path);

    // Обновить статус бар с состоянием работы
    void updateStatusBar(WorkingState state);

    // Включить/отключить кнопку старта
    void setStartButtonEnabled(bool enabled);

    // Включить/отключить кнопку паузы
    void setPauseButtonEnabled(bool enabled);

    // Включить/отключить кнопку отмены
    void setCancelButtonEnabled(bool enabled);

    // Включить/отключить кнопки выбора папок
    void setBrowseButtonsEnabled(bool enabled);

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
    void setStyle();

    // Подключить сигналы и слоты
    void connectUISignals();

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
    QLineEdit *m_editMask;
    QPushButton *m_startButton;
    QToolButton *m_pauseButton;
    QToolButton *m_cancelButton;
    QStatusBar *m_statusBar;

    MainWindowController *m_controller;
};
#endif // APPLICATION_H
