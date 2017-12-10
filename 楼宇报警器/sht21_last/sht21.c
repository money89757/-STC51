#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/device.h>

#define SHT21_MAJOR 501
#define SHT21_MINOR 0

#define TEMP _IOR('s',0,int)
#define HUM  _IOR('s',1,int)

#define SHT21_T_HOLD   0xe3
#define SHT21_RH_HOLE  0xe5

static dev_t devno;
static struct cdev sht21_cdev;
static struct class *cls;
static struct device *devs;
static struct i2c_client *glo_client;

int sht21_read_data(unsigned char reg)
{
	int temp;
	int temp_l;
	int temp_h;
	char tbuf[1] = {reg};
	char rbuf[2];
	struct i2c_msg msg[] = {
	{glo_client->addr,0,1,tbuf},
	{glo_client->addr,1,2,rbuf},
	};
    i2c_transfer(glo_client->adapter,msg,ARRAY_SIZE(msg));

	temp_l =(int)rbuf[1];
	temp_h =(int)rbuf[0];
	temp = temp_h << 8 | temp_l;
	return temp;
}

void sht21_write_data(unsigned char reg,unsigned char val)
{
	char tbuf[2] = {reg,val};
	struct i2c_msg msg[] = {
		{glo_client->addr,0,2,tbuf},
	};
	i2c_transfer(glo_client->adapter,msg,ARRAY_SIZE(msg));
}

static long sht21_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	int ret1,ret2,temp,hum;

	switch(cmd)
	{
		case TEMP:	
			temp = sht21_read_data(SHT21_T_HOLD);
			temp = 0xffffffc & temp;
			ret1 = copy_to_user((void *)arg,&temp,sizeof(temp));
			if(ret1 < 0)
			{
				printk("ret1 = %d\n",ret1);
				return -EBUSY;
			}
			break;
		case HUM:
			hum = sht21_read_data(SHT21_RH_HOLE);
			hum = 0xffffffc & hum;
			ret2 = copy_to_user((void *)arg,&hum,sizeof(hum));
			if(ret2 < 0)
			{
				printk("ret2 = %d\n",ret2);
				return -EBUSY;
			}
			break;
		default:
			break;
	}

	return 0;
}

static int sht21_open(struct inode *inode ,struct file *filp)
{
	printk("sht21_open\n");
	return 0;
}

static int sht21_close(struct inode *inode,struct file *filp)
{
	printk("sht21_close\n");
	return 0;
}

static struct file_operations sht21_ops = {
	.open    = sht21_open,
	.release = sht21_close,
	.unlocked_ioctl   = sht21_ioctl,
};

static int sht21_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
	int ret;
	printk("match OK\n");
	glo_client = client;
	devno = MKDEV(SHT21_MAJOR,SHT21_MINOR);

	ret = register_chrdev_region(devno,1,"sht20");
	if(ret)
	{
		printk("register_chrdev_region is fail\n");
		return -EBUSY;
	}
	cdev_init(&sht21_cdev,&sht21_ops);
	ret = cdev_add(&sht21_cdev,devno,1);
	if(ret)
	{
		printk("cdev_add is fail\n");
		unregister_chrdev_region(devno,1);
	}
	cls = class_create(THIS_MODULE,"sht21_demo");
	if(IS_ERR(cls))
	{
		ret = PTR_ERR(cls);
		printk("class_create is fail\n");
		return ret;
	}
	
	devs = device_create(cls,NULL,devno,NULL,"sht21");
	if(IS_ERR(devs))
	{
		ret = PTR_ERR(devs);
		printk("device_create is fail\n");
		return ret;
	}
	return 0;
}

static int sht21_remove(struct i2c_client *client)
{
	device_destroy(cls,devno);
	class_destroy(cls);
	cdev_del(&sht21_cdev);
	unregister_chrdev_region(devno,1);
	printk("sht21_remove\n");
	return 0;
}

static struct of_device_id sht21_of_table[] = {
	{
		.compatible = "fs4412,sht21",
	},
	{},
};

static struct i2c_device_id sht21_id_table[] = {
	{
		.name = "sht21",
	},
	{}
};

static struct i2c_driver sht21_drv = {
	.probe  = sht21_probe,
	.remove = sht21_remove,
	.id_table = sht21_id_table,
	.driver = {
		.name = "sht21",
		.of_match_table = sht21_of_table,
	},
};

static int sht21_init(void)
{
	i2c_add_driver(&sht21_drv);
	printk("sht21_init is OK!\n");
	return 0;
}

static void sht21_exit(void)
{
	i2c_del_driver(&sht21_drv);
	printk("sht21_exit is remove!\n");
	return ;
}

module_init(sht21_init);
module_exit(sht21_exit);
MODULE_LICENSE("GPL");
