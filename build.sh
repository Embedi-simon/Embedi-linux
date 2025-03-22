#!/bin/bash

# 声明环境变量
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-

CUR_PATH=`pwd`
LINUX_PATH="$CUR_PATH/linux-6.9.1"
BUSYBOX_PATH="$CUR_PATH/busybox-1.36.1"

function usage()
{
	echo "usage:"
	echo "	linux: build linux"
	echo "	busybox: build busybox"
}

function build_linux()
{
	echo "build linux"
	cd $LINUX_PATH
	pwd
	make menuconfig
}


function build_busybox()
{
	echo "build busybox"
	cd $BUSYBOX_PATH
	pwd
	make menuconfig
}

linux_path=./linux-6.9.1
#compile
if [ $# -lt 1 ]; then
	usage
else
	case $1 in
		linux)
			build_linux
		;;
		busybox)
			build_busybox
		;;
		*)
		usage
		;;
	esac

fi
