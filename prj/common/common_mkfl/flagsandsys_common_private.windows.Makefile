#
# repo:		cppsockets
# file:		flagsandsys_common_private.windows.Makefile
# created on:	2020 Dec 14
# created by:	Davit Kalantaryan (davit.kalantaryan@desy.de)
# purpose:	This file can be only as include
#

!IFNDEF cpputilsSocketsPrivateFlagsAndSysCommonIncluded
cpputilsSocketsPrivateFlagsAndSysCommonIncluded		= 1
!IFNDEF cpputilsSocketsRepoRoot
cpputilsSocketsRepoRoot	= $(MAKEDIR)\..\..\..
!ENDIF
!include <$(cpputilsSocketsRepoRoot)\prj\common\common_mkfl\raw\flagsandsys_common_private_raw.windows.Makefile>
!ENDIF
