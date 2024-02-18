#!/bin/bash

DEVICE_PATH=/mnt/data/monitoring_system
DEVICE_PATHTMP=/tmp/monitoring_system

mkdir ${DEVICE_PATHTMP}
cp -af ${DEVICE_PATH}/PTMS ${DEVICE_PATHTMP}/PTMS

IP=192.168.36.66
PORT=6379

[ -x "${DEVICE_PATHTMP}/PTMS" ] && ${DEVICE_PATHTMP}/PTMS ${IP} ${PORT} &