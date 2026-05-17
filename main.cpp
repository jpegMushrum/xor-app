#include "src/application.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Application w;
    w.show();
    return QCoreApplication::exec();
}
// TODO:
// Асинхронный вызов file processor
// Пофиксить WorkingState
// Удаление исходных файлов
// Работа по таймеру
// Остановка и отмена
// Статус бар
