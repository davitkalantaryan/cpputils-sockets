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

isEmpty(CPPSOCKETS_TCP_SERVER_NOT_USE_CINTERNAL_LOGGER) {
    CPPSOCKETS_TCP_SERVER_NOT_USE_CINTERNAL_LOGGER = $$(CPPSOCKETS_TCP_SERVER_NOT_USE_CINTERNAL_LOGGER)
    isEmpty(CPPSOCKETS_TCP_SERVER_NOT_USE_CINTERNAL_LOGGER) {
        message("-- CPPSOCKETS_TCP_SERVER_NOT_USE_CINTERNAL_LOGGER is not defined")
        DEFINES += CPPSOCKETS_TCP_SERVER_USE_CINTERNAL_LOGGER
    } else {
        message("++ CPPSOCKETS_TCP_SERVER_NOT_USE_CINTERNAL_LOGGER is defined")
    }
} else {
    message("++ CPPSOCKETS_TCP_SERVER_NOT_USE_CINTERNAL_LOGGER is defined")
}

isEmpty(CPPSOCKETS_TCP_SERVER_EXTRA_LOGGING_NEEDED) {
    CPPSOCKETS_TCP_SERVER_EXTRA_LOGGING_NEEDED = $$(CPPSOCKETS_TCP_SERVER_EXTRA_LOGGING_NEEDED)
    isEmpty(CPPSOCKETS_TCP_SERVER_EXTRA_LOGGING_NEEDED) {
        message("-- CPPSOCKETS_TCP_SERVER_EXTRA_LOGGING_NEEDED is not defined")
    } else {
        DEFINES += CPPSOCKETS_TCP_SERVER_EXTRA_LOGGING_NEEDED
        message("++ CPPSOCKETS_TCP_SERVER_EXTRA_LOGGING_NEEDED is defined")
    }
} else {
    DEFINES += CPPSOCKETS_TCP_SERVER_EXTRA_LOGGING_NEEDED
    message("++ CPPSOCKETS_TCP_SERVER_EXTRA_LOGGING_NEEDED is defined")
}

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
