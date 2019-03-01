/*
 * can_if.cpp
 *
 * Interface to the CAN hardware functions provided by SSV
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 24, 2019
 */

#include "can_man.h"
#include "can_struct.h"
#include <devctl.h>

#undef DO_TRACE

/** declared and set in can_dev.c by can_send
 */
extern time_t last_time_can_sent;
int can_timeout_count = 0;
int tx_buffer_flush = 0;

/** Utilities to handle the circular queue of output messages
 */
void can_cq_add(cbuff_typ *pbuff, can_msg_t *new_msg)
{
	int index = get_circular_index(pbuff);
	can_msg_t *pdata = (can_msg_t *) pbuff->data_array;
#ifdef DO_TRACE
	printf("cq add data 0x%02x to buff 0x%x at index %d\n",
		new_msg->data[0], (unsigned int) pbuff, index);
#endif
	pdata[index] = *new_msg;
}

can_msg_t *can_cq_read_first(cbuff_typ *pbuff)
{
	can_msg_t *pdata = (can_msg_t *) pbuff->data_array;
	return (&pdata[pbuff->data_start]);
}

can_msg_t *can_cq_pop_first(cbuff_typ *pbuff)
{
	can_msg_t *pdata = (can_msg_t *) pbuff->data_array;
#ifdef DO_TRACE
	printf("cq pop from buff 0x%x, start %d, count %d\n",
		(unsigned int) pbuff,pbuff->data_start, pbuff->data_count);
#endif
	if (pbuff->data_count > 0) {
		can_msg_t *rtnval = &pdata[pbuff->data_start];
		pbuff->data_count--;
		pbuff->data_start++;
		if (pbuff->data_start == pbuff->data_size)
			pbuff->data_start = 0;
		return (rtnval);
	}
	return (NULL);
}

/**
 *	Print routine for debugging can_msg_t
 */
void print_can_msg(can_msg_t *pc)
{
	int i;
	printf("0x%08x ", (unsigned int) pc->id);
	printf("%d ", pc->size);
	for (i = 0; i < 8; i++)
		printf("0x%02x ", pc->data[i]);
	printf("%d\n", pc->error);
}

/** Adds a new message if ID and MASK allow
 */

void can_new_msg(can_msg_t *pmsg, IOFUNC_ATTR_T *pattr)
{
	can_info_t *pinfo = &pattr->can_info;
	unsigned long id = pinfo->filter.id;
	unsigned long mask = pinfo->filter.mask;

#ifdef DO_TRACE
	printf("can_new_msg: id 0x%08x mask 0x%08x\n", pmsg->id,
			(unsigned int) pinfo->filter.mask);
	fflush(stdout);
#endif

	if((id & mask) == ((pmsg->id) & mask)) {
		can_cq_add(&pattr->in_buff, pmsg);
		return;
	}
}

can_msg_t can_dev_read(IOFUNC_ATTR_T *pattr)
{
	can_msg_t msg;
	can_msg_t *pmsg;

	memset(&msg, 0, sizeof(can_msg_t));

	pmsg = can_cq_pop_first(&pattr->in_buff);

#ifdef DO_TRACE
	printf("can_dev_read 0x%x\n", (unsigned int) pmsg);
#endif
	if (pmsg == NULL)
		msg.error = 1;
	else {
		msg = *pmsg;
		msg.error = 0;
	}
#ifdef DO_TRACE
	print_can_msg(&msg);
	fflush(stdout);
#endif
	return (msg);
}

/**
 *	Used to throw out buffered output when sending gets
 *	too far behind.
 */
int can_dev_flush_q(IOFUNC_ATTR_T *pattr)
{
	int j = 0;
	can_msg_t *dummy = can_cq_pop_first(&pattr->out_buff);
	while (pattr->out_buff.data_count > 1) {
		dummy = can_cq_pop_first(&pattr->out_buff);
		j++;
	}
	return(j);
}

int can_dev_write(can_ocb_t *pocb, can_msg_t *pmsg)  // FIXME: RESMGR_OCB_T *pocb
{
#ifdef DO_TRACE_TX
	int j;
#endif
	can_attr_t *pattr = (can_attr_t*) pocb->io_ocb.attr;

	pmsg->error = 0;
#ifdef DO_TRACE
	printf("can_dev_write: buff 0x%x, msg 0x%x\n",
			(unsigned int) pattr->out_buff.data_array,
			(unsigned int) pmsg);
	print_can_msg(pmsg);
	fflush(stdout);
#endif
	can_cq_add(&pattr->out_buff, pmsg);

#ifdef DO_TRACE_TX
	printf("write id 0x%x ", (unsigned int) pmsg->id);
	for (j = 0; j < pmsg->size; j++)
		printf(" %d", pmsg->data[j]);
	printf("\n");
	printf("data count: %d\n", pattr->out_buff.data_count);
	fflush(stdout);
#endif

	if(pattr->out_buff.data_count == 1)
		can_send(pattr);
	else {
		// Assumes message sends are at least 1 Hz
		if ((time((time_t *)NULL) - last_time_can_sent) > 1) {
			can_timeout_count++;
			can_send(pattr);
		}

		if (pattr->out_buff.data_count > 10) {
			// must have missed an interrupt, throw old stuff out

			tx_buffer_flush += can_dev_flush_q(pattr);
		}

	}
#ifdef DO_TRACE_TX
	printf("returning from can_dev_write\n");
#endif
	return (EOK);

}


/**
 *	Used to throw out buffered input when a receiver first
 *	connects to the driver.
 */
int can_dev_empty_q(IOFUNC_ATTR_T *pattr)
{
	int j = 0;
	can_msg_t *dummy = can_cq_pop_first(&pattr->in_buff);
	while (dummy != NULL) {
		j++;
		dummy = can_cq_pop_first(&pattr->in_buff);
	}
	return(j);
}

/**
 *	Attach the hardware interrupt, and save the event to be
 *	used to notify the client in the ocb structure.
 */
int can_dev_arm(resmgr_context_t *ctp, can_ocb_t *pocb, sigevent event)  // FIXME: RESMGR_OCB_T *pocb
{
	IOFUNC_ATTR_T *pattr = (IOFUNC_ATTR_T *)pocb->io_ocb.attr;

	pocb->rcvid = ctp->rcvid;
	pocb->clt_event = event;
	pattr->notify_pocb = pocb;

	return EOK;
}

/**
 *	Not implemented yet at the level of the device registers
 *	All are received in Object 15. Later this function may
 *      include calls to device register operations.
 */

int can_dev_add_filter(IOFUNC_ATTR_T *pattr, can_filter_t filter)
{
	pattr->can_info.filter = filter;
	return EOK;
}
