Introduction
-------------
This application will create mvmdio-uio.ko kernel module that needs to
be inserted from userspace to access mdio bus from userspace

Compilation
------------
1. Set following environment variables 
   $ export CROSS_COMPILE=<path/to/toochain>
   $ export ARCH=arm64
   $ export KERNELDIR=<path/to/kernel_dir>
2. Compile the kernel code
   $ make
3. copy mvmdio-uio.ko to the target board.

DTS Modification
-----------------
/ {
        cpn-110-master {
                config-space {
                        mdio: mdio@12a200 {
                                #address-cells = <1>;
                                #size-cells = <0>;
                        };
                        switch {
                                compatible = "marvell,mvmdio-uio";
                                status = "okay";
                                mii-bus = <&mdio>;
                        };
		};
	};
};

On Target Board
----------------
1. Use Image and dtb file compiled with above modification to boot the board.
2. Insert kernel module
   $ insmod mvmdio-uio.ko
   This will create a /dev/mvmdio-uio device
3. Now run the application which opens /dev/mvmdio-uio device and uses
   read/write register functions
