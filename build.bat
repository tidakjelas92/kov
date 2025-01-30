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

if "%raylib%"=="1" (
	where cl >nul 2>nul
	if NOT %errorlevel% == 0 (
		echo cl is not found in path, please run vcvarsall.bat manually or use devenv.bat located in this directory.
		exit /b
	)

	echo info: building target raylib.
	set target_exists=1

	if not exist tmp mkdir tmp
	cd tmp

	set raylib_src=..\vendor\raylib\src
	set sources=!raylib_src!\rcore.c !raylib_src!\rshapes.c !raylib_src!\rtextures.c !raylib_src!\rtext.c !raylib_src!\rmodels.c !raylib_src!\utils.c !raylib_src!\raudio.c !raylib_src!\rglfw.c
	set common_cflags=/D_DEFAULT_SOURCE /DPLATFORM_DESKTOP /DGRAPHICS_API_OPENGL_33 /utf-8 /validate-charset /EHsc /std:c11
	set iflags=/I!raylib_src! /I!raylib_src!\external\glfw\include

	if "%release%"=="1" (
		echo info: release build mode
		set cflags=!common_cflags! /O1 /GL /favor:blend
	) else (
		echo info: debug build mode
		set cflags=!common_cflags! /Od /Zi
	)

	REM turning off all warnings because there's a ton of them :shrug:
	cl /w /c !sources! !iflags! !cflags!
	if !errorlevel!==0 (
		echo info: raylib target built successfully.
	) else (
		exit /b 1
	)

	cd ..
)

if "%main%"=="1" (
	where cl >nul 2>nul
	if NOT %errorlevel% == 0 (
		echo cl is not found in path, please run vcvarsall.bat manually or use devenv.bat located in this directory.
		exit /b
	)

	if not exist tmp echo "no tmp folder detected, please rebuild raylib"

	if not exist out mkdir out

	cd out

	REM Compile and link definitions
	set common_cflags=/Wall
	set iflags=/I..\src /I..\vendor\raylib\src /I..\vendor\raygui\src
	set common_lflags=/link gdi32.lib msvcrt.lib winmm.lib kernel32.lib user32.lib shell32.lib opengl32.lib ..\tmp\raudio.obj ..\tmp\rcore.obj ..\tmp\rglfw.obj ..\tmp\rmodels.obj ..\tmp\rshapes.obj ..\tmp\rtext.obj ..\tmp\rtextures.obj ..\tmp\utils.obj /NODEFAULTLIB:libcmt /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup

	echo info: building target main.
	set target_exists=1

	if "%release%"=="1" (
		echo info: release build mode
		set cflags=%common_cflags% /O1 /GL /DNDEBUG
		set lflags=!common_lflags! /LTCG
	) else (
		echo info: debug build mode
		set cflags=%common_cflags% /Od /Zi
		set lflags=!common_lflags! /DEBUG
	)

	echo info: used flags !cflags! !iflags! !lflags!
	cl ..\build.c !cflags! !iflags! !lflags! /out:kov.exe
	if !errorlevel!==0 (
		echo info: main target built successfully.
		xcopy ..\assets ..\out\assets /e /s /y
		del build.obj
	) else (
		exit /b 1
	)

	cd ..
)

:: Warn on no target
if "%target_exists%"=="" (
	echo warning: no target was specified.
	exit /b 1
)

endlocal
