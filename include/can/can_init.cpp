/**\file
 *
 * can_init.cpp
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 26, 2019
 */

#include <stdlib.h>
#include <setjmp.h>
#include <sys/neutrino.h>
#include "utils/common.h"
#include "utils/sys.h"
#include "can_man.h"
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#undef DO_TRACE


/** Process and respond to interrupt events from the CAN card.
 *
 * When pulse is received by the resource manager as a result of the
 * InterruptAttachEvent in pulse_init the device-specific routine
 * can_dev->interrupt() is called to reset any interrupt registers, etc.
 *
 * Furthermore, any event registered by the client with can_arm is delivered to
 * the client, who will then do a read to get the data that has been copied into
 * the message buffer.
 *
 * @param ctp
 * 		dummy variable
 * @param code
 * 		dummy variable
 * @param flags
 * 		dummy variable
 * @param ptr
 * 		pointer to the device attributes object
 * @return
 * 		0 for success, or -1 if an error occurs
 */
int can_handle_interrupt(message_context_t *ctp, int code, unsigned flags,
		void *ptr) {
	int status;
	int mask_count;
	int is_recv = 0;	// set to 1 if interrupt is receive CAN
	can_attr_t *pattr = (can_attr_t *) ptr;
	can_info_t *pinfo = &pattr->can_info;
	can_ocb_t *pocb = pattr->notify_pocb;

#ifdef DO_TRACE
        printf("enter can_handle_interrupt: pocb 0x%x\n", (unsigned int) pocb);
	fflush(stdout);
#endif

	is_recv = pattr->can_dev->interrupt(pattr->in_buff, pattr->out_buff,
			(pattr->can_info).filter);

#ifdef DO_TRACE
	printf("can_handle_interrupt: is_recv %d, pocb 0x%x\n",
		is_recv, (unsigned int) pocb);
	fflush(stdout);
#endif

	if (is_recv && pocb != NULL) {
		status = MsgDeliverEvent(pocb->rcvid, &pocb->clt_event);
#ifdef DO_TRACE
		printf("MsgDeliverEvent %d \n", pocb->rcvid);
		fflush(stdout);
#endif
		if (status == ERROR) {
			pattr->can_dev->can_notify_client_err++;
#ifdef DO_TRACE
			printf("Failed to deliver CAN client notify event\n");
#endif
		}
	}

	if ((mask_count = InterruptUnmask(pinfo->irq, pinfo->intr_id)) != 0)
		pattr->can_dev->mask_count_non_zero++;

#ifdef DO_TRACE
	printf("mask_count %d\n", mask_count);
	fflush(stdout);
#endif

	return EOK;
}


void pulse_init(dispatch_t *dpp, can_attr_t *pattr) {
	can_info_t *pinfo = &pattr->can_info;
	sigevent *pevent;
	int mask_count;

#ifdef DO_TRACE
	printf("pulse_init: irq %d\n", pinfo->irq);
	fflush(stdout);
#endif
	if (pinfo->irq != 0) {
		pevent = &pattr->hw_event;
		if (pulse_attach(dpp, MSG_FLAG_ALLOC_PULSE,
				0, can_handle_interrupt, pattr) == ERROR) {
			fprintf(stderr, "Unable to attach irq_handler pulse.\n");
			exit(EXIT_FAILURE);
		}
		if ((pevent->sigev_coid = message_connect(dpp, MSG_FLAG_SIDE_CHANNEL))
				== ERROR) {
			fprintf(stderr, "Unable to attach pulse to channel.\n");
			exit(EXIT_FAILURE);
		}
		pevent->sigev_notify = SIGEV_PULSE;
		pevent->sigev_priority = -1;
		pevent->sigev_value.sival_int = 0;
		if ((pinfo->intr_id = InterruptAttachEvent(pinfo->irq,
				 pevent, _NTO_INTR_FLAGS_TRK_MSK)) == -1) {
			perror("InterruptAttach");
		} else {
			printf("IRQ %d attached: ID %d\n", pinfo->irq,
				pinfo->intr_id);
		}
		fflush(stdout);
		if ((mask_count = InterruptUnmask(pinfo->irq, pinfo->intr_id)) != 0) {
			printf("mask count: %d\n", mask_count);
		}
	}
}
