#
# repo:         cpputils sockets
# name:			sys_common.pri
# path:			${repositoryRoot}/prj/common/common_qt/sys_common.pri
# created on:		2022 Jan 16
# created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)  
# usage:		Use this qt include file to calculate some platform specific stuff
#


message("!!! $${PWD}/resolve_common.pri")
cpputilsSocketsResolveCommonIncluded = 1

isEmpty( cpputilsSocketsRepoRoot ) {
        cpputilsSocketsRepoRoot = $$(cpputilsSocketsRepoRoot)
        isEmpty(cpputilsSocketsRepoRoot) {
            cpputilsSocketsRepoRoot = $${PWD}/../../..
        }
}

isEmpty( repositoryRoot ) {
        repositoryRoot = $$(repositoryRoot)
        isEmpty(repositoryRoot) {
            repositoryRoot = $${cpputilsSocketsRepoRoot}
        }
}

isEmpty(artifactRoot) {
    artifactRoot = $$(artifactRoot)
    isEmpty(artifactRoot) {
        artifactRoot = $${repositoryRoot}
    }
}


isEmpty( cpputilsRepoRoot ) {
        cpputilsRepoRoot = $$(cpputilsRepoRoot)
        isEmpty(cpputilsRepoRoot) {
            cpputilsRepoRoot=$${cpputilsSocketsRepoRoot}/contrib/cpputils
        }
}
