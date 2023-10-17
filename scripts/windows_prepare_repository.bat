
@echo off

rem
rem file:			windows_prepare_repository.bat
rem path:			scripts/windows_prepare_repository.bat
rem created on:		2023 Feb 03
rem created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
rem

setlocal EnableDelayedExpansion

set scriptDirectory=%~dp0
cd /D "%scriptDirectory%.."
set "repositoryRoot=%cd%\"

git submodule sync --recursive
git submodule update --init --recursive


endlocal
