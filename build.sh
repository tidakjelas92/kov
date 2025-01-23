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

# compile and link arguments
CC="clang"
COMMON_CFLAGS="-Wall -Wextra"
DEBUG_CFLAGS="-g"
RELEASE_CFLAGS="-O3 -ffast-math -DNDEBUG -Wl,-s"
LFLAGS="-Lvendor/raylib/src -lraylib -lm"
IFLAGS="-Isrc -Ivendor/raylib/src -Ivendor/raygui/src"

# build targets
if [ -v main ]; then
	echo "info: building target main."
	didbuild=1

	if [ -v release ]; then
		cflags="$COMMON_CFLAGS $RELEASE_CFLAGS"
		echo "info: release build"
	else
		cflags="$COMMON_CFLAGS $DEBUG_CFLAGS"
		echo "info: debug build"
	fi

	mkdir -p out
	echo "info: used flags $cflags $LFLAGS $IFLAGS"
	$CC build.c $cflags $LFLAGS $IFLAGS -o out/main
	if [ $? -eq 0 ]; then
		echo "info: main target built successfully."
		rsync -r assets out
		echo "info: synced assets directory."
	else
		exit 1
	fi
fi

# finally, warn if no build happened.
if [ ! -v didbuild ]; then
	echo "error: no valid build target specified."
	exit 1
fi
