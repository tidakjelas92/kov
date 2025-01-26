:: inspired from raddbg's build.bat
@echo off
setlocal enabledelayedexpansion
cd /D "%~dp0"

:: Unpack arguments
for %%a in (%*) do set "%%a=1"
if "%~1"=="" (
	echo info: defaulted to building 'main'
	set main=1
)
if "%~1"=="release" if "%~2"=="" (
	echo info: defaulted to building 'main'
	set release=1
	set main=1
)

:: Compile and link definitions
set cc=cl
set common_cflags=/Wall
set cflags=
set iflags=/Isrc /Ivendor\raylib\src /Ivendor\raygui\src
set lflags=gdi32.lib msvcrt.lib raylib.lib winmm.lib kernel32.lib user32.lib shell32.lib /link /LIBPATH:vendor\raylib\src /NODEFAULTLIB:libcmt

if not exist out mkdir out

if "%main%"=="1" (
	echo info: building target main.
	set target_exists=1

	if "%release%"=="1" (
		echo info: release build mode
		set cflags=%common_cflags% /O2
	) else (
		echo info: debug build mode
		set cflags=%common_cflags%
	)

	echo info: used flags !cflags! %iflags% %lflags%
	%cc% build.c !cflags! %iflags% %lflags% /OUT:out\kov.exe
	if !errorlevel!==0 (
		echo info: main target built successfully.
    xcopy assets out\assets /e /s /y
	) else (
		exit /b 1
	)
)

:: Warn on no target
if "%target_exists%"=="" (
	echo warning: no target was specified.
	exit /b 1
)

endlocal
