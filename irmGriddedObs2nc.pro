TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS = -Wno-c++11-narrowing

LIBS += -lboost_system -lboost_filesystem -lboost_program_options -lboost_iostreams -lnetcdf_c++

# pour date
INCLUDEPATH += $$PWD/date/include/date/
DEPENDPATH += $$PWD/date/include/date/

TARGET = irm2nc

SOURCES += \
    irm.cpp \
    main.cpp\

HEADERS+=date.h\ \
    irm.h
