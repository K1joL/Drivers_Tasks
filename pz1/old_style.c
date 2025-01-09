#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/device.h>

MODULE_LICENSE("GPL");

#define DEVICE_NAME "stariy_driver"
#define IOCTL_RESET_BUFFER _IO('a', 1)
#define IOCTL_SET_SIZE _IOW('a', 2, int)

static int major;
static char *buffer;
static size_t buffer_size = 1024;

static struct class *dev_class;
static struct device *dev_device;

static ssize_t device_read(struct file *file, char __user *user_buffer, size_t count, loff_t *offset) 
{
   	printk(KERN_INFO "Stariy driver: offset %lld count %d\n", *offset, count);
	printk(KERN_INFO "Stariy driver: %p\n", buffer);
    if (count > buffer_size) 
        return 0;
    if (copy_to_user(user_buffer, buffer, count))
        return -EFAULT;
    return count;
}

static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *offset) 
{
	printk(KERN_INFO "Stariy driver: offset %lld count %d\n", *offset, count);
    if (*offset >= buffer_size)
        return -ENOSPC;
    if (*offset + count > buffer_size)
        count = buffer_size - *offset;
    if (copy_from_user(buffer + *offset, user_buffer, count))
        return -EFAULT;
    *offset += count;
    return count;
}

static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg) 
{
    switch (cmd) 
	{
        case IOCTL_RESET_BUFFER:
            memset(buffer, 0, buffer_size);
            file->f_pos = 0;
            break;
        case IOCTL_SET_SIZE:
            if (arg > 0) 
			{
                char *new_buffer = krealloc(buffer, arg, GFP_KERNEL);
                if (!new_buffer)
                    return -ENOMEM;
                buffer = new_buffer;
                buffer_size = arg;
                memset(buffer, 0, buffer_size);
                file->f_pos = 0;
            }
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static struct file_operations fops = 
{
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = device_ioctl,
};

int init_module(void) 
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register stariy driver\n");
        return major;
    }
	
    dev_class = class_create("stariy_driver_class");
    if (IS_ERR(dev_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create device class\n");
        return PTR_ERR(dev_class);
    }

    dev_device = device_create(dev_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(dev_device)) {
       	class_destroy(dev_class);
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create device\n");
        return PTR_ERR(dev_device);
    }
	
    buffer = kmalloc(buffer_size, GFP_KERNEL);
    if (!buffer) {
    	device_destroy(dev_class, MKDEV(major, 0));
        class_destroy(dev_class);
        unregister_chrdev(major, DEVICE_NAME);
        return -ENOMEM;
    }
    
    memset(buffer, 0, buffer_size); // Initialize buffer
    printk(KERN_INFO "Stariy driver registered with major number %d\n", major);
    return 0;
}

void cleanup_module(void) 
{
	device_destroy(dev_class, MKDEV(major, 0));
	class_destroy(dev_class);
    kfree(buffer);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Stariy driver unregistered\n");
}
