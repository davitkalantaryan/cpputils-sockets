#
# repo:		    cppsockets
# file:		    flagsandsys_common_private_pure.windows.Makefile
# created on:	    2020 Dec 14
# created by:	    Davit Kalantaryan (davit.kalantaryan@desy.de)
# purpose:	    This file can be only as include
#

!IFNDEF cpputilsSocketsRepoRoot
cpputilsSocketsRepoRoot	= $(MAKEDIR)\..\..\..\..
!ENDIF
!include <$(cpputilsSocketsRepoRoot)\prj\common\common_mkfl\flagsandsys_common.windows.Makefile>

CFLAGS				= $(CFLAGS) /I"$(cpputilsSocketsRepoRoot)\src\include"
