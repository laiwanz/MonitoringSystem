#!/bin/sh
#

if [ $# -lt 1 ]; then
	echo "params: a8?"
	exit 1
fi

plate_name=""
pack_sqush=0

for arg in $*; do
	case "${arg}" in
	a7|a8|rk3308|ssd212)
		plate_name="${arg}"
	;;
	sqush)
		pack_sqush=1
	;;
	*)
	esac
done

work_path=$(realpath $(dirname $0))
src_exe="${MIDDLEWARE_ROOT}/${plate_name}/bin/PTMS"
scr_fs_name="fs"
src_dir="${work_path}/${scr_fs_name}"
fs_src_dir="${src_dir}/fs"
dest_exe_dir="${fs_src_dir}/mnt/data/monitoring_system/"
dest_dir="${work_path}/osf"
temp_dir="${MIDDLEWARE_ROOT}/osf"


if [ $# -lt 3 ]; then
	osf_version="VER:R05R006_V1.5.62_D20240119"
else
	osf_version="$3"
fi
osf_describe="com.wecon.hmi.fs"
osf_name="monitor.osf3"

function get_version_info()
{
	if [ $# -lt 1 ]; then
		echo "get_version_info params error  $#"
		exit 1
	fi

	ver_info=""
	case "$1" in
	PI3000)
		ver_info="{\"MIF\":\"AM335x_FLASH_v2\",\"PIF\":\"800_480\",\"PLFI\":\"Hmi.fs\",\"PLVS\":\"${osf_version}\"}"
	;;
	PI8000)
		ver_info="{\"MIF\":\"AM335x_eMMC_v2\",\"PIF\":\"800_480\",\"PLFI\":\"Hmi.fs\",\"PLVS\":\"${osf_version}\"}"
	;;
	PI9000)
		ver_info="{\"MIF\":\"s5p4418_FLASH_v2\",\"PIF\":\"1920_1080\",\"PLFI\":\"Hmi.fs\",\"PLVS\":\"${osf_version}\"}"
	;;
	PI3000i)
		ver_info="{\"MIF\":\"imx6ull_FLASH_v2\",\"PIF\":\"800_480\",\"PLFI\":\"Hmi.fs\",\"PLVS\":\"${osf_version}\"}"
	;;
	PI3000ie)
		ver_info="{\"MIF\":\"rk3308b_FLASH64Mmem_v2\",\"PIF\":\"800_480\",\"PLFI\":\"Hmi.fs\",\"PLVS\":\"${osf_version}\"}"
	;;
	PI3000ig)
		ver_info="{\"MIF\":\"rk3308b_FLASH_v2\",\"PIF\":\"800_480\",\"PLFI\":\"Hmi.fs\",\"PLVS\":\"${osf_version}\"}"
	;;
	PI8000ig)
		ver_info="{\"MIF\":\"rk3308b_eMMC_v2\",\"PIF\":\"1280_720\",\"PLFI\":\"Hmi.fs\",\"PLVS\":\"${osf_version}\"}"
	;;
	IN1071)
		ver_info="{\"MIF\":\"rk3308b_FLASH64Mmem_IN_v2\",\"PIF\":\"800_480\",\"PLFI\":\"Hmi.fs\",\"PLVS\":\"${osf_version}\"}"
	;;
	IN1072)
		ver_info="{\"MIF\":\"ssd212&mem=64M&flash=128M\",\"PIF\":\"800_480\",\"PLFI\":\"Hmi.fs\",\"PLVS\":\"${osf_version}\"}"
	;;
	MD3000)
		ver_info="{\"MIF\":\"rk3308b_eMMC_MD_v2\",\"PIF\":\"800_480\",\"PLFI\":\"Hmi.fs\",\"PLVS\":\"${osf_version}\"}"
	;;
	IN1101)
		ver_info="{\"MIF\":\"rk3308b_FLASH64Mmem_IN_v2\",\"PIF\":\"1024_600\",\"PLFI\":\"Hmi.fs\",\"PLVS\":\"${osf_version}\"}"
	;;
	*)
		echo "get_version_info err"
		exit 1
	esac

	echo "${ver_info}"
}

function move_to_fs_by_serial_name()
{
	echo "----------------move_to_fs_by_serial_name-------------------"
	if [ ! -f ${src_exe} ]; then
		echo "err dir [${src_exe}] not exit!!!"
		return 1
	fi
	
	cp -af ${src_exe} ${dest_exe_dir}
}

function pack_by_serial_name()
{
	if [ $# -lt 1 ]; then
		echo "pack_by_serial_name params error  $#"
		exit 1
	fi
	
	serial_name="$1"
	move_to_fs_by_serial_name

	if [ ! -d ${fs_src_dir} ]; then
		echo "err dir [${fs_src_dir}] not exit!!!"
		return 1
	fi
	
	temp_serial_dir="${temp_dir}/${serial_name}"
	if [ ! -d ${temp_serial_dir} ]; then
		echo "err dir [${temp_serial_dir}] not exit!!!"
		mkdir -p ${temp_serial_dir}
	else
		rm -rf ${temp_serial_dir}/*
	fi
	
	temp_fs_dir="${temp_serial_dir}/HMIFS"
	if [ ! -d ${temp_fs_dir} ]; then
		mkdir -p ${temp_fs_dir}
	else
		rm -rf ${temp_fs_dir}/*
	fi

	tar zcvf ${temp_fs_dir}/fs.tar.gz -C ${fs_src_dir} .
	cp -af ${src_dir}/updatefs.sh ${temp_fs_dir}/
	cp -af ${src_dir}/delete.sh ${temp_fs_dir}/
	
	chmod -R 777 ${temp_fs_dir}/*
	export LD_LIBRARY_PATH=${MIDDLEWARE_ROOT}/x86/lib:${LD_LIBRARY_PATH}
	
	tgz_out_file="${temp_dir}/${serial_name}/${osf_name}"
	if [ -e ${tgz_out_file} ]; then
		rm -rf ${tgz_out_file}
	fi

	version_info=`get_version_info "${serial_name}"`
	${work_path}/packtgz ${temp_fs_dir} ${version_info} ${tgz_out_file} ${osf_describe}
	
	serial_dest_dir="${dest_dir}"
	if [ ! -d ${serial_dest_dir} ]; then
		mkdir -p ${serial_dest_dir}
	fi
	
	serial_dest_osf="${serial_dest_dir}/${serial_name}_${osf_name}"
	cp -f ${tgz_out_file} ${serial_dest_osf}

	echo ""
	echo "----------------md5-------------------"
	md5sum ${tgz_out_file}
	echo "----------------md5-------------------"
	echo ""
}

case "$1" in
	a7)
		pack_by_serial_name "PI3000i"
	;;
	a8)
		pack_by_serial_name "PI3000"
		pack_by_serial_name "PI8000"
		pack_by_serial_name "PI9000"
	;;
	rk3308)
		pack_by_serial_name "PI3000ie"
		pack_by_serial_name "PI3000ig"
		pack_by_serial_name "PI8000ig"
		pack_by_serial_name "IN1071"
		pack_by_serial_name "MD3000"
		pack_by_serial_name "IN1101"
	;;
	ssd212)
		pack_by_serial_name "IN1072"
	;;
	*)
		echo "Usage: $1 config file not find"
		exit 1
esac

exit 0

