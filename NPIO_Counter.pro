QT += core
QT += widgets serialport



requires(qtConfig(combobox))

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = terminal
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    console.cpp \
    drawwidget_console.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h \
    console.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    NPIO_Counter.qrc


LIBS += -lws2_32

target.path = $$[QT_INSTALL_EXAMPLES]/Workspace/NPIO_Counter (1)
INSTALLS += target
