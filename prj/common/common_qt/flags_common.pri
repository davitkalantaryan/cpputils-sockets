#
# repo:         cpputils sockets
# name:			flags_common.pri
# path:			${repositoryRoot}/prj/common/common_qt/flags_common.pri
# created on:		2022 Oct 19
# created by:		Davit Kalantaryan (davit.kalantaryan@gmail.com)
# usage:		Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/flags_common.pri")
cpputilsSocketsFlagsCommonIncluded = 1

isEmpty( cpputilsSocketsResolveCommonIncluded ) {
        include("$${PWD}/resolve_common.pri")
        cpputilsSocketsResolveCommonIncluded = 1
}

isEmpty( cpputilsFlagsCommonIncluded ) {
        include ( "$${cpputilsRepoRoot}/prj/common/common_qt/flags_common.pri" )
        cpputilsFlagsCommonIncluded = 1
}

INCLUDEPATH += $${cpputilsSocketsRepoRoot}/include
