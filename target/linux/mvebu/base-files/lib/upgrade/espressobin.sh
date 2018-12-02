platform_do_flash() {
    local tar_file=$1
    local board=$2
    local kernel=$3
    local rootfs=$4

    mkdir -p /rootfs
    mount /dev/$rootfs /rootfs

    echo "flashing rootfs to /dev/$rootfs"
    rm -rf /rootfs/*
    tar xzf $tar_file -C /rootfs
    sync
    umount /rootfs
}

platform_do_upgrade_espressobin() {
    local tar_file="$1"
    local board=globalscale,espressobin
    local kernel=sda1
    local rootfs=sda1

    platform_do_flash $tar_file $board $kernel $rootfs
}

platform_copy_config_espressobin() {
	return 0
}

