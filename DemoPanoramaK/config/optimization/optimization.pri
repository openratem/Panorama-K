windows {
    win32-msvc* {
        QMAKE_CXXFLAGS         += /MP
        QMAKE_CXXFLAGS_RELEASE += /Ox /Qpar /GL /Ob2 /Oi /Ot /GT
        QMAKE_LFLAGS_RELEASE   += /LTCG /OPT:REF /OPT:ICF
    }
}
linux {
    # Clang
    QMAKE_CXXFLAGS_RELEASE += -Ofast
    QMAKE_LFLAGS_RELEASE += -flto

}
macx {
    QMAKE_CXXFLAGS_RELEASE += -O3
    QMAKE_LFLAGS_RELEASE += -flto
}

# включаем большое количество предупреждений компилятора
macx {
    QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -Werror=return-type
}
linux {
    QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic -Werror=return-type
}
win32-g++ {
    QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -pedantic-errors -Werror=return-type
}
win32-msvc* {
#    QMAKE_CXXFLAGS += /W4 # Высокий уровень вывода предупреждений
}
equals(QMAKE_CXX, clang++) {
    QMAKE_CXXFLAGS += -Wall -Wextra -pedantic # -Weverything
}
