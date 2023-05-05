# местоположение временных файлов компиляции
MOC_DIR     = .tmp
OBJECTS_DIR = .tmp
RCC_DIR     = .tmp
UI_DIR      = .tmp

# включаем стандарт С++17
macx {
    CONFIG += c++17
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 11.0
}
linux {
    CONFIG += c++17
}
win32-g++ {
    CONFIG += c++17
}
win32-msvc* {
    CONFIG += c++17
    CONFIG += /std:c++17
}
