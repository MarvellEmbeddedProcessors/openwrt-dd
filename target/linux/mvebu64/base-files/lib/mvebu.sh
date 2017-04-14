#!/bin/sh
#
# Copyright (C) 2017 OpenWrt.org
#

MVEBU_BOARD_NAME=
MVEBU_MODEL=

mvebu_board_detect() {
	local machine
	local name

	machine=$(cat /proc/device-tree/model)

	case "$machine" in
	*"Marvell Armada-8040 development board A setup")
		name="armada-a8k-8040-db"
		;;
	*"Marvell Armada-7040 development board A setup")
		name="armada-a7k-7040-db"
		;;
	*"Marvell Armada 3720 Development Board DB-88F3720-DDR3")
		name="armada-3720-db"
		;;
	*"Marvell Armada 3700 Community Board")
		name="armada-ESPRESSObin"
		;;
	*"Marvell 8040 MACHIATOBin")
		name="armada-a8k-MACCHIATOBin"
		;;
	*)
		name="unknown"
		;;
	esac

	[ -z "$name" ] && name="unknown"

	[ -z "$MVEBU_BOARD_NAME" ] && MVEBU_BOARD_NAME="$name"
	[ -z "$MVEBU_MODEL" ] && MVEBU_MODEL="$machine"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	echo "$MVEBU_BOARD_NAME" > /tmp/sysinfo/board_name
	echo "$MVEBU_MODEL" > /tmp/sysinfo/model
}

mvebu_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] || mvebu_board_detect
	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}
