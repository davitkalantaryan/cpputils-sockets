

TEMPLATE = subdirs
#CONFIG += ordered

include ( "$${PWD}/../../prj/common/common_qt/flagsandsys_common_private.pri" )


SUBDIRS		+=	"$${cpputilsSocketsRepoRoot}/prj/tests/any_quick_test_qt/any_quick_test.pro"
SUBDIRS		+=	"$${cpputilsSocketsRepoRoot}/prj/tests/cppsockets_unit_test_mult/cppsockets_unit_test.pro"
SUBDIRS		+=	"$${cpputilsSocketsRepoRoot}/prj/tests/tcp_client_test_qt/tcp_client_test.pro"
SUBDIRS		+=	"$${cpputilsSocketsRepoRoot}/prj/tests/tcp_server_test_qt/tcp_server_test.pro"

cinternalFromHere{
        SUBDIRS	+= "$${cinternalRepoRoot}/workspaces/cinternal_all_qt/cinternal_all.pro"
}


OTHER_FILES += $$files($${cpputilsSocketsRepoRoot}/scripts/*.sh,true)
OTHER_FILES += $$files($${cpputilsSocketsRepoRoot}/scripts/*.bat,true)
OTHER_FILES += $$files($${cpputilsSocketsRepoRoot}/scripts/.cicd/*.sh,true)
OTHER_FILES += $$files($${cpputilsSocketsRepoRoot}/scripts/.cicd/*.bat,true)
OTHER_FILES += $$files($${cpputilsSocketsRepoRoot}/scripts/.raw/*.sh,true)
OTHER_FILES += $$files($${cpputilsSocketsRepoRoot}/scripts/.raw/*.bat,true)
OTHER_FILES += $$files($${cpputilsSocketsRepoRoot}/docs/*.md,true)
OTHER_FILES += $$files($${cpputilsSocketsRepoRoot}/docs/*.txt,true)
OTHER_FILES += $$files($${cpputilsSocketsRepoRoot}/.github/*.yml,true)
OTHER_FILES += $$files($${cpputilsSocketsRepoRoot}/prj/common/common_mkfl/*.Makefile)
OTHER_FILES += $$files($${PWD}/../cpputils_sockets_all_mkfl/*.Makefile)


OTHER_FILES	+=	\
        "$${cpputilsSocketsRepoRoot}/.gitattributes"						\
	"$${cpputilsSocketsRepoRoot}/.gitignore"						\
	"$${cpputilsSocketsRepoRoot}/.gitmodules"						\
	"$${cpputilsSocketsRepoRoot}/LICENSE"							\
	"$${cpputilsSocketsRepoRoot}/README.md"
