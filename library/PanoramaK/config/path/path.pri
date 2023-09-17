# дирректория куда помещается бинарный файл
macx {
    # Apple OS X
    CONFIG(release, debug|release): DST_PATH = ../../build/macos/release   # конечная версия программы
    CONFIG(debug  , debug|release): DST_PATH = ../../build/macos/debug # отладочная версия программы
}
linux {
    # Linux
    equals(QT_ARCH, arm) {
        CONFIG(release, debug|release): DST_PATH = ../../build/linuxArm/release   # конечная версия программы
        CONFIG(debug  , debug|release): DST_PATH = ../../build/linuxArm/debug # отладочная версия программы
    }
    else {
        contains(QT_ARCH, i386) {
            # 32 bit
            CONFIG(release, debug|release): DST_PATH = ../../build/linux32/release   # конечная версия программы
            CONFIG(debug  , debug|release): DST_PATH = ../../build/linux32/debug # отладочная версия программы
        }
        else {
            # 64 bit
            CONFIG(release, debug|release): DST_PATH = ../../build/linux64/release   # конечная версия программы
            CONFIG(debug  , debug|release): DST_PATH = ../../build/linux64/debug # отладочная версия программы
        }
    }
}
windows {
    # Windows
    contains(QT_ARCH, i386) {
        # 32 bit
        win32-msvc* {
            # Visual Studio
            CONFIG(release, debug|release): DST_PATH = ../../build/win32_msvc/release    # конечная версия программы
            CONFIG(debug  , debug|release): DST_PATH = ../../build/win32_msvc/debug  # отладочная версия программы
        }
        win32-g++ {
            # MinGW
            CONFIG(release, debug|release): DST_PATH = ../../build/win32_mingw/release   # конечная версия программы
            CONFIG(debug  , debug|release): DST_PATH = ../../build/win32_mingw/debug # отладочная версия программы
        }
    }
    else {
        # 64 bit
        win32-msvc* {
            # Visual Studio
            CONFIG(release, debug|release): DST_PATH = ../../build/win64_msvc/release    # конечная версия программы
            CONFIG(debug  , debug|release): DST_PATH = ../../build/win64_msvc/debug  # отладочная версия программы
        }
        win32-g++ {
            # MinGW
            CONFIG(release, debug|release): DST_PATH = ../../build/win64_mingw/release   # конечная версия программы
            CONFIG(debug  , debug|release): DST_PATH = ../../build/win64_mingw/debug # отладочная версия программы
        }
    }
}
