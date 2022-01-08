#!/bin/bash

# This has extra echos for buildkite output parsing to auto collapse sections of a job
set -eo pipefail

DEBUG=debug
RELEASE=release

RELEASE_CONFIG_ARG=MinSizeRel
DEBUG_CONFIG_ARG=Debug

#Default to release build
CONFIG=release
CONFIG_ARG=$RELEASE_CONFIG_ARG

# This script only throws errors for args
ERROR_MESSAGE="ERROR: Invalid args.  Expected 0 or 1 arguments. Acceptable values: release or debug."

if [ $# -gt 1 ]; then
   echo $ERROR_MESSAGE
   exit 1
fi

if [ $# -eq 1 ]; then
	if [[ "$1" == $DEBUG || "$1" == $RELEASE ]]; then
		CONFIG=$1
	else
		echo $ERROR_MESSAGE
		exit 1
	fi
fi

if [[ "$CONFIG" == $DEBUG ]]; then
	CONFIG_ARG=$DEBUG_CONFIG_ARG
fi

if [ ! -d "./build/$CONFIG" ]; then
   echo "--- Ensuring $CONFIG build dir"
   mkdir -p ./build/$CONFIG
fi

echo "--- Configure Build"
# Please note that subsequent configurations will throw a warning about an unused variable for the toolchain.  This can be safely ignored.
cmake -DCMAKE_BINARY_DIR=./ \
	  -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
	  -DCMAKE_BUILD_TYPE:STRING=$CONFIG_ARG \
	  -DCMAKE_TOOLCHAIN_FILE=toolchain.gcc.cmake \
	  -H./ \
	  -B./build/$CONFIG \
	  -GNinja

ln -sf ./build/$CONFIG/compile_commands.json .
if [[ $? -ne 0 ]]; then
  echo "^^^ +++"
  echo "Failed to configure debug build!"
fi
