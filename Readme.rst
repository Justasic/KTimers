Real-Time Kernel-Space Timers
=============================

Timers inside the kernel for userspace use!

Introduction
------------

The purpose of this project is to provide realtime timers for user space applications in a
very slow environment where the linux CPU Governor and I/O Scheduler are mostly slammed at
all times preventing things like my alarm app on my android phone from being allocated
CPU time to wake me up for work on time because Facebook wants to spend 4 years checking
for new messages and notifications on the picture of pie I posted the night before.

It is also my first serious attempt at making something with the linux kernel hence it's
near uselessness ;)

The idea
--------

The idea is that the program (our alarm app in this case) registers a timer with the linux kernel
api and much like the way EPoll works, it yeilds it's CPU time to allow other processes (like facebook)
to run. When the timer expires (checked each time the kernel ticks) it will forcefully allocate CPU time
for the registered application, returning from our wait call and allowing the alarm app to run on time
regardless of the other applications being scheduled. This overrules the "sharing is caring" CPU governor
style within the linux kernel and allows the application to run on time so I can get to work on time.


How to build?
-------------

Make sure you have the linux kernel (duh) then run `make`



License
=======

GNU GPL v2 (see COPYING)
