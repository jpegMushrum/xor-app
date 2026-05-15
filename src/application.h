#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>

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

class Application : public QMainWindow
{
    Q_OBJECT

public:
    explicit Application(QWidget *parent = nullptr);
    ~Application() override;

private:
    void setUi();
    void setController();
    void setStyle();

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
};
#endif // APPLICATION_H
