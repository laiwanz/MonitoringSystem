#!/bin/sh
#

#parse params
if [ $# -lt 3 ]; then
	echo "params: cbl -r -a7 -nrb "
	exit 1
fi

work_path="$(dirname $(readlink -f $0))"
project_name=""
build_type="-r"
plate_name="-x86"
is_rebuild="true"

while [ "$1" ]; do

	echo "arg is [$1]"
	case "$1" in
		-p)
			project_name="$2"
			shift
			;;
		-r|-d)
			build_type="$1"
			;;
		-a7|-a8|-a8plus|-rk3308|-x86|-ssd212)
			plate_name="$1"
			;;
		-rb)
			is_rebuild="true"
			;;
		-nrb)
			is_rebuild="false"
			;;
		*)
	esac
	shift
done

if [ -z "${project_name}" ]; then
	echo "project name error!"
	exit 3
fi

echo "work_path=${work_path}"

project_dir="${work_path}/${project_name}/${project_name}"
echo "project_dir=${project_dir}"

case "${plate_name}" in
	-a7)
		build_dir="${work_path}/build/a7"
		echo "plateform=a7"
		;;
	-a8|-a8plus)
		build_dir="${work_path}/build/a8"
		echo "plateform=a8"
		;;
	-rk3308)
		build_dir="${work_path}/build/rk3308"
		echo "plateform=rk3308"
		;;
	-x86)
		build_dir="${work_path}/build/x86"
		echo "plateform=x86"
		;;
	-ssd212)
	build_dir="${work_path}/build/ssd212"
	echo "plateform=ssd212"
	;;
	*)
esac

if [ "-r" == "${build_type}" ]; then
	echo "build_type=release"
	build_dir="${build_dir}/release"
else
	echo "build_type=debug"
	build_dir="${build_dir}/debug"
fi

build_dir="${build_dir}/${project_name}"
echo "build_dir=${build_dir}"
echo "is_rebuild=${is_rebuild}"

if [ ! -d $project_dir ]; then
	echo "project {$project_dir} no exist "
	exit 4
fi

function create_build_dir()
{
	if [ ! -d ${build_dir} ]; then
		mkdir -p ${build_dir}
		if [ 0 -ne $? ]; then
			return 1
		fi
	else
		rm -rf ${build_dir}/*
		if [ 0 -ne $? ]; then
			return 2
		fi
	fi
	
	cd ${build_dir}
	xmake ${build_type} ${plate_name} ${project_dir}
	if [ 0 -ne $? ]; then
		return 3
	fi
	
	return 0
}

function build_project()
{
	if [ ! -d ${build_dir} ]; then
		echo "build dir [${build_dir}] not exist use rebuild"
		create_build_dir
		if [ 0 -ne $? ]; then
			echo "creat build dir error!"
			return 1
		fi
	fi
	
	cd ${build_dir}
	make -j32
	if [ 0 -ne $? ]; then
		return 2
	fi
	
	return 0
}

#no rebuild
if [ "false" == "${is_rebuild}" ]; then
	echo "rsy:: ---no rebuild---"
else
	echo "rsy:: ---rebuild---"
	create_build_dir
	if [ 0 -ne $? ]; then
		echo "creat build dir error!"
		exit 5
	fi
fi

build_project
if [ 0 -ne $? ]; then
	exit 6
fi

if [ "-r" == ${build_type} ]; then
	make install/strip
else
	make install
fi

echo "done"
exit 0
