#include <QIcon>
#include <QApplication>
#include "common.h"
#include "Kernel.h"

int main(int argc, char *argv[])
{
    // создание экземпляра программы
    QApplication a(argc, argv);

    // инициализация основных параметров программы
    // создание каталогов для хранения служебных файлов
    Common::initialize();

    // запуск программы
    Kernel kenel;
    kenel.run();

    // вход в основной цикл обработки сообщений
    return a.exec();
}
