/**\file
 * can_dev.h
 *
 * Structures, definitions, and prototypes for the SSV CAN driver
 *
 *  Created on: Feb 19, 2019
 *      Author: aboudy
 */

#ifndef INCLUDE_JBUS_CAN_DEV_H_
#define INCLUDE_JBUS_CAN_DEV_H_

#include "das_clt.h"
#include "can_struct.h"
#include <sys/iomsg.h>
#include <sys/iofunc.h>

/** Defined in can_init.c */
extern int can_notify_client_err;
extern int mask_count_non_zero;

/** Defined in can_dev.c */
extern unsigned int shadow_buffer_count;
extern unsigned int intr_in_handler_count;
extern unsigned int rx_interrupt_count;
extern unsigned int rx_message_lost_count;
extern unsigned int tx_interrupt_count;


/* -------------------------------------------------------------------------- */
/* ---------------------- Implemented in can_init.cpp ----------------------- */
/* -------------------------------------------------------------------------- */

extern int can_handle_interrupt(message_context_t *ctp, int code,
			unsigned flags, void *ptr);
extern void pulse_init(dispatch_t *dpp, IOFUNC_ATTR_T *pattr);


/* -------------------------------------------------------------------------- */
/* ----------------------- Implemented in can_if.cpp ------------------------ */
/* -------------------------------------------------------------------------- */

extern void can_cq_add(cbuff_typ *pbuff, can_msg_t *new_msg);
extern can_msg_t *can_cq_read_first(cbuff_typ *pbuff);
extern can_msg_t *can_cq_pop_first(cbuff_typ *pbuff);
extern void can_new_msg(can_msg_t *pmsg, IOFUNC_ATTR_T *pattr);


/* -------------------------------------------------------------------------- */
/* ----------------------- Implemented in io_func.cpp ----------------------- */
/* -------------------------------------------------------------------------- */

extern int io_devctl(resmgr_context_t *ctp, io_devctl_t *msg,
	iofunc_ocb_t *io_ocb);  // FIXME
//	can_ocb_t *ocb);  // FIXME: RESMGR_OCB_T

extern int io_open(resmgr_context_t *ctp, io_open_t *msg,
	RESMGR_HANDLE_T *handle, void *extra);


/* -------------------------------------------------------------------------- */
/* ----------------------- Implemented in can_dev.cpp ----------------------- */
/* -------------------------------------------------------------------------- */

extern can_msg_t can_dev_read(IOFUNC_ATTR_T *pattr);

extern int can_dev_write(can_ocb_t *pocb, can_msg_t *pmsg);

extern int can_dev_empty_q(IOFUNC_ATTR_T *pattr);

extern int can_dev_arm(resmgr_context_t *ctp, can_ocb_t *pocb,
		sigevent event);

int can_dev_add_filter(IOFUNC_ATTR_T *pattr, can_filter_t filter);

extern can_err_count_t can_dev_clear_errs();

extern can_err_count_t can_dev_get_errs();

extern void can_dev_init(unsigned int base_address, unsigned int bit_speed,
		unsigned char extended_frame);

extern int can_dev_interrupt(IOFUNC_ATTR_T *pattr);

extern void can_send(IOFUNC_ATTR_T *pattr);

extern int digital_dir(int port, long bits);

extern int digital_in(int port, long *pbits);

extern int digital_out(int port, long bits, long mask, long *pold_bits,
		long *pnew_bits);


#endif /* INCLUDE_JBUS_CAN_DEV_H_ */
