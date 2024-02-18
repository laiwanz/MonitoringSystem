#!/bin/sh

echo "---------------fs update start--------------" 
#$1 
IMG_PATH=$1
mount -o remount rw, /
killall PTMS
#升级文件系统
gunzip -f $IMG_PATH/fs.tar.gz -c > /tmp/fs.tar
if [ $? -eq 0 ]; then
	sync
	tar xvf /tmp/fs.tar -C /
	if [ $? -eq 0 ]; then
		sync
		touch  /tmp/updatefs_ok
	else
		touch  /tmp/updatefs_err11
	fi
else
    touch  /tmp/updatefs_err22
fi


#没用了删掉
rm -f /tmp/fs.tar
rm -f $IMG_PATH/fs.tar.gz

#mkdir /mnt/wecon/app/hmiapp/etc
#mv -f /etc/afterHMIApp.sh /mnt/wecon/app/hmiapp/etc/
chmod 777 /mnt/wecon/app/hmiapp/etc/afterHMIApp.sh
#mv -f /etc/monitoring_system /mnt/data/
chmod -R 777 /mnt/data/monitoring_system/*

echo "---------------fsFLASH update end--------------" 
sync
sync
