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


/* declared and set in can_dev.c by can_send */  // TODO: remove globals

extern time_t last_time_can_sent;
int can_timeout_count = 0;
int tx_buffer_flush = 0;


///** Print routine for debugging can_msg_t */
//void print_can_msg(can_msg_t *pc) {
//	int i;
//	printf("0x%08x ", (unsigned int) pc->id);
//	printf("%d ", pc->size);
//	for (i = 0; i < 8; i++)
//		printf("0x%02x ", pc->data[i]);
//	printf("%d\n", pc->error);
//}


void can_new_msg(can_msg_t *pmsg, IOFUNC_ATTR_T *pattr) {
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


can_msg_t can_dev_read(IOFUNC_ATTR_T *pattr) {
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
	return msg;
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
		can_dev_send(pattr);
	else {
		// Assumes message sends are at least 1 Hz
		if ((time((time_t *)NULL) - last_time_can_sent) > 1) {
			can_timeout_count++;
			can_dev_send(pattr);
		}

		if (pattr->out_buff.data_count > 10) {
			// must have missed an interrupt, throw old stuff out
			tx_buffer_flush += can_dev_empty_q(pattr);
		}
	}

	return EOK;

}


int can_dev_arm(resmgr_context_t *ctp, iofunc_ocb_t *io_ocb, sigevent event)
{
	can_ocb_t *pocb = new can_ocb_t();
	pocb->rcvid = ctp->rcvid;
	pocb->clt_event = event;

	IOFUNC_ATTR_T *pattr = (IOFUNC_ATTR_T *)io_ocb->attr;
	pattr->notify_pocb = pocb;

	return EOK;
}


int can_dev_add_filter(IOFUNC_ATTR_T *pattr, can_filter_t filter) {
	pattr->can_info.filter = filter;
	return EOK;
}
