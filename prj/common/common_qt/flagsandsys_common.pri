#
# repo:			cppsockets
# name:			flagsandsys_common.pri
# path:			prj/common/common_qt/flagsandsys_common.pri
# created on:   2023 Jun 21
# created by:   Davit Kalantaryan (davit.kalantaryan@desy.de)
# usage:		Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/flagsandsys_common.pri")

isEmpty(cpputilsSocketsFlagsAndSysCommonIncluded){
    cpputilsSocketsFlagsAndSysCommonIncluded = 1

    cpputilsSocketsRepoRoot = $${PWD}/../../..

    isEmpty(artifactRoot) {
        artifactRoot = $$(artifactRoot)
	        isEmpty(artifactRoot) {
		        artifactRoot = $${cpputilsSocketsRepoRoot}
		}
    }

    include("$${cpputilsSocketsRepoRoot}/contrib/cinternal/prj/common/common_qt/flagsandsys_common.pri")

    INCLUDEPATH += $${cpputilsSocketsRepoRoot}/include

    LIBS	+= -L$${cpputilsSocketsRepoRoot}/sys/$${CODENAME}/$$CONFIGURATION/lib
    LIBS	+= -L$${cpputilsSocketsRepoRoot}/sys/$${CODENAME}/$$CONFIGURATION/tlib

    OTHER_FILES += $$files($${PWD}/../common_mkfl/*.Makefile,true)
}
