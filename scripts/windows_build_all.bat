::
:: file:			windows_build_all.bat
:: path:			scripts/windows_build_all.bat
:: created on:		2023 Feb 03
:: created by:		Davit Kalantaryan (davit.kalantaryan@desy.de)
::

@echo off
setlocal EnableDelayedExpansion

:: here we define default platformtarget to build, platforms are following
:: other platforms should be provided as first argument
:: ARM, ARM64, x86, x64
set PlatformTarget=ARM,ARM64,x86,x64
set Configuration=Debug,Release
set ActionConfirm=Build

set scriptName=%0
set scriptDirectory=%~dp0
cd /D "%scriptDirectory%.."
set "repositoryRoot=%cd%\"

if not defined PlatformToolsetVar (
	set "PlatformToolsetVarMid=%VCToolsVersion:~0,2%"
	set "PlatformToolsetVarEnd=%VCToolsVersion:~3,1%"
	set "PlatformToolsetVar=v%PlatformToolsetVarMid%%PlatformToolsetVarEnd%"

	rem todo: get rid of below code
	set "PlatformToolsetVar=v143"
	echo PlatformToolsetVar=%PlatformToolsetVar%
)


:: handling arguments
::set argC=0
::for %%x in (%*) do Set /A argC+=1
set nextArg=%scriptName%
for %%x in (%*) do (
	if /i "%%x"=="help" (
		call :call_help
		exit /b 0
	)
	set nextArg=%%x
	set possibleQuote=!nextArg:~0,1!
	set possibleQuote|find """" >nul && set quote_found=yes||set quote_found=no
	if "!quote_found:~0,3!"=="yes" ( 
		set nextArg=!nextArg:~1,-1! 
		echo fixed arg: !nextArg!
	)
	call :parse_argument
	if not "!ERRORLEVEL!"=="0" (exit /b %ERRORLEVEL%)
)

echo action=%ActionConfirm%,PlatformTarget=!PlatformTarget!,configuration=%Configuration%
cd "%repositoryRoot%prj\tests\cpputils_unit_test_mult"
if not "!ERRORLEVEL!"=="0" (exit /b !ERRORLEVEL!)

for %%p in (%PlatformTarget%) do (
	echo "!!!!!!!!!!!! platform %%p"
	for %%c in (%Configuration%) do (
		echo "!!!!!!!!!!!! !!!!!!!!!!!! compiling for configuration %%c"
		rem call msbuild "%repositoryRoot%workspaces\cpputils_all_vs\cpputils_all.sln" /t:!ActionConfirm! /p:Configuration=%%c /p:Platform=%%p
		call msbuild "%repositoryRoot%workspaces\cpputils_all_vs\cpputils_all.sln" /t:!ActionConfirm! /p:Configuration=%%c /p:Platform=%%p
		if not "!ERRORLEVEL!"=="0" (exit /b !ERRORLEVEL!)
		call nmake -f cpputils_unit_test.windows.Makefile /e Platform=%%p /e Configuration=%%c
		if not "!ERRORLEVEL!"=="0" (exit /b !ERRORLEVEL!)
	)
)

exit /b 0

:parse_argument
	set isNextArgPlatform=true
	if /i not "!nextArg:~0,3!"=="ARM" if /i not "!nextArg:0,5!"=="ARM64" if /i not "!nextArg:~0,3!"=="x86" if /i not "!nextArg:~0,3!"=="x64" (set isNextArgPlatform=false)
	if "!isNextArgPlatform!"=="true" (set PlatformTarget=!nextArg!) else (
		set isNextArgAction=true
		if /i not "!nextArg!"=="Build" if /i not "!nextArg!"=="Rebuild" if /i not "!nextArg!"=="Clean" (set isNextArgAction=false)
		if "!isNextArgAction!"=="true" (set ActionConfirm=!nextArg!) else (
			set isNextArgConfiguration=true
			rem if /i not "!nextArg!"=="Debug" if /i not "!nextArg!"=="Release" (set isNextArgConfiguration=false)
			if /i not "!nextArg:~0,5!"=="Debug" if /i not "!nextArg:~0,7!"=="Release" (set isNextArgConfiguration=false)
			if "!isNextArgConfiguration!"=="true" (set Configuration=!nextArg!) else (
				echo Unknown argument !nextArg!.
				call :call_help
				exit /b 1
			)
		)		
	)

	exit /b 0

:call_help
	echo Valid platforms are ARM, ARM64, x86 and x64. Using defalt x64.
	echo Valid configurations are Debug and Release. Using default Debug.
	echo Valid actions are Build, Rebuild and Clen. Using default Build.
	echo Call !scriptName! help to displaye help message
	exit /b 0

endlocal

