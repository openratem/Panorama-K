# Panorama-K

Проект Panorama-K это проект библиотеки с открытым исходным кодом для анализа состояния антенно-фидерного устройства 
коротковолновой радиостанции.

#### Перечень направлений прикладного использования
В процессе эксплуатации коротковолновых средств связи на их антенно-фидерное устройство АФУ (антенны, линии питания антенн) влияют внешние воздействующие факторы. Ультрафиолетовое излучение, ветер, дождь, мороз, обледенение, механические повреждения, все эти факторы приводят к разрушению элементов конструкции, электрических цепей и излучающих элементов. Своевременное обнаружение выхода из строя или ухудшения параметров АФУ является важным фактором в сохранении работоспособности радиостанции, которая имеет гораздо большую стоимость и сложна в ремонте.
Нештатные ситуации с антенно-фидерным трактом могут происходить у различных эксплуатантов радиоэлектронной аппаратуры. При её эксплуатации возникают описанные выше явления, требующие соответствующей реакции. Ниже приведен перечень направлений прикладного использования, где применяется коротковолновая радиосвязь и может быть применена защита АФУ на основе разработанной библиотеки:
 - сухопутная коротковолновая радиосвязь в составе стационарных и мобильных радиосредств метеостанций, постов авиалесоохраны, МЧС, росгвардии, компаний по добыче недр (геологоразведка, нефтедобыча, газодобыча и др.), охотоведческие и оленеводческие хозяйства;
 - морская/речная коротковолновая радиосвязь в составе стационарных береговых и мобильных узлов связи, на борту;
 - любительская коротковолновая радиосвязь в составе стационарных и мобильных радиосредств;
 - автоматизированные системы оповещения населения в составе автоматизированных систем радиосвязи для автоматического контроля состояния АФУ.

#### Описание
Библиотека PanoramaK написана на языке С++17 с использованием [фрэймворка Qt5/Qt6](https://www.qt.io/), интерфейс 
библиотеки реализован на языке С, это позволяет использовать её в программах написанных на других языках 
программирования. Библиотека компилируется в двоичную динамическую библиотеку и распространяется вместе с заголовочным 
файлом **PanoramaKCommon.h**, это файл содержит определения типов использующихся в интерфейсе библиотеки. Если проект 
программы для библиотеки PanoramaK пишется на С++17 и [фрэймворке Qt5/Qt6](https://www.qt.io/) тогда библиотеку можно
подключить в виде исходных текстов к основному проекту.

В качестве демонстрации использования библиотеки реализован проект программы **PanoramaK_App** на С++17 и 
[фрэймворке Qt5/Qt6](https://www.qt.io/). Программа имеет графический пользовательский интерфейс, собрать программу 
можно под все ОС, которые указаны для библиотеки **PanoramaK**. Программа собирается в двух вариантах, с загрузкой 
скомпилированной динамической библиотеки при запуске программы и с подключением библиотеки в виде исходных текстов. 
Таким образом демонстрируются два варианта использования библиотеки.

#### Реализованная функциональность отмечена ниже:

1. [x] Разработка алгоритмов определения качества антенно-фидерных устройств
2. [x] Разработка модуля управления радиостанциями для трансиверов линейки SunSDR
3. [x] Разработка модуля управления радиостанциями:
      ``` 
   Elecraft K2
   Elecraft K3
   Elecraft KX3
   Yaesu FT-1000
   Yaesu FT-1000 MP
   Yaesu FT-1000 
   Yaesu FT-100 D 
   Yaesu FT-2000 
   Yaesu FT-450 
   Yaesu FT-747 
   Yaesu FT-817
   Yaesu FT-840 
   Yaesu FT-847 
   Yaesu FT-857
   Yaesu FT-897
   Yaesu FT-900
   Yaesu FT-9000 v1
   Yaesu FT-920
   Yaesu FT-950
   Yaesu FT-990
   Yaesu FT-991
   Yaesu FT-DX3000
   Yaesu FT-DX-5000MP
   Icom IC-7000
   Icom IC-7100
   Icom IC-7200
   Icom IC-7300
   Icom IC-7410
   Icom IC-746
   Icom IC-746 Pro
   Icom IC-756
   Icom IC-756 Pro
   Icom IC-756 Pro II
   Icom IC-756 Pro III
   Icom IC-7600
   Icom IC-7600v2
   Icom IC-7700
   Icom IC-7700v2
   Icom IC-7800
   Icom IC-7800v3
   Icom IC-7850
   Icom IC-7851
   Icom IC-9100
   Icom IC-9100v2
   TenTec Eagle
   TenTec Orion
   TenTec Paragon II
   Kenwood TS-2000
   Kenwood TS-440
   Kenwood TS-480
   Kenwood TS-590
   Kenwood TS-690
   KenwoodTS-870
   KenwoodTS-930
      ```
3. [x] Разработка программной библиотеки под ОС Windows, Linux и MacOS
4. [x] Разработка графической оболочки для библиотеки в ОС Linux, Windows и MacOS
5. [x] Тестирование программного обеспечения в реальных условиях

## Структура проекта

Проект состоит из подпроекта библиотеки **PanoramaK** и подпроекта программы **PanoramaK_App**, которая предоставляет 
графический интерфейс для библиотеки. 

Подпроект библиотеки **PanoramaK** состоит из двух частей:
1. Модуль PanoramaK_lib в котором реализована вся логика _(расположен в library/PanoramaK_lib)_;
2. Проект библиотеки PanoramaK на языке С, является обёрткой модуля PanoramaK_lib, которая собирает библиотеку в двоичный файл _(расположен в library/PanoramaK)_.

Обёртка модуля PanoramaK_lib на языке C позволяет использовать двоичный файл библиотеки в программах написанных в других 
языках программирования.

## 🧑‍💻 Как собрать проект

Для сборки проекта необходимо иметь установленные инструменты разработки для языка С++17 и [фрэймворк Qt5/Qt6](https://www.qt.io/), а также 
необходимо установить систему контроля версий [git](https://git-scm.com).

Установить Qt5/Qt6 можно скачав онлайн инстолятор и выполнив процесс установки, такой способ позволяет установить все 
необходимые модули для библиотеки **PanoramaK**. В дистрибутивах ОС Linux установить Qt5/Qt6 можно из репозитория этой ОС, 
в качестве примера для установки всех необходимых программ и фреймворка Qt5 в ОС Linux Ubuntu 22.04 выполните следующие команды в терминале:

```
sudo apt install git
sudo apt install build-essential
sudo apt install qt5-default
sudo apt install libqt5serialport5-dev
sudo apt install libqt5websockets5-dev 
sudo apt install qtcreator
```

Библиотека **PanoramaK** использует дополнительные модули **serialport** и **websockets**, которые не входят в пакет 
**qt5-default** и устанавливаются отдельными командами. Библиотека **PanoramaK** использует модули **core**, **network**, 
**websockets**, **serialport**, поэтому библиотеки фреймворка Qt/Qt6, реализующие функциональность этих модулей, 
должны лежать рядом с скомпилированной библиотекой **PanoramaK**.

Для ОС Windows и macOS рекомендуется устанавливать Qt5/Qt6 через онлайн инстолятор, рекомендуемая версия для сборки 
проекта **Panorama-K** Qt 5.12.0 и новее. 

### Скачать проект

```
git clone https://github.com/openratem/Panorama-K.git
```

### Сборка библиотеки PanoramaK

```
cd ./Panorama-K/library/PanoramaK
qmake ./PanoramaK.pro
make 
```

### Сборка программы PanoramaK_App

По умолчанию сборка выполняется с загрузкой динамической библиотеки во время запуска программы. Скомпилированная 
библиотека **PanoramaK** должна лежать рядом с исполняемым файлом программы PanoramaK_App перед её запуском.

```
cd ../../PanoramaK_App
qmake ./PanoramaK_App.pro
make 
```

Для запуска программы перейдите в директорию, в которой расположены скомпилированные файлы.

```
cd ../../build/linux64/release
./PanoramaK_App
```

Реализована возможность собрать программу в виде единого бинарного файла, а библиотека PanoramaK подключается к проекту 
в виде исходных текстов, для этого при сборке нужно указать аргумент для утилиты qmake **BUILD_MODE=builtin**, подключение 
библиотеки выполняется добавлением к проекту файла **PanoramaK_lib.pri** _(расположен в library/PanoramaK_lib)_.

```
qmake BUILD_MODE=builtin ./PanoramaK_App.pro
make
```

Директория, в которой будут находится скомпилированные файлы, расположена в корневой папке проекта и называется **build**. Внутри папки **build**
перед сборкой создаются необходимые каталоги, для каждой ОС создаются свои каталоги, например для ОС **Linux** и сборки 
release расположение скомпилимрованных файлов будет следующим: **./build/linux64/release**.
Посмотреть все вараинты каталогов для разных ОС и вариантов сборки **debug** или **release** можно в файле **./library/PanoramaK/config/path/path.pri**.

Выполнить сборку можно в фирменной IDE от разработчиков фрэймворка [QtCreator](https://www.qt.io/product/development-tools), 
для этого нужно в IDE [открыть](https://doc.qt.io/qtcreator/creator-project-opening.html) файл проекта **PanoramaK.pro**, 
указать версию фреймворка и выполнить сборку. Рекомендуемая версия QtCreator 4.11 или новее, руководство пользователя для 
IDE QtCreator [доступно](https://doc.qt.io/qtcreator/index.html) в интернете.


### Сборка и запуск демонстрационной программы LibLoader

В папке **examples** находится демонстрационный проект, который показывает процесс загрузки и использования скомпилированной библиотеки **PanoramaK**.
Чтобы скомпилировать проект  **LibLoader** нужно перейти в директорию проекта. Находясь в корневом каталоге проекта **Panorama-K**:

```
cd ./examples/LibLoader
qmake ./LibLoader.pro
make 
```
Скомпилированная программа **LibLoader** будет находится в той-же папке, что и библиотека **PanoramaK**. Для запуска 
программы нужно перейти в каталог с программой.
Пример запуска для ОС **Linux** и варианта **release**:

```
cd ../../build/linux64/release
./LibLoader --path=./libPanoramaK.so
```

После запуска программа выведет в консоль список поддерживаемых трансиверов. Для ОС windows потребуется дополнительный шаг, 
перед запуском нужно скопировать необходимые для запуска библиотеки фреймворка Qt5/Qt6 в папку с программой, например, 
если сборка проекта выполняется фреймворком Qt5 для варианта **release**, потребуются следующие :
 - Qt5Core.dll.
 - Qt5SerialPort.dll
 - Qt5Network.dll
 - Qt5WebSockets.dll

```
Support devices:
 -  SunSDR TCI
 -  Elecraft K2
 -  Elecraft K3
 -  Elecraft KX3
 -  FT-1000
 -  FT-1000 MP
 -  FT-100 D
 -  FT-2000
 -  FT-450
 -  FT-747
 -  FT-817
 -  FT-840
 -  FT-847
 -  FT-857
 -  FT-897
 -  FT-900
 -  FT-9000v1
 -  FT-920
 -  FT-950
 -  FT-990
 -  FT-991
 -  FTdx3000
 -  FTDX-5000MP
 -  IC-7000
 -  IC-7100
 -  IC-7200
 -  IC-7300
 -  IC-7410
 -  IC-746
 -  IC-746 Pro
 -  IC-756
 -  IC-756 Pro
 -  IC-756 Pro II
 -  IC-756 Pro III
 -  IC-7600
 -  IC-7600v2
 -  IC-7700
 -  IC-7700v2
 -  IC-7800
 -  IC-7800v3
 -  IC-7850
 -  IC-7851
 -  IC-9100
 -  IC-9100v2
 -  TenTec Eagle
 -  TenTec Orion
 -  TenTec Paragon II
 -  TS-2000
 -  TS-440
 -  TS-480
 -  TS-590
 -  TS-690
 -  TS-870
```

## 📝 Лицензия

[MIT License](LICENSE).
