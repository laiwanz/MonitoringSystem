#!/bin/bash

device_new_file="/mnt/data/monitoring_system/PTMS_new"
device_file="/mnt/data/monitoring_system/PTMS"

if [ -f "$device_new_file" ]; then
    
    chmod +x "$device_new_file"

   
    pid=$(pid=$(ps -ef | grep "/mnt/data/monitoring_system/PTMS" | grep -v grep | awk '{print $2}'))
    if [ -n "$pid" ]; then
        kill "$pid"
        kill_status=$?
        if [ $kill_status -eq 0 ]; then
            echo "Previous PTMS process terminated successfully."
        else
            echo "Failed to terminate previous PTMS process with PID $pid."
        fi
    fi



    mv "$device_new_file" "$device_file"
    

    #"$device_file" &
else
    echo "No device_new file found."
fi