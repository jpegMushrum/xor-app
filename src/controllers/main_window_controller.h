#ifndef MAIN_WINDOW_CONTROLLER_H
#define MAIN_WINDOW_CONTROLLER_H

#include <QObject>
#include <QString>

class Orchestrator;
class Application;

// Контроллер главного окна - управляет взаимодействием UI и логики
class MainWindowController : public QObject
{
    Q_OBJECT

public:
    explicit MainWindowController(Application *mainWindow, QObject *parent = nullptr);
    ~MainWindowController() override;

    // Инициализировать контроллер
    void initialize();

    // Обработать нажатие кнопки "Старт"
    void onStartButtonClicked();

    // Обработать нажатие кнопки "Остановить"
    void onPauseButtonClicked();

    // Обработать нажатие кнопки "Отменить"
    void onCancelButtonClicked();

    // Обработать выбор исходной директории
    void onSourceDirectorySelected(const QString &directory);

    // Обработать выбор результирующей директории
    void onResultDirectorySelected(const QString &directory);

    // Обработать изменение маски файлов
    void onFileMaskChanged(const QString &mask);

    // Обработать изменение состояния флага удаления
    void onDeleteSourceFilesToggled(bool checked);

    // Обновить UI с информацией о прогрессе
    void updateProgressUI();

    // Показать сообщение об ошибке
    void showErrorMessage(const QString &message);

    // Показать сообщение об успехе
    void showSuccessMessage(const QString &message);

private:
    Application *m_mainWindow;
    Orchestrator *m_orchestrator;

    // Подключить сигналы и слоты
    void connectSignals();
};

#endif // MAIN_WINDOW_CONTROLLER_H
