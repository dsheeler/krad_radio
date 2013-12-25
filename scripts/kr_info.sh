#!/bin/bash

echo "System Info:"
date
uname -a
free -m
ls -l /sys/bus/cpu/devices
cat /sys/bus/cpu/devices/cpu0/cpufreq/cpuinfo_min_freq
cat /sys/bus/cpu/devices/cpu0/cpufreq/cpuinfo_max_freq
cat /sys/bus/cpu/devices/cpu0/cpufreq/scaling_governor
lsmod | grep black
ls /dev/video*
cat /sys/class/graphics/fb0/name
ls /sys/class/video4linux/
ls /sys/class/sound/

echo "Krad Info:"
which kr
which krad_radio
ldd /usr/local/bin/kr
ldd /usr/local/bin/krad_radio
kr --version
kr --vg
echo "Running Stations:"
kr ls
