#!/bin/bash

# https://intoli.com/blog/exit-on-errors-in-bash-scripts/
# exit when any command fails
set -e
# keep track of the last executed command
trap 'last_command=$current_command; current_command=$BASH_COMMAND' DEBUG

#scriptFileFullPath=`readlink -f ${0}`
#scriptDirectory=`dirname ${scriptFileFullPath}`
#cd ${scriptDirectory}/../..
#repositoryRoot=`pwd`
# in mac os above short calculation will not work
# also realpath utilit is missing in mac
scriptDirectory=`dirname "${0}"`
scriptFileName=`basename "${0}"`
cd "${scriptDirectory}"
fileOrigin=`readlink "${scriptFileName}"` || :
while [ ! -z "${fileOrigin}" ]
do
	scriptDirectory=`dirname "${fileOrigin}"`
	scriptFileName=`basename "${fileOrigin}"`
	cd "${scriptDirectory}"
	fileOrigin=`readlink "${scriptFileName}"`  || :
done
scriptDirectory=`pwd`

source ${scriptDirectory}/unix_source_per_session.sh ${scriptDirectory}/unix_source_per_session.sh ${scriptDirectory}/unix_source_per_session.sh

cd ${cpputilsSocketsRepoRoot}/workspaces/cpputils_sockets_all_mkfl
unset CPPUTILS_DEBUG
make -f cpputils_sockets_all.unix.Makefile CPPUTILS_RELEASE=1
unset CPPUTILS_RELEASE
make -f cpputils_sockets_all.unix.Makefile CPPUTILS_DEBUG=1
