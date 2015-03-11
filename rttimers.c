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
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>

static struct timer_list timers;

void TimerCallback(unsigned long data)
{
		printk("Timer callback in %ld jiffies!\n", jiffies);
}

static int __init ktimers_init(void)
{
		 printk("My module worked!\n");
		 
		 // Associate the timer callback with the timer list.
		 setup_timer(&timers, TimerCallback, 0);

		 printk("starting timer every 200ms (%ld)\n", jiffies);
		 
		 if (mod_timer(&timers, jiffies + msecs_to_jiffies(200)))
				 printk("Error in mod_timer\n");

		 return 0;
}

static void __exit ktimers_exit(void)
{
		 printk("Unloading my module.\n");
		 if (del_timer(&timers))
				 printk("Timers are still in use...\n");
		 return;
}

module_init(ktimers_init);
module_exit(ktimers_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Justin T. Crawford <Justasic@gmail.com>");
MODULE_DESCRIPTION("Provides kernel-based userspace timer API");
