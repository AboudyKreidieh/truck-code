/**\file
 *
 * das_clt.h
 *
 * Definitions common to client and DAS I/O management server. Includes devctl
 * definitions for the CAN driver.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date January 15, 2019
 */

#ifndef SRC_JBUS_DAS_CLT_H_
#define SRC_JBUS_DAS_CLT_H_

#include <devctl.h>


#define DAS_MSG_BUF	2048
#define MIN_SAMPLE_TICK	1
#define MAX_SAMPLE_TICK	0xffff
#define NUM_SCAN_ENT	96		/* Number of scan table entries. */
#define DAS_MAX_TIMERS	10


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
#define DCMD_DAS_DIG_OUT __DIOTF(_DCMD_DAS, DAS_DIG_OUT, int)	/* TODO: replace from das_dout_t */
#define DCMD_DAS_DIG_IN	 __DIOTF(_DCMD_DAS, DAS_DIG_IN, int)	/* TODO: replace from das_din_t */
#define DCMD_DAS_AD_ENQ	 __DIOT(_DCMD_DAS, DAS_AD_ENQ, sigevent)
#define DCMD_DAS_AD_TERM __DIOT(_DCMD_DAS, DAS_AD_TERM, int)
#define DCMD_DAS_AD_SET_SMP __DIOT(_DCMD_DAS, DAS_AD_SET_SMP, das_sample_t)
#define DCMD_DAS_TMR_MODE __DIOT(_DCMD_DAS, DAS_TMR_MODE, das_tmr_mode_t)
#define DCMD_DAS_TEST_PULSE __DIOT(_DCMD_DAS, DAS_TEST_PULSE, int)
#define DCMD_CAN_I82527_READ   __DIOF(_DCMD_DAS, CAN_I82527_READ, can_msg_t)
#define DCMD_CAN_I82527_WRITE __DIOT(_DCMD_DAS, CAN_I82527_WRITE, can_msg_t)
#define DCMD_CAN_ARM __DIOT(_DCMD_DAS, CAN_ARM, sigevent)
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
