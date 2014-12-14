
QT += core gui widgets

TARGET = DragProxyWindow
TEMPLATE = app

SOURCES += DragProxy.cpp \
           DragProxyWindow.cpp \
           main.cpp
           
HEADERS += DragProxy.H \
           DragProxyWindow.H
           
FORMS += DragProxyWindow.ui

RESOURCES += DragProxyWindow.qrc