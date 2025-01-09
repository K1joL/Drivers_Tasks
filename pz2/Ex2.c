#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>
#include <linux/timekeeping.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

static u64 last_print_time;
static u64 min_print_interval_ns = 1000000000;

static char *message = "Пук";
static int freq_sec = 1;

module_param(message, charp, 0);
module_param(freq_sec, int, 0);

struct params
{
    int freq_sec;
    char* message;
};

int mythread(void* params)
{
    struct params* param = (struct params*)params;
    int i = 0;
	u64 current_time = 0;
    u64 time_diff = 0;
    u64 current_interval = param->freq_sec * min_print_interval_ns;
    while(1)
    {
		current_time = ktime_get_ns();
        time_diff = current_time - last_print_time;
    	if(time_diff > current_interval){
        	printk(KERN_INFO "%s\n", param->message);
        	last_print_time = current_time;
        }else
        	i++;
    if (kthread_should_stop()) break;
    }
    return 0;
}

struct task_struct * thread = NULL;
struct params param;

static int __init mess_init(void)
{
    param.message = message;
    param.freq_sec = freq_sec;
    last_print_time = ktime_get_ns();
    printk(KERN_ALERT "Привяо..");
    thread = kthread_run(mythread, &param, "foo");
    return 0;
}


static void __exit mess_exit(void)
{
    kthread_stop(thread);
    printk(KERN_ALERT "Пока..");
}

module_init(mess_init);
module_exit(mess_exit);