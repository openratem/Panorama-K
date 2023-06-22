# Panorama-K

Проект Panorama-K это проект библиотеки с открытым исходным кодом для анализа состояния антенно-фидерного устройства 
коротковолновой радиостанции.

На данный момент библиотека реализована в виде набора исходных текстов, которые подключаются к основному проекту. 
В качестве такого проекта написана демонстрационная программа DemoPanoramaK, в которой реализован графический 
пользовательский интерфейс для удобного взаимодейтсвия с пользователем.

Проект разрабатывается на языке С++17 и использует кроссплатформенный [фрэймворк Qt5/Qt6](https://www.qt.io/).

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
   Icom IC-7000.ini
   Icom IC-7100.ini
   Icom IC-7200.ini
   Icom IC-7300.ini
   Icom IC-7410.ini
   Icom IC-746.ini
   Icom IC-746 Pro.ini
   Icom IC-756.ini
   Icom IC-756 Pro.ini
   Icom IC-756 Pro II.ini
   Icom IC-756 Pro III.ini
   Icom IC-7600.ini
   Icom IC-7600v2.ini
   Icom IC-7700.ini
   Icom IC-7700v2.ini
   Icom IC-7800.ini
   Icom IC-7800v3.ini
   Icom IC-7850.ini
   Icom IC-7851.ini
   Icom IC-9100.ini
   Icom IC-9100v2.ini
   TenTec Eagle.ini
   TenTec Orion.ini
   TenTec Paragon II.ini
   Kenwood TS-2000.ini
   Kenwood TS-440.ini
   Kenwood TS-480.ini
   Kenwood TS-590.ini
   Kenwood TS-690.ini
   KenwoodTS-870.ini
   KenwoodTS-930.ini
      ```
3. [ ] Разработка программной библиотеки под ОС Windows и Linux, MacOS
4. [x] Разработка графической оболочки для библиотеки в ОС Linux, Windows, MacOS (минимальная функциональность)
5. [ ] Тестирование программного обеспечения в реальных условиях

## Как собрать проект

Для сборки проекта необходимо иметь установленные инструменты разарботки для языка С++ и фрэймворк Qt5/Qt6, а также 
необходимо установить систему контроля версий git.

### Скачать проект

```
git clone https://github.com/openratem/Panorama-K.git
```

### Сборка программы DemoPanoramaK

```
cd ./Panorama-K/DemoPanoramaK
qmake ./DemoPanoramaK.pro
make 
```

Выполнить сборку можно в фирменной IDE от разработчиков фрэймворка QtCreator, для этого нужно в IDE открыть файл проекта
DemoPanoramaK.pro.

