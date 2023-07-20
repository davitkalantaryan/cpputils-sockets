#
# repo:         cpputils sockets
# name:			sys_common.pri
# path:			${repositoryRoot}/prj/common/common_qt/sys_common.pri
# created on:		2022 Jan 16
# created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)  
# usage:		Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/sys_common.pri")
cpputilsSocketsSysCommonIncluded = 1

isEmpty( cpputilsSocketsResolveCommonIncluded ) {
        include("$${PWD}/resolve_common.pri")
        cpputilsSocketsResolveCommonIncluded = 1
}

isEmpty( cpputilsSysCommonIncluded ) {
        include ( "$${cpputilsRepoRoot}/prj/common/common_qt/sys_common.pri" )
        cpputilsSysCommonIncluded = 1
}
