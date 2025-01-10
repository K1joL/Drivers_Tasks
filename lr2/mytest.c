#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/timer.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mytest"
#define INITIAL_BUFFER_SIZE 100

// ioctl command
#define MYTEST_GET_REACTIONS _IOR('m', 0, ktime_t *)
#define MYTEST_GET_COUNT _IOR('m', 1, int *)

static struct timer_list my_timer;
static int frequency = 1;
static ktime_t event_time;
static ktime_t *response_time = NULL;
static int buffer_size = INITIAL_BUFFER_SIZE;
static int index = 0;
static bool event_pending = false;
static bool reaction_pending = false;

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *dev_class;

module_param(frequency, int, 0);

static int device_open(struct inode *inode, struct file *file) {
  // Reset response buffer and index
  if (response_time != NULL) {
    kfree(response_time);
    response_time = NULL;
  }
  index = 0;
  buffer_size = INITIAL_BUFFER_SIZE;

  response_time = kmalloc(sizeof(ktime_t) * buffer_size, GFP_KERNEL);
  if (!response_time) {
    printk(KERN_ERR "Failed to allocate buffer in open\n");
    return -ENOMEM;
  }
  return 0;
}

static ssize_t device_read(struct file *file, char __user *buffer, size_t len,
                           loff_t *offset) {
  if (!event_pending) return 0;

  reaction_pending = true;
  return 1;
}

static ssize_t device_write(struct file *file, const char __user *buffer,
                            size_t len, loff_t *offset) {
  ktime_t reaction_time;

  if (!reaction_pending) return -EINVAL;

  ktime_t current_time = ktime_get();
  reaction_time = ktime_sub(current_time, event_time);

  if (index >= buffer_size) {
    buffer_size *= 2;
    response_time =
        krealloc(response_time, sizeof(ktime_t) * buffer_size, GFP_KERNEL);
    if (!response_time) {
      return -ENOMEM;
    }
  }

  response_time[index++] = reaction_time;
  printk(KERN_INFO "Reaction Time: %lld ns\n", ktime_to_ns(reaction_time));
  event_pending = false;
  reaction_pending = false;
  return len;
}

void timer_callback(struct timer_list *timer) {
  if (event_pending) {
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000 / frequency));
    return;
  }

  event_time = ktime_get();
  event_pending = true;

  mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000 / frequency));

  printk(KERN_INFO "External Event Triggered\n");
}

static long device_ioctl(struct file *file, unsigned int cmd,
                         unsigned long arg) {
  switch (cmd) {
    case MYTEST_GET_REACTIONS:
      if (response_time == NULL) {
        return -EFAULT;
      }
      if (copy_to_user((ktime_t *)arg, response_time, sizeof(ktime_t) * index))
        return -EFAULT;
      printk(KERN_INFO "Returning %d reaction times via ioctl\n", index);
      break;
    case MYTEST_GET_COUNT:
      if (copy_to_user((int *)arg, &index, sizeof(int))) return -EFAULT;
      printk(KERN_INFO "Returning %d as reaction count via ioctl\n", index);
      break;
    default:
      return -ENOTTY;
  }
  return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .read = device_read,
    .write = device_write,
    .unlocked_ioctl = device_ioctl,
};

static int __init mytest_init(void) {
  int result;

  result = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
  if (result < 0) {
    printk(KERN_ALERT "Failed to allocate device number\n");
    return result;
  }

  cdev_init(&my_cdev, &fops);
  my_cdev.owner = THIS_MODULE;
  result = cdev_add(&my_cdev, dev_num, 1);
  if (result < 0) {
    printk(KERN_ALERT "Failed to add character device\n");
    unregister_chrdev_region(dev_num, 1);
    return result;
  }

  dev_class = class_create(DEVICE_NAME);
  if (IS_ERR(dev_class)) {
    printk(KERN_ALERT "Failed to create device class\n");
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    return PTR_ERR(dev_class);
  }

  if (IS_ERR(device_create(dev_class, NULL, dev_num, NULL, DEVICE_NAME))) {
    printk(KERN_ALERT "Failed to create device file\n");
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    return PTR_ERR(device_create(dev_class, NULL, dev_num, NULL, DEVICE_NAME));
  }

  response_time = kmalloc(sizeof(ktime_t) * buffer_size, GFP_KERNEL);
  if (!response_time) {
    device_destroy(dev_class, dev_num);
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    return -ENOMEM;
  }
  timer_setup(&my_timer, timer_callback, 0);
  mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000 / frequency));

  printk(KERN_INFO "mytest Driver Initialized\n");
  return 0;
}

static void __exit mytest_exit(void) {
  del_timer(&my_timer);
  if (response_time != NULL) kfree(response_time);
  device_destroy(dev_class, dev_num);
  class_destroy(dev_class);
  cdev_del(&my_cdev);
  unregister_chrdev_region(dev_num, 1);

  printk(KERN_INFO "mytest Driver Exited\n");
}

module_init(mytest_init);
module_exit(mytest_exit);

MODULE_LICENSE("GPL");