/**\file
 *
 * delay.h
 *
 * Implement Linux udelay function with nanosleep; not sure if the driver needs
 * this to be uninterruptible?
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date April 6, 2019
 */

#ifndef INCLUDE_CAN_DELAY_H_
#define INCLUDE_CAN_DELAY_H_


#include "utils/timestamp.h"
#include <sys/time.h>			/* timeval */


/** Use on QNX to get microsecond delay available on Linux. */
static inline unsigned int udelay(unsigned int usec)
{
	timespec requested, remaining;
    requested.tv_sec = usec/1000000;
    requested.tv_nsec = usec*1000;
    return ((unsigned int) nanosleep(&requested, &remaining));
}


/** Add a definition for linux/time.h's do_gettimeofday in terms of gettimeofday
 * on QNX6. */
static inline void do_gettimeofday(timeval *tv)
{
	gettimeofday(tv, NULL);
}


/**
 *  Also throw the linux-style atomic_t in here, taken
 * from /usr/include/asm-i386/atomic.h, from
 * Linux version 2.6.21.5-smp (root@midas)
 * (gcc version 4.1.2) #2 SMP Tue Jun 19 14 :58:11 CDT 2007
 */
typedef struct {
	int counter;
} atomic_t;


/** Read atomic variable. Atomically reads the value of @v.
 *
 * @param v  pointer of type atomic_t
 */
#define atomic_read(v)          ((v)->counter)


/** Set atomic variable.
 *
 * Atomically sets the value of @v to @i.
 *
 * @param v pointer of type atomic_t
 * @param i required value
 */
#define atomic_set(v,i)         (v->counter = i)


#endif /* INCLUDE_CAN_DELAY_H_ */
