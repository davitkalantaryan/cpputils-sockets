#
# repo:		cppsockets
# file:		flagsandsys_common_pure.windows.Makefile
# created on:	2020 Dec 14
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
# purpose:	This file can be only as include
#

!IFNDEF MakeFileDir
MakeFileDir			= $(MAKEDIR)\..
!ENDIF

!IFNDEF cpputilsSocketsRepoRoot
cpputilsSocketsRepoRoot	= $(MakeFileDir)\..\..\..
!ENDIF

!IFNDEF artifactRoot
artifactRoot	= $(cpputilsSocketsRepoRoot)
!ENDIF

!IFNDEF cinternalRepoRoot
cinternalRepoRoot	= $(cpputilsSocketsRepoRoot)\contrib\cinternal
!ENDIF

!include <$(cinternalRepoRoot)\prj\common\common_mkfl\flagsandsys_common.windows.Makefile>

CFLAGS				= $(CFLAGS) /I"$(cpputilsSocketsRepoRoot)\include"

LFLAGS				= $(LFLAGS) /LIBPATH:"$(cpputilsSocketsRepoRoot)\sys\win_$(Platform)\$(Configuration)\lib"
LFLAGS				= $(LFLAGS) /LIBPATH:"$(cpputilsSocketsRepoRoot)\sys\win_$(Platform)\$(Configuration)\tlib"
