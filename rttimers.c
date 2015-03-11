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
#include <linux/config.h>
#include <linux/init.h>

static int __init mymodule_init(void)
{
		 printk ("My module worked!\n");
		 return 0;
}

static void __exit mymodule_exit(void)
{
		 printk ("Unloading my module.\n");
		 return;
}

module_init(mymodule_init);
module_exit(mymodule_exit);

MODULE_LICENSE("GPL");

