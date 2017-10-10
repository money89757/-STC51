#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/io.h>

#define GPX2CON 0x11000c40

#define LED_ON  _IO('b',0)
#define LED_OFF _IO('b',1)

volatile unsigned int *gpx2con;
volatile unsigned int *gpx2dat;

static int led_major = 500;
static int led_minor = 0;
static struct cdev led_cdev;
struct class *led_class;
struct device *dev;

dev_t devno;
static void led_init(void)
{
    writel((readl(gpx2con)& ~(0xf<<28)) |(0x1<<28),gpx2con) ;
}

static void led_on(void)
{
    writel(readl(gpx2dat) |(0x1<<7), gpx2dat);  
}

static void led_off(void)
{
    writel(readl(gpx2dat) &(~(0x1<<7)), gpx2dat);  
}

static int led_demo_open(struct inode *inode, struct file *file)
{
	printk("led: device open\n");
	led_init();
	return 0;
}

static int led_demo_close(struct inode *inode, struct file *file)
{
	printk("led: device close\n");
	return 0;
}

static long led_demo_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	printk("led : device ioctl\n");
	switch(cmd)
	{
		case LED_ON:
			printk("led:ON\n");
			led_on();
			break;
		case LED_OFF:
			printk("led:OFF\n");
			led_off();
			break;
		default:
			printk("led ioctl fail\n");
			break;
	}
	return 0;
}

static struct file_operations led_ops = {
    .owner           = THIS_MODULE,
    .open           = led_demo_open,
    .release        = led_demo_close,
    .unlocked_ioctl = led_demo_ioctl,
};

static ssize_t led_demo_enable_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{

	return 0;
}
static ssize_t led_demo_enable_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	unsigned long data;
	int error;

	error = strict_strtoul(buf, 10, &data);
	printk("set value = %d\n", data);

	if (error)
		return error;

	if (data == 1) {
		printk("turn on\n");
		led_init();
		led_on();
	}
	else {
		printk("turn off\n");
		led_off();
	}

	return count;
}

static DEVICE_ATTR(enable,0777,
            led_demo_enable_show,led_demo_enable_store);

static struct attribute *led_demo_attributes[] = {
    &dev_attr_enable.attr,
    NULL,
};

static struct attribute_group led_demo_attributes_group = {
    .attrs = led_demo_attributes,
};

static int led_demo_init(void)
{
    int ret;
    devno = MKDEV(led_major,led_minor);
    ret = register_chrdev_region(devno,1,"led_demo");
    if(ret)
    {
        printk("register_chrdev_region is fail\n");
        return ret;
    }
    cdev_init(&led_cdev,&led_ops);
    ret = cdev_add(&led_cdev,devno,1);
    if(ret)
    {
        printk("cdev_add is fail\n");
        unregister_chrdev_region(devno,1);
    }

    gpx2con = ioremap(GPX2CON,4);
	gpx2dat = gpx2con + 1;
    *gpx2con = (*gpx2con & ~(0xf << 28)) | 1 << 28; 

    led_class = class_create(THIS_MODULE,"led_demo");
    if(IS_ERR(led_class))
    {
       	printk("class_create fail\n");
		return -EFAULT; 
    }

	dev = device_create(led_class, NULL, devno, NULL, "led");

    ret = sysfs_create_group(&dev->kobj,&led_demo_attributes_group);
    if(ret < 0)
    {
        printk("led: sysfs_create_group err\n");
		return -EFAULT;
    }
    return 0;
}

static void led_demo_exit(void)
{
	sysfs_remove_group(&dev->kobj, &led_demo_attributes_group);
	device_destroy(led_class, devno);
	class_destroy(led_class);
	iounmap(GPX2CON);
    cdev_del(&led_cdev);
	unregister_chrdev_region(devno, 1);
	printk("led_demo_exit\n");
}

module_init(led_demo_init);
module_exit(led_demo_exit);
MODULE_LICENSE("GPL");


