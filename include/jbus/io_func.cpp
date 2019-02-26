/**\file
 * io_func.c
 *
 * Replacement functions for the default iofunc functions in the DAS resource
 * manager.
 */

#include "can_struct.h"
#include "can_dev.h"
#include <devctl.h>
#include <sys/iofunc.h>

#undef DO_TRACE

//int io_devctl (resmgr_context_t *ctp, io_devctl_t *msg, can_ocb_t *ocb)
int io_devctl (resmgr_context_t *ctp, io_devctl_t *msg, iofunc_ocb_t *io_ocb)
{
//	can_ocb_t *ocb = (can_ocb_t*) ocb_res;
	int status;
	int dcmd;
	void *data;
//	can_attr_t *pattr = (can_attr_t*) ocb->io_ocb.attr;  // FIXME: IOFUNC_ATTR_T
	can_attr_t *pattr = (can_attr_t*) io_ocb->attr;

	/* temporary variables for devctl data */
	sigevent event;
	can_msg_t can_msg;
	das_din_t *pdin;
	das_dout_t *pdout;
	can_filter_t filter;
	can_err_count_t *perrs;

	/// see if it's a standard POSIX-supported devctl()
//	if ((status = iofunc_devctl_default(ctp, msg, (iofunc_ocb_t*) ocb)) FIXME
	if ((status = iofunc_devctl_default(ctp, msg, io_ocb))
			!= _RESMGR_DEFAULT) {
		return status;
	}

	/** set up "data" to point to the data area after the devctl msg struct
	 * this pointer is the same for input message and output message type
	 * save dcmd field and clear message header when data is to be returned in
	 * the reply, ctp->iov is set up, otherwise no data is returned
	 */
	data = _DEVCTL_DATA(msg->i);
	dcmd = msg->i.dcmd;
	memset (&msg->o, 0, sizeof (msg->o));

	///  see which command it was, and act on it
	switch (dcmd) {
	case DCMD_CAN_I82527_READ:
		* (can_msg_t *) data = can_dev_read(pattr);
		msg->o.nbytes = sizeof(can_msg_t);
	        return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));

//	case DCMD_CAN_I82527_WRITE:
//		can_msg =  * (can_msg_t *) data;
//		return can_dev_write(ocb, &can_msg);

	case DCMD_CAN_ARM:
		event =  * (struct sigevent *) data;
		printf("io_devctl: can_dev_arm, fd %d\n", event.sigev_code);
#if 0
		if ((mask_count =
			InterruptUnmask(pinfo->irq, pinfo->intr_id)) != 0) {
			printf("mask count: %d\n", mask_count);
		}
#endif
//		return can_dev_arm(ctp, ocb, event);  FIXME

	case DCMD_CAN_FILTER:
		filter = * (can_filter_t *) data;
		printf("io_devctl: can_dev_arm, id %ld mask 0x%08x\n",
				filter.id, (unsigned int) filter.mask);
		return (can_dev_add_filter(pattr, filter));

	case DCMD_CAN_EMPTY_Q:
		* (int *) data = can_dev_empty_q(pattr);
		msg->o.nbytes = sizeof(int);
		return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));

        case DCMD_CAN_CLEAR_ERRS:
                perrs = (can_err_count_t *) data;
		*(perrs) = can_dev_clear_errs();
		memset(&msg->o, 0, sizeof(msg->o));
                msg->o.nbytes = sizeof(can_err_count_t);
                return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));

        case DCMD_CAN_GET_ERRS:
                perrs = (can_err_count_t *) data;
		*(perrs) = can_dev_get_errs();
		memset(&msg->o, 0, sizeof(msg->o));
                msg->o.nbytes = sizeof(can_err_count_t);
                return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));

	case DCMD_DAS_DIG_DIR:
		pdin = (das_din_t *) data;
		return (digital_dir(pdin->port, pdin->bits));

	case DCMD_DAS_DIG_IN:
		pdin = (das_din_t *) data;
#ifdef DO_TRACE
		printf("DCMD_DAS_DIG_IN: pdin->port %d\n", pdin->port);
#endif
		status = digital_in(pdin->port, &pdin->bits);
		msg->o.nbytes = sizeof(das_din_t);
	        return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));

	case DCMD_DAS_DIG_OUT:
		pdout = (das_dout_t *) data;
		status =digital_out(pdout->port, pdout->mask, pdout->bits,
				 &pdout->old_bits, &pdout->new_bits);
		msg->o.nbytes = sizeof(msg) + sizeof(das_dout_t);
#ifdef DO_TRACE
		printf("&msg->o 0x%lx, pdout 0x%lx\n", (long) &msg->o, (long) pdout);
		printf("DCMD_DAS_DIG_OUT: old 0x%lx, new 0x%lx\n",
				pdout->old_bits, pdout->new_bits);
#endif
	        return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));

	default:
		return (ENOSYS);
	}
}

int io_open(resmgr_context_t *ctp, io_open_t *msg,
	RESMGR_HANDLE_T *handle, void *extra)
{
	int retval;
	retval = iofunc_open_default(ctp, msg, handle, extra);
	return retval;
}

