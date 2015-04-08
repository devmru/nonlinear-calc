#-------------------------------------------------
#
# Project created by QtCreator 2015-04-05T01:30:33
#
#-------------------------------------------------

QT       += core gui
QT       += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = nonlinear-calc
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    muParser/muParser.cpp \
    muParser/muParserBase.cpp \
    muParser/muParserBytecode.cpp \
    muParser/muParserCallback.cpp \
    muParser/muParserComplex.cpp \
    muParser/muParserDLL.cpp \
    muParser/muParserError.cpp \
    muParser/muParserInt.cpp \
    muParser/muParserTest.cpp \
    muParser/muParserTokenReader.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    muParser/muParser.h \
    muParser/muParserBase.h \
    muParser/muParserBytecode.h \
    muParser/muParserCallback.h \
    muParser/muParserComplex.h \
    muParser/muParserDef.h \
    muParser/muParserDLL.h \
    muParser/muParserError.h \
    muParser/muParserFixes.h \
    muParser/muParserInt.h \
    muParser/muParserStack.h \
    muParser/muParserTest.h \
    muParser/muParserToken.h \
    muParser/muParserTokenReader.h

FORMS    += mainwindow.ui
