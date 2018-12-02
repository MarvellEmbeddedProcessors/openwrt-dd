#include <linux/platform_device.h>
#include <linux/of_mdio.h>
#include <linux/cdev.h>

#define MVMDIO_DEV_NAME "mvmdio-uio"

static struct mii_bus *mv_mii_bus;

typedef struct s_mii_data {
        unsigned short phyId;
        unsigned short reg;
        unsigned short data;
} mii_data;

static dev_t dev_num;
static struct cdev mv_cdev;
static struct class *mv_cl;
static int retVal;

/* probe function to get handle to mdio bus */
static int mvmdio_probe(struct platform_device *pdev)
{
	struct device_node *np;
	struct device_node *mdio;

	np = pdev->dev.of_node;
	mdio = of_parse_phandle(np, "mii-bus", 0);
	if (!mdio) {
		pr_err("[SOHO] mvmdio_probe : parse handle failed\n");
		return -EINVAL;
	}

	mv_mii_bus = of_mdio_find_bus(mdio);
	if (!mv_mii_bus) {
		pr_err("[SOHO] mvmdio_probe : mdio find bus failed\n");
		return -EINVAL;
	}

	return 0;
}

static int mvmdio_remove(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id mvmdio_match[] = {
	{ .compatible = "marvell,mvmdio-uio" },
	{ }
};

static struct platform_driver mvmdio_driver = {
	.probe = mvmdio_probe,
	.remove = mvmdio_remove,
	.driver = {
		.name = MVMDIO_DEV_NAME,
		.of_match_table = mvmdio_match,
	},
};

/* Create character device */
static int mv_mdio_device_open(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t mv_mdio_device_read(struct file *file, char *buf, size_t count, loff_t *f_pos)
{
	int ret;
	mii_data *mii;

	mii = (mii_data*)buf;

	ret = mv_mii_bus->read(mv_mii_bus, mii->phyId, mii->reg);
	if (ret < 0) {
		pr_err("smi read failed at devAddr: %X, regAddr: %X\n",
		       mii->phyId, mii->reg);
		return -1;
	}
	mii->data = (unsigned)ret;

	return 0;
}

static ssize_t mv_mdio_device_write(struct file *file, const char *buf, size_t count, loff_t *f_pos)
{
	int ret;
	mii_data *mii;

	mii = (mii_data*)buf;

	ret = mv_mii_bus->write(mv_mii_bus, mii->phyId, mii->reg, mii->data);
	if (ret < 0) {
		pr_err("smi write failed at devAddr: %X, regAddr: %X\n",
		       mii->phyId, mii->reg);
		return -1;
	}

	return 0;
}

static int mv_mdio_device_close(struct inode *inode, struct file *file)
{
	return 0;
}

static struct file_operations fops =
{
	.owner                = THIS_MODULE,
	.open                 = mv_mdio_device_open,
	.read                 = mv_mdio_device_read,
	.write                = mv_mdio_device_write,
	.release              = mv_mdio_device_close,
};

static int mv_mdio_device_init(void)
{
	/* register mvmdio_driver */
	platform_driver_register(&mvmdio_driver);

	retVal = alloc_chrdev_region(&dev_num, 0, 1, MVMDIO_DEV_NAME);
	if(retVal < 0)
	{
		printk("Error: Marvell Mdio Registration failed\n");
		return -1;
	}

	if((mv_cl = class_create(THIS_MODULE, "chardev"))== NULL)
	{
		printk("Error: Marvell Mdio class  creation failed\n");
		unregister_chrdev_region( dev_num, 1 );
		return -1;
	}

	if( device_create(mv_cl, NULL, dev_num, NULL, MVMDIO_DEV_NAME ) == NULL )
	{
		printk("Error: Marvell Mdio Device creation failed\n" );
		class_destroy(mv_cl);
		unregister_chrdev_region( dev_num, 1 );
		return -1;
	}

	cdev_init( &mv_cdev, &fops );

	if( cdev_add( &mv_cdev, dev_num, 1 ) == -1)
	{
		printk("Error: Marvell Mdio Device addition failed\n" );
		device_destroy( mv_cl, dev_num );
		class_destroy( mv_cl );
		unregister_chrdev_region( dev_num, 1 );
		return -1;
	}

	printk("Marvell Mdio Device Driver init success\n");
	return 0;
}

static void mv_mdio_device_exit(void)
{
	/* unregister mvmdio_driver */
	platform_driver_unregister(&mvmdio_driver);

	device_destroy( mv_cl, dev_num );
	class_destroy( mv_cl );
	unregister_chrdev_region( dev_num, 1 );
	printk("Marvell Mdio Device Driver exit success\n");
}

module_init(mv_mdio_device_init);
module_exit(mv_mdio_device_exit);

MODULE_DESCRIPTION("Marvell MDIO interface driver");
MODULE_LICENSE("GPL");
