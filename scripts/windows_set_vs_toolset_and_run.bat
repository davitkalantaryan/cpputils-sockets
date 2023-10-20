
@echo off

rem
rem repository:		cpputils
rem file:			windows_set_vs_toolset_and_run.bat
rem path:			scripts/windows_set_vs_toolset_and_run.bat
rem created on:		2023 May 18
rem created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
rem

setlocal EnableDelayedExpansion

set scriptDirectory=%~dp0
cd /D "%scriptDirectory%.."
set "repositoryRoot=%cd%\"

echo "repositoryRoot is %repositoryRoot%"

set "PlatformToolsetVarMid=%VCToolsVersion:~0,2%"
set "PlatformToolsetVarEnd=%VCToolsVersion:~3,1%"
set "PlatformToolsetVar=v%PlatformToolsetVarMid%%PlatformToolsetVarEnd%"
echo PlatformToolsetVar=%PlatformToolsetVar%


devenv workspaces\cpputils_all_vs\cpputils_all.sln

endlocal
