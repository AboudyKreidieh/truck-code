/**\file
 *
 * das_clt.h
 *
 *	Definitions common to *
 *  client and DAS I/O management server.
 *	Includes devctl definitions for the CAN driver.
 *
 *	Rewritten for QNX6 because of message passing changes.
 *	QNX4 version last modified in 1998.
 *
 * Copyright (c) 2003   Regents of the University of California
 *
 *
 *	static char rcsid[] = "$Id";
 *
 *	$Log: das_clt.h,v $
 *	Revision 1.3  2004/11/01 20:43:27  dickey
 *	Bugs found and changes made while doing CAN driver.
 *
 *	Modified Files:
 *	 	das_clt.h sys_das.c
 *
 *	Revision 1.2  2004/10/29 02:33:29  dickey
 *	Made changes to Tilcon utilities, and minor changes to other header
 *	files. Crow's Landing version of veh_utils.cpp.
 *
 *	Modified Files:
 *	 	das_clt.h path_packet.h tilcon_utils.cpp tilcon_utils.h
 *	 	veh_utils.cpp
 *
 *	Revision 1.1.1.1  2004/08/26 23:45:03  dickey
 *	local for IDS, CCW, CACC, etc.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date July 15, 2019
 */

#ifndef SRC_JBUS_DAS_CLT_H_
#define SRC_JBUS_DAS_CLT_H_

#include <devctl.h>


#define DAS_MSG_BUF	2048
#define MIN_SAMPLE_TICK	1
#define MAX_SAMPLE_TICK	0xffff
#define NUM_SCAN_ENT	96		/// Number of scan table entries.	*/
#define DAS_MAX_TIMERS	10

/*	das_info structure for das_get_info()
 */

typedef struct
{
	int		port;		/// Base address of adapter.
	int		irq;		/// Interrupt request line.
	int		num_ai;		/// 8/16 if differential/single ended.
	float		ad_min;		/// Input range.
	float		ad_max;
	float		ad_step;
	float		da_min;		/// Output range.
	float		da_max;
	float		da_step;
	long		ad_ticks;
	unsigned long	pace_clk;	/// pacer clock freq. in Hz.
} das_info_typ;


/**	Set sample rate.
 */
typedef struct
{
	short int chnl;
	unsigned long ticks;
} das_sample_t;

/**	Digital to analog sync
 */
typedef struct
{
	short int chnl;
	float data;
} das_da_sync_t;


/**	Digital input.
 */

typedef struct
{
	short int		port;	/// port number
	long			bits;	/// data returned from port
} das_din_t;


/**	Digital output.
 */

typedef struct
{
	short int port;	/// port number
	long mask;	/// active bits set
	long bits;	/// data sent to port (only active bits to be changed)
	long old_bits;	/// previous value at device
	long new_bits;	/// value after change
} das_dout_t;

/**	Set timer mode.
 */

typedef struct
{
	unsigned		timer;
	unsigned		mode;
	unsigned		value;
} das_tmr_mode_t;

/** CAN driver devctls are included here as well, because the SSV CAN
 *  board also has digital I/O and supports the DAS_DIG devctls
 */
enum {
	DAS_QUERY,
	DAS_DA_SYNC,
	DAS_DA_TERM,
	DAS_DIG_DIR,
	DAS_DIG_OUT,
	DAS_DIG_IN,
	DAS_AD_ENQ,
	DAS_AD_TERM,
	DAS_AD_SET_SMP,
	DAS_TMR_MODE,
	DAS_TEST_PULSE,
	CAN_FILTER,
	CAN_ARM,
	CAN_I82527_READ,
	CAN_I82527_WRITE,
	CAN_EMPTY_Q,
	CAN_GET_ERRS,
	CAN_CLEAR_ERRS,
};

/** _DCMD_DAS definition must be compatible with ranges already reserved
 * in /usr/include/devctl.h and /usr/include/sys/dcmd*.h
 * CAN driver devctls use same range, because SSV CAN board also has
 * digital I/O.
 */
#define _DCMD_DAS	0x0f00

#define DCMD_DAS_QUERY	 __DIOF(_DCMD_DAS, DAS_QUERY, das_info_typ)
#define DCMD_DAS_DA_SYNC __DIOT(_DCMD_DAS, DAS_DA_SYNC, das_da_sync_t)
#define DCMD_DAS_DA_TERM __DIOT(_DCMD_DAS, DAS_DA_TERM, int)
#define DCMD_DAS_DIG_DIR __DIOT(_DCMD_DAS, DAS_DIG_DIR, long)
#define DCMD_DAS_DIG_OUT __DIOTF(_DCMD_DAS, DAS_DIG_OUT, das_dout_t)
#define DCMD_DAS_DIG_IN	 __DIOTF(_DCMD_DAS, DAS_DIG_IN, das_din_t)
#define DCMD_DAS_AD_ENQ	 __DIOT(_DCMD_DAS, DAS_AD_ENQ, struct sigevent)
#define DCMD_DAS_AD_TERM __DIOT(_DCMD_DAS, DAS_AD_TERM, int)
#define DCMD_DAS_AD_SET_SMP __DIOT(_DCMD_DAS, DAS_AD_SET_SMP, das_sample_t)
#define DCMD_DAS_TMR_MODE __DIOT(_DCMD_DAS, DAS_TMR_MODE, das_tmr_mode_t)
#define DCMD_DAS_TEST_PULSE __DIOT(_DCMD_DAS, DAS_TEST_PULSE, int)
#define DCMD_CAN_I82527_READ   __DIOF(_DCMD_DAS, CAN_I82527_READ, can_msg_t)
#define DCMD_CAN_I82527_WRITE __DIOT(_DCMD_DAS, CAN_I82527_WRITE, can_msg_t)
#define DCMD_CAN_ARM __DIOT(_DCMD_DAS, CAN_ARM, struct sigevent)
#define DCMD_CAN_FILTER __DIOT(_DCMD_DAS, CAN_FILTER, can_filter_t)
#define DCMD_CAN_EMPTY_Q __DIOTF(_DCMD_DAS, CAN_EMPTY_Q, int)
#define DCMD_CAN_GET_ERRS __DIOTF(_DCMD_DAS, CAN_GET_ERRS, can_err_count_t)
#define DCMD_CAN_CLEAR_ERRS __DIOTF(_DCMD_DAS, CAN_CLEAR_ERRS, can_err_count_t)

/** _IOMGR_DAS is a private definition, see sys/iomgr.h
 *  IOMSG_DAS subtype values are also private
 */
#define _IOMGR_DAS	0xff00

enum {
	IOMSG_DAS_AD_READ,
	IOMSG_DAS_AD_SET_SCAN,
	IOMSG_DAS_TMR_SCAN,
	IOMSG_DAS_TMR_READ,
};

#endif /* SRC_JBUS_DAS_CLT_H_ */
