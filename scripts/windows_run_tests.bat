
@echo off

rem
rem file:			windows_prepare_host.bat
rem path:			scripts/windows_prepare_host.bat
rem created on:		2023 Feb 03
rem created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
rem

setlocal EnableDelayedExpansion

set scriptDirectory=%~dp0
cd /D "%scriptDirectory%.."
set "repositoryRoot=%cd%\"

echo "repositoryRoot is %repositoryRoot%"

cd "%repositoryRoot%sys\win_x64\Debug\test"
if not "!ERRORLEVEL!"=="0" (exit /b !ERRORLEVEL!)

.\cpputils_unit_test_mkfl.exe
if not "!ERRORLEVEL!"=="0" (exit /b !ERRORLEVEL!)

endlocal
