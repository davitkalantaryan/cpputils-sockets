

TEMPLATE = subdirs
#CONFIG += ordered

include ( "$${PWD}/../../prj/common/common_qt/flagsandsys_common_private.pri" )


SUBDIRS		+=	"$${cpputilsRepoRoot}/prj/tests/any_quick_test_qt/any_quick_test.pro"
SUBDIRS		+=	"$${cpputilsRepoRoot}/prj/tests/cpputils_unit_test_mult/cpputils_unit_test.pro"

cinternalFromHere{
        SUBDIRS	+= "$${cinternalRepoRoot}/workspaces/cinternal_all_qt/cinternal_all.pro"
}


OTHER_FILES += $$files($${cpputilsRepoRoot}/scripts/*.sh,true)
OTHER_FILES += $$files($${cpputilsRepoRoot}/scripts/*.bat,true)
OTHER_FILES += $$files($${cpputilsRepoRoot}/scripts/.cicd/*.sh,true)
OTHER_FILES += $$files($${cpputilsRepoRoot}/scripts/.cicd/*.bat,true)
OTHER_FILES += $$files($${cpputilsRepoRoot}/scripts/.raw/*.sh,true)
OTHER_FILES += $$files($${cpputilsRepoRoot}/scripts/.raw/*.bat,true)
OTHER_FILES += $$files($${cpputilsRepoRoot}/docs/*.md,true)
OTHER_FILES += $$files($${cpputilsRepoRoot}/docs/*.txt,true)
OTHER_FILES += $$files($${cpputilsRepoRoot}/.github/*.yml,true)
OTHER_FILES += $$files($${cpputilsRepoRoot}/prj/common/common_mkfl/*.Makefile)


OTHER_FILES	+=	\
        "$${cpputilsRepoRoot}/.gitattributes"						\
	"$${cpputilsRepoRoot}/.gitignore"						\
	"$${cpputilsRepoRoot}/.gitmodules"						\
	"$${cpputilsRepoRoot}/LICENSE"							\
	"$${cpputilsRepoRoot}/README.md"
