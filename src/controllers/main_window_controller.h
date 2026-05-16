#ifndef MAIN_WINDOW_CONTROLLER_H
#define MAIN_WINDOW_CONTROLLER_H

#include <QObject>
#include <QString>
#include "../utils/structures.h"

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

private slots:
    // Обработать нажатие кнопки "Старт"
    void onStartButtonClicked();

    // Обработать нажатие кнопки "Остановить"
    void onPauseButtonClicked();

    // Обработать нажатие кнопки "Отменить"
    void onCancelButtonClicked();

    // Обработать запрос на выбор исходной директории
    void onBrowseSourceDirectory();

    // Обработать запрос на выбор результирующей директории
    void onBrowseTargetDirectory();

private:
    // Проверить корректность параметров
    bool validateInputs();

    // Показать ошибку валидации
    void showValidationError(const QString &message);

    // Показать ошибку во время исполнения
    void showRuntimeError(const QString &message);

    Application *m_mainWindow;
    Orchestrator *m_orchestrator;
    WorkingState m_currentState;
};

#endif // MAIN_WINDOW_CONTROLLER_H
