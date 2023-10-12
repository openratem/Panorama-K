#include "loader/LibLoader.h"
#include <QtCore>

int main(int argc, char* argv[])
{
    // создание экземпляра программы
    QCoreApplication a(argc, argv);

    // Проверка на количество аргументов.
    if (a.arguments().size() > 1) {
        for (const QString& arg : a.arguments()) {
            // Проверка на аргумент "path".
            if (arg.contains("--path=")) {
                QStringList splitString = arg.split("=");

                if (splitString.size() > 1) {
                    QFileInfo info(splitString.at(1));

                    // Проверяем, существует ли файл.
                    if (!info.exists()) {
                        qDebug() << "Файл: " << splitString.at(1) << " не существует";
                        break;
                    }

                    // Загружаем библиотеку.
                    QString libraryPath = "./" + splitString.at(1);
                    LibLoader libLoader(libraryPath.toStdString());

                    // Получаем указатель на первый элемент в списке устройств.
                    DeviceInfo* pDeviceinfo = libLoader.getDevices();

                    // Выводим список поддерживаемых устройств.
                    qDebug() << "Список поддерживаемых устройств:";
                    for (DeviceInfo* pInfo = pDeviceinfo->next; pInfo->next != nullptr;) {
                        qDebug() << pInfo->name;
                        pInfo = pInfo->next;
                    }
                    break;
                }
            }
        }
    } else {
        qDebug() << "Укажите путь до файла библиотеки, например: --path=path/to/library/libPanoramaK.so";
    }

    return 0;
}
