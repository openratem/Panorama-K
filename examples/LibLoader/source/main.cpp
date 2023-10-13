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

#ifdef Q_OS_WINDOWS
                    LibLoader libLoader(splitString.at(1).toStdWString());
#else
                    LibLoader libLoader(splitString.at(1).toStdString());
#endif

                    // Получаем указатель на первый элемент в списке устройств.
                    DeviceInfo* pDeviceinfo = libLoader.getDevices();

                    // Выводим список поддерживаемых устройств.
                    qDebug() << "Список поддерживаемых устройств:";
                    for (DeviceInfo* pInfo = pDeviceinfo->next; pInfo->next != nullptr;) {
                        qDebug() << " - " << pInfo->name;
                        pInfo = pInfo->next;
                    }
                    break;
                }
            }
        }
    } else {
#ifdef Q_OS_WINDOWS
        qDebug() << "Укажите путь до файла библиотеки, например: --path=D:\\path\\to\\libPanoramaK.dll";
#elif defined(Q_OS_MACOS)
        qDebug() << "Укажите путь до файла библиотеки, например: --path=path/to/library/libPanoramaK.dylib";
#else
        qDebug() << "Укажите путь до файла библиотеки, например: --path=path/to/library/libPanoramaK.so";
#endif
    }

    return 0;
}
