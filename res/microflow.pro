QT += quick qml

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

################

INCLUDEPATH += \
        $$PWD/../inc/ \
        $$PWD/../src/ \
        $$PWD/../res/
        
        

HEADERS += \

SOURCES += \
        ../src/main.cpp \

RESOURCES += \
        ../res/qml.qrc \
        qml.qrc



LIBS +=

################

QML_IMPORT_PATH =

QML_DESIGNER_IMPORT_PATH =

################

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

