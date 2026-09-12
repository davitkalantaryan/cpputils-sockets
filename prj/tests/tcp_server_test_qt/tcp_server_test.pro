#
# file:			any_quick_test.pro
# path:			prj/tests/any_quick_test_qt/any_quick_test.pro
# created on:	2021 Mar 07
# created by:	Davit Kalantaryan
#

message("!!! $${_PRO_FILE_}")
include ( "$${PWD}/../../common/common_qt/flagsandsys_common.pri" )
DESTDIR     = "$${artifactRoot}/sys/$${CODENAME}/$$CONFIGURATION/test"

QT -= gui
QT -= core
QT -= widgets
CONFIG -= qt
CONFIG += console

DEFINES += CPPSOCKETS_TCP_SERVER_EXTRA_LOGGING_NEEDED
DEFINES += CPPSOCKETS_TCP_SERVER_USE_CINTERNAL_LOGGER

win32{
	LIBS += -lWs2_32
} else {
	LIBS += -pthread
}


SOURCES += "$${cpputilsSocketsRepoRoot}/src/tests/main_tcp_server_test.cpp"
SOURCES += $$files($${cpputilsSocketsRepoRoot}/src/core/*.cpp,true)
SOURCES += "$${cinternalRepoRoot}/src/core/cinternal_core_logger.c"

HEADERS += $$files($${cpputilsSocketsRepoRoot}/src/core/*.hpp,true)
HEADERS += $$files($${cpputilsSocketsRepoRoot}/include/*.h,true)
HEADERS += $$files($${cpputilsSocketsRepoRoot}/include/*.hpp,true)

#OTHER_FILES += $$files($${PWD}/../any_quick_test_mkfl/*.Makefile,false)
