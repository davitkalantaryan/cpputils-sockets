#
# repo:		    cppsockets
# file:		    flagsandsys_common_pure.unix.Makefile
# created on:	    2020 Dec 14
# created by:	    Davit Kalantaryan (davit.kalantaryan@desy.de)
# purpose:	    This file can be only as include
#

ifndef cpputilsSocketsRepoRoot
        mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
        mkfile_dir		=  $(shell dirname $(mkfile_path))
        cpputilsSocketsRepoRoot	:= $(shell curDir=`pwd` && cd $(mkfile_dir)/../../../.. && pwd && cd ${curDir})
endif

ifndef artifactRoot
        artifactRoot	= $(cpputilsSocketsRepoRoot)
endif

include $(cpputilsSocketsRepoRoot)/contrib/cinternal/prj/common/common_mkfl/flagsandsys_common.unix.Makefile

COMMON_FLAGS	+= -I$(cpputilsSocketsRepoRoot)/include

LIBS	+= -L$(cpputilsSocketsRepoRoot)/sys/$(lsbCode)/$(Configuration)/lib
LIBS	+= -L$(cpputilsSocketsRepoRoot)/sys/$(lsbCode)/$(Configuration)/tlib
