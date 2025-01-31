#!/bin/bash
# inspired by raddebugger's build script
# usage:
# 	./build.sh [build_target_1] [build_target_2] [...] [options]
#
# example:
# 	# defaults to 'main' if no target is specified
# 	./build.sh
#
# 	# builds 'main' target
# 	./build.sh main
#
# 	# build default target with 'release' config
# 	./build.sh release
#
# available build targets:
# 	- main
#
# options:
# 	- release: create release build, create debug build otherwise


# e for exit immediately after encountering non 0 exit status
set -e
# change to the script's directory
cd "$(dirname "$0")"

# parse build targets and set variables
for arg in "$@"; do declare $arg=1; done

if [[ $1 -eq 0 ]]; then
	echo "info: defaulted to target 'main'"
	main=1
elif [[ -v release ]] && [[ $2 -eq 0 ]]; then
	echo "info: defaulted to target 'main'"
	main=1
fi

if [ -v raylib ]; then
	echo "info: building raylib."
	target_exists=1

	mkdir -p tmp
	cd tmp

	RAYLIB_SRC="../vendor/raylib/src"
	COMMON_CFLAGS="-D_DEFAULT_SOURCE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33 -D_GLFW_X11 -std=c99"
	IFLAGS="-I$RAYLIB_SRC -I$RAYLIB_SRC/external/glfw/include"
	RAYLIB_C_FILES="$RAYLIB_SRC/rcore.c $RAYLIB_SRC/rshapes.c $RAYLIB_SRC/rtextures.c $RAYLIB_SRC/rtext.c $RAYLIB_SRC/rmodels.c $RAYLIB_SRC/utils.c $RAYLIB_SRC/raudio.c $RAYLIB_SRC/rglfw.c"

	if [ -v release ]; then
		cflags="$COMMON_CFLAGS -Os -flto"
		echo "info: release build"
	else
		cflags="$COMMON_CFLAGS -O0 -g -Wall -Wextra -Wpedantic"
		echo "info: debug build"
	fi

	echo "info: used flags $cflags $IFLAGS"
	clang -c $RAYLIB_C_FILES $cflags $IFLAGS
	if [ $? -eq 0 ]; then
		echo "info: raylib target built successfully."
	else
		exit 1
	fi

	cd ..
fi

# build targets
if [ -v main ]; then
	echo "info: building target main."
	target_exists=1

	if ! [ -d tmp ]; then
		echo "error: no tmp directory detected, please rebuild raylib."
		exit 1
	fi

	COMMON_CFLAGS="-Wall -Wextra -Wpedantic"
	COMMON_LFLAGS="-lm"
	IFLAGS="-Isrc -Ivendor/raylib/src"
	RAYLIB_O_FILES="tmp/rcore.o tmp/rshapes.o tmp/rtextures.o tmp/rtext.o tmp/rmodels.o tmp/utils.o tmp/raudio.o tmp/rglfw.o"

	if [ -v release ]; then
		cflags="$COMMON_CFLAGS -Os -ffast-math -flto -DNDEBUG -Wl,-s"
		lflags="$COMMON_LFLAGS -flto"
		echo "info: release build"
	else
		cflags="$COMMON_CFLAGS -g"
		lflags="$COMMON_LFLAGS"
		echo "info: debug build"
	fi

	mkdir -p out

	echo "info: used flags $cflags $lflags $IFLAGS"
	clang build.c $RAYLIB_O_FILES $cflags $lflags $IFLAGS -o out/kov
	if [ $? -eq 0 ]; then
		echo "info: main target built successfully."
		rsync -r assets out
		echo "info: synced assets directory."
	else
		exit 1
	fi
fi

# finally, warn if no build happened.
if [ ! -v target_exists ]; then
	echo "error: no valid build target specified."
	exit 1
fi
