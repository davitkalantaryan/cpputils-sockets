#
# repo:				cpputils
# file:				cpputils_unit_test.windows.Makefile
# created on:	    2020 Dec 02
# created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
#


MakeFileName            = cpputils_unit_test.windows.Makefile
MakeFileDir				= $(MAKEDIR)
TargetName				= cpputils_unit_test_mkfl
TargetExtension			= exe
TargetCategory          = test
SrcBaseDir				= $(MakeFileDir)\..\..\..\src

DEFINES       			= /D "CINTERNAL_UNIT_TEST_USE_GTEST_LIKE_MACROSES"
LIBS					= $(LIBS) "Ws2_32.lib"
LFLAGS					= $(LFLAGS) /SUBSYSTEM:CONSOLE

Objects					=	..\contrib\cinternal\src\core\cinternal_core_unit_test_checks.$(Platform)_$(ObjectsExtension)_obj
Objects					=	$(Objects) ..\contrib\cinternal\src\core\cinternal_core_unit_test_main.$(Platform)_$(ObjectsExtension)_obj
Objects					=	$(Objects) ..\contrib\cinternal\src\core\cinternal_core_logger.$(Platform)_$(ObjectsExtension)_obj


#SourcesToCompile	=
#DirectoriesToCompile	= $(DirectoriesToCompile) nr-build\gen\cpp\sss\ssslang\antlr
DirectoriesToCompile	=
DirectoriesToCompile	= tests\unit_test
DirectoriesToCompile	= $(DirectoriesToCompile) core

default: unittest

unittest: __preparationForSetObjects __setObjects

!include <$(MakeFileDir)\..\..\common\common_mkfl\flagsandsys_common_private.windows.Makefile>
