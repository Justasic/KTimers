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
#include <linux/jiffies.h> // for global variable `jiffies'!
#include <linux/signal.h>  // for force_sigsegv()
#include <linux/sched.h>   // for wake_up_process() and find_task_by_pid()
#include <linux/time.h>    // for various time-related functions

// For PROC
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <linux/seq_file.h>

static struct timer_list timers;
// Because I like the libc-style time api.
#define time(unused) (CURRENT_TIME.tv_sec)
// The check interval for all timers
#define KERNEL_TIMER_FREQ 1000

// A structure to keep track of all the processes registering timers with us.
typedef struct ProcessTimer_s
{
	// The task's PID, we do this instead of a task_struct to prevent
	// us from referencing a location which no longer exists.
	pid_t pid;	
	// The timepoint the task will expire.
	__kernel_time_t expire;
	// The next timer in the list
	struct ProcessTimer_s *next;
} ProcessTimer_t;

// The start of the process-timer list.
static ProcessTimer_t *head = NULL;

static int read_callback(struct seq_file *m, void *v)
{
	struct task_struct *task = current;
	
	seq_printf(m, "Your PID is %d!\n", task->pid);
	seq_printf(m, "Current unix timestamp is: %ld\n", time(NULL));
	return 0;
}

static int write_callback(struct inode *inode, struct file *file)
{
	// use valloc and task_pid
	return 0;
}

static int open_callback(struct inode *inode, struct  file *file)
{
	// Get the task of the process opening the file.
	//struct task_struct *task = current;
	
	// Kill said task. -- this was just to test and make sure shit works.
	//kill_pid(task_pid(task), SIGSEGV, 1);
	
	return single_open(file, read_callback, NULL);
}

struct proc_dir_entry *proc_file_entry;
static const struct file_operations proc_file_fops = {
	.owner   = THIS_MODULE,
	.open    = open_callback,
	//.write   = write_callback,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = single_release,
};

void TimerCallback(unsigned long data)
{
	ProcessTimer_t *cur, *prev;
	printk("Timer callback in %ld jiffies!\n", jiffies);
	
	// Basically, here we find the `struct task_struct` object
	// of the process we need to talk to and call `wake_up_process`
	// to schedule it next. This is the best way to make the kernel
	// schedule shit.

	for (cur = head, prev = NULL; cur;)
	{
		// Find our task.
		struct task_struct *task = find_task_by_vpid(cur->pid);
		
		// No task found? must have been deallocated (aka, process terminated)
		// we must remove it from our timer list.
		if (!task)
		{
			// Remove the task from the list.
			if (!prev) // we're the start of the list.
			{
				// Set our head.
				head = cur->next;
				// Deallocate our list item.
				vfree(cur);
				// Set the cursor.
				cur = head;
				// Process next item in the list.
				continue;
			}
			else // We're not the first element.
			{
				// Adjust the list.
				prev->next = cur->next;
				// Deallocate our item.
				vfree(cur);
				// Set cursor.
				cur = prev->next;
				// Process next item in list.
				continue;
			}
		}
		
		// Check if we have expired timers.
		if (time(NULL) >= cur->expire)
		{
			// We found a process still running and it has an expired timer, schedule it immediately.
			// TODO: Tell the process which timer expired.
			wake_up_process(task);
		}
		
		// Set our cursors.
		prev = cur;
		cur = cur->next;
	}
	
	// Reschedule our kernel timer for the next check of user-space timers.
	if (mod_timer(&timers, jiffies + msecs_to_jiffies(KERNEL_TIMER_FREQ)))
		printk("Error in mod_timer\n");
}

static int __init ktimers_init(void)
{
	printk("Loaded KTimers\n");
	
	// Create the proc interface
	proc_create("killme", 0, NULL, &proc_file_fops);
	 
	// Associate the timer callback with the timer list.
	setup_timer(&timers, TimerCallback, 0);

	printk("starting timer every %dms (%ld)\n", KERNEL_TIMER_FREQ, jiffies);
	 
	if (mod_timer(&timers, jiffies + msecs_to_jiffies(KERNEL_TIMER_FREQ)))
		printk("Error in mod_timer\n");

	return 0;
}

static void __exit ktimers_exit(void)
{
	ProcessTimer_t *cur = head;
	
	printk("Unloading KTimers\n");
	if (del_timer(&timers))
		printk("Timers are still in use...\n");
		
	remove_proc_entry("killme", NULL);
	
	// Delete all the virtual memory.
	for (; cur; cur = cur->next)
	{
		static ProcessTimer_t *prev;
		// Free the previous process instance
		if (prev)
			vfree(prev);
			
		prev = cur;
	}
}

// Register our module init and exit handlers
module_init(ktimers_init);
module_exit(ktimers_exit);

// Register various module information
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Justin T. Crawford <Justasic@gmail.com>");
MODULE_DESCRIPTION("Provides kernel-based userspace timer API");
