#!/bin/sh

echo "rsy::params $# $0 $1 $2 $3 $4"

#parse params
if [ $# -lt 3 ]
then
	#PrintUsage
	echo "params: -r -a7 -nrb or -r -a7 -nrb HmiBase"
	exit 1
fi

BUILD_TYPE=$1
BUILD_PLATFORM=$2
IS_REBUILD=$3
PRJ_NAME=$4
COLOR_RED="\033[1;31m"
COLOR_GREEN="\033[1;32m"
COLOR_YEALLOW="\033[1;33m"
COLOR_RESET="\033[0m"
PRJ_BUILD_OK=""
PRJ_BUILD_ERR=""
PRJ_REMAIN=""


function build(){
	STOP_INSTALL=0
	RET=0
	PROJECT_LIST="PTMS"
	for i in $PROJECT_LIST;
	do
		if [ 0 -ne $STOP_INSTALL ]; then
			PRJ_REMAIN="$PRJ_REMAIN $i\n"
			continue;
		fi
		
		PLATFORM=$BUILD_PLATFORM
		if [ $PLATFORM == "-a8" ]; then
			PLATFORM="-a8plus"
		fi
		
		./build_install.sh -p $i $BUILD_TYPE $PLATFORM $IS_REBUILD
		if [ 0 -ne $? ]; then
			PRJ_BUILD_ERR=$i
			STOP_INSTALL=1
			RET=1
		else
			PRJ_BUILD_OK="$PRJ_BUILD_OK $i\n"
		fi
	done 
	
	return $RET
}

if [ $# -eq 4 ];then
	if [ $PRJ_NAME == "Custom_Part" ]; then
		build
	else
		PLATFORM=$BUILD_PLATFORM
		if [ $PLATFORM == "-a8" ]; then
			PLATFORM="-a8plus"
		fi
		./build_install.sh -p $PRJ_NAME $BUILD_TYPE $PLATFORM $IS_REBUILD
	fi
else
build
fi


if [ 0 -ne $? ]; then
	echo -ne "$COLOR_GREEN"
	echo -ne "$PRJ_BUILD_OK"
	
	echo -ne "$COLOR_RED"
	echo -e " $PRJ_BUILD_ERR"

	echo -ne "$COLOR_YEALLOW"
	echo -ne "$PRJ_REMAIN"
	
	echo -ne "$COLOR_RESET"
	exit 1
else
	echo -ne "$COLOR_GREEN"
	echo "all project build success!"
	echo -ne "$PRJ_BUILD_OK"
	echo -ne "$COLOR_RESET"
	exit 0
fi
