#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/pci.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/module.h>
struct input_dev *vms_input_dev;
static struct platform_device *vms_dev;


MODULE_LICENSE("Dual BSD/GPL");

static ssize_t write_vms(struct device *dev,
	       	struct device_attribute *attr,
		const char *buffer, size_t count)
{
	int x,y;
	sscanf(buffer, "%d%d", &x,&y);
	pr_info("[vms] buffer = %s, x= %d, y= %d\n", buffer, x, y);
	input_report_rel(vms_input_dev, REL_X, x);
	input_report_rel(vms_input_dev, REL_Y, y);

	input_sync(vms_input_dev);
	return count;
}

DEVICE_ATTR(coordinates, 0644, NULL, write_vms);

static struct attribute *vms_attrs[] = {
	&dev_attr_coordinates.attr,
	NULL,
};	

static struct attribute_group vms_attr_group = {
	.attrs = vms_attrs,
};

int __init vms_init(void)
{
	vms_dev = platform_device_register_simple("vms", -1, NULL, 0);
	if(IS_ERR(vms_dev)) {
		pr_info("vms_init: ERR\n");
		return PTR_ERR(vms_dev);
	}

	sysfs_create_group(&vms_dev->dev.kobj, &vms_attr_group);
	vms_input_dev = input_allocate_device();

	if(!vms_input_dev) {
		pr_info("Bad input_allocate_device()\n");
		return -ENOMEM;
	}

	vms_input_dev->name = "Virtual Mouse";
	vms_input_dev->phys = "vmd/input0"; // "vmd" is the driver's name
	vms_input_dev->id.bustype = BUS_VIRTUAL;
	vms_input_dev->id.vendor  = 0x0000;
	vms_input_dev->id.product = 0x0000;
	vms_input_dev->id.version = 0x0000;

	set_bit(EV_REL, vms_input_dev->evbit);
	set_bit(REL_X, vms_input_dev->relbit);
	set_bit(REL_Y, vms_input_dev->relbit);

	input_register_device(vms_input_dev);
	pr_info("virtual mouse register success\n");
	return 0;
}

void __exit vms_cleanup(void)
{
	input_unregister_device(vms_input_dev);
	sysfs_remove_group(&vms_dev->dev.kobj, &vms_attr_group);
	platform_device_unregister(vms_dev);
}


module_init(vms_init);
module_exit(vms_cleanup);
