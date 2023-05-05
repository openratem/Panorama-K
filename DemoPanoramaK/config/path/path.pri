# дирректория куда помещается бинарный файл
macx {
    # Apple OS X
    CONFIG(release, debug|release): DST_PATH = bin/macx   # конечная версия программы
    CONFIG(debug  , debug|release): DST_PATH = bin/macx_d # отладочная версия программы
}
linux {
    # Linux
    equals(QT_ARCH, arm) {
        CONFIG(release, debug|release): DST_PATH = bin/linuxARM   # конечная версия программы
        CONFIG(debug  , debug|release): DST_PATH = bin/linuxARM_d # отладочная версия программы
    }
    else {
        contains(QT_ARCH, i386) {
            # 32 bit
            CONFIG(release, debug|release): DST_PATH = bin/linux32   # конечная версия программы
            CONFIG(debug  , debug|release): DST_PATH = bin/linux32_d # отладочная версия программы
        }
        else {
            # 64 bit
            CONFIG(release, debug|release): DST_PATH = bin/linux64   # конечная версия программы
            CONFIG(debug  , debug|release): DST_PATH = bin/linux64_d # отладочная версия программы
        }
    }
}
windows {
    # Windows
    contains(QT_ARCH, i386) {
        # 32 bit
        win32-msvc* {
            # Visual Studio
            CONFIG(release, debug|release): DST_PATH = bin/win32_msvc    # конечная версия программы
            CONFIG(debug  , debug|release): DST_PATH = bin/win32_msvc_d  # отладочная версия программы
        }
        win32-g++ {
            # MinGW
            CONFIG(release, debug|release): DST_PATH = bin/win32_mingw   # конечная версия программы
            CONFIG(debug  , debug|release): DST_PATH = bin/win32_mingw_d # отладочная версия программы
        }
    }
    else {
        # 64 bit
        win32-msvc* {
            # Visual Studio
            CONFIG(release, debug|release): DST_PATH = bin/win64_msvc    # конечная версия программы
            CONFIG(debug  , debug|release): DST_PATH = bin/win64_msvc_d  # отладочная версия программы
        }
        win32-g++ {
            # MinGW
            CONFIG(release, debug|release): DST_PATH = bin/win64_mingw   # конечная версия программы
            CONFIG(debug  , debug|release): DST_PATH = bin/win64_mingw_d # отладочная версия программы
        }
    }
}
