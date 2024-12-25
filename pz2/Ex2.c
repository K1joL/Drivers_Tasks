#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>
MODULE_LICENSE("GPL");

static char *message = "Пук";
static int freq = 1;

module_param(message, charp, 0);
module_param(freq, int, 0);

struct params
{
    int freq;
    char* message;
};

int mythread(void* params)
{
    struct params* param = (struct params*)params;
    while(1)
    {
        for (int i = 0; i<param -> freq; i++ )
        {
            printk(KERN_INFO "%s\n", param -> message);
        }
    if (kthread_should_stop()) break;
    }
    return 0;
}

struct task_struct * thread = NULL;
struct params param;

static int __init mess_init(void)
{
    param.message = message;
    param.freq = freq;
    thread = kthread_run(mythread, &param, "foo");
    return 0;
}


static void __exit mess_exit(void)
{
    printk(KERN_ALERT "Пока...Кай");
    kthread_stop(thread);
}

module_init(mess_init);
module_exit(mess_exit);
