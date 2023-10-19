

targetName=cppsockets_all

mkfile_path		=  $(abspath $(lastword $(MAKEFILE_LIST)))
mkfile_dir		=  $(shell dirname $(mkfile_path))

ifndef cpputilsRepoRoot
        cpputilsRepoRoot	:= $(shell curDir=`pwd` && cd $(mkfile_dir)/../.. && pwd && cd ${curDir})
endif


all:
	$(MAKE) -f $(cpputilsRepoRoot)/prj/tests/cpputils_unit_test_mult/cpputils_unit_test.unix.Makefile			&& \
	$(MAKE) -f $(cpputilsRepoRoot)/prj/tests/any_quick_test_mkfl/any_quick_test.unix.Makefile

.PHONY: clean
clean:
	$(MAKE) -f $(cpputilsRepoRoot)/prj/tests/cpputils_unit_test_mult/cpputils_unit_test.unix.Makefile clean
	$(MAKE) -f $(cpputilsRepoRoot)/prj/tests/any_quick_test_mkfl/any_quick_test.unix.Makefile clean
	
