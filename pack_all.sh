#!/bin/sh


if [ $# -lt 1 ]; then
	echo "./pack_all a7|a8|rk3308|ssd212|all"
	exit 1
fi

plate_name=""
pack_sqush=0
PLATE_LIST="a7 a8 rk3308 ssd212"



function build()
{
	serial_name="$1"
	./build_all.sh -r -${serial_name} -rb
}

function pack()
{
	serial_name="$1"
	./pack.sh ${serial_name}
}

function packAll()
{
	for i in $PLATE_LIST;
	do
		pack "$i"
	done 
}

for arg in $*; do
	case "${arg}" in
	a7|a8|rk3308|ssd212)
		plate_name="${arg}"
		build ${plate_name}
		cd packMonitor
		pack ${plate_name}
	;;
	all)
		for i in $PLATE_LIST;
		do
			build "$i"
		done 
		cd packMonitor
		packAll
	;;
	*)
	esac
done



