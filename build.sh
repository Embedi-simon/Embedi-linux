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

function linux_config()
{
	echo "linux config"
	cd $LINUX_PATH
	pwd
	make menuconfig
}

function linux_build()
{
	echo "linux compile"
	cd $LINUX_PATH
	pwd
	make
}

function busybox_config()
{
	echo "busybox config"
	cd $BUSYBOX_PATH
	pwd
	make menuconfig
}

function busybox_build()
{
	echo "busybox compile"
	cd $BUSYBOX_PATH
	pwd
	make
}

linux_path=./linux-6.9.1
#compile
if [ $# -lt 1 ]; then
	usage
else
	case $1 in
		-m)
			case $2 in
				linux)
					linux_config
					;;
				busybox)
					busybox_config
					;;
				*)
					usage
					;;
			esac
			;;
		-b)
			case $2 in
				linux)
					linux_build
					;;
				busybox)
					busybox_build
					;;
				*)
					usage
					;;
			esac
			;;
		*)
			usage
			;;
	esac

fi
