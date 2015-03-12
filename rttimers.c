/* Real-Time kernel-based application timers for realtime notifications of time
 * Copyright (C) 2015 Justin T. Crawford (Justasic)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <linux/module.h> // for module macros and functions
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/jiffies.h> // for global variable `jiffies'
#include <linux/signal.h>  // for force_sigsegv()
#include <linux/sched.h>   // for wake_up_process()

// For PROC
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <linux/seq_file.h>

static struct timer_list timers;

static int read_callback(struct seq_file *m, void *v)
{
	seq_printf(m, "Hello proc!\n");
	return 0;
}

static int open_callback(struct inode *inode, struct  file *file)
{
	// Get the task of the process opening the file.
	struct task_struct *task = current;
	
	// Kill said task.
	kill_pid(task_pid(task), SIGSEGV, 1);
	
	return single_open(file, read_callback, NULL);
}

struct proc_dir_entry *proc_file_entry;
static const struct file_operations proc_file_fops = {
	.owner   = THIS_MODULE,
	.open    = open_callback,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

void TimerCallback(unsigned long data)
{
	printk("Timer callback in %ld jiffies!\n", jiffies);
	
	// Basically, here we find the `struct task_struct` object
	// of the process we need to talk to and call `wake_up_process`
	// to schedule it next. This is the best way to make the kernel
	// schedule shit.
	
	// Reschedule our timer
	if (mod_timer(&timers, jiffies + msecs_to_jiffies(1000)))
		printk("Error in mod_timer\n");
}

static int __init ktimers_init(void)
{
	printk("My module worked!\n");
	
	// Create the proc interface
	proc_create("killme", 0, NULL, &proc_file_fops);
	 
	// Associate the timer callback with the timer list.
	setup_timer(&timers, TimerCallback, 0);

	printk("starting timer every 1000ms (%ld)\n", jiffies);
	 
	if (mod_timer(&timers, jiffies + msecs_to_jiffies(1000)))
		printk("Error in mod_timer\n");

	return 0;
}

static void __exit ktimers_exit(void)
{
	printk("Unloading my module.\n");
	if (del_timer(&timers))
		printk("Timers are still in use...\n");
		
	remove_proc_entry("killme", NULL);
	return;
}

module_init(ktimers_init);
module_exit(ktimers_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Justin T. Crawford <Justasic@gmail.com>");
MODULE_DESCRIPTION("Provides kernel-based userspace timer API");
