/**\file
 *
 * io_func.cpp
 *
 * Replacement functions for the default iofunc functions in the DAS resource
 * manager.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 26, 2019
 */

#include "can_man.h"
#include <devctl.h>


int io_devctl(resmgr_context_t *ctp, io_devctl_t *msg, iofunc_ocb_t *io_ocb)
{
//	can_ocb_t *ocb = (can_ocb_t*) ocb_res;
	int status;
	int dcmd;
	void *data;
	IOFUNC_ATTR_T *pattr = (IOFUNC_ATTR_T*) io_ocb->attr;

	/* temporary variables for devctl data */
	sigevent event;
	can_filter_t filter;
	can_err_count_t *perrs;

	/* See if it's a standard POSIX-supported devctl() */
	if ((status = iofunc_devctl_default(ctp, msg, io_ocb)) != _RESMGR_DEFAULT)
		return status;

	/** set up "data" to point to the data area after the devctl msg struct
	 * this pointer is the same for input message and output message type
	 * save dcmd field and clear message header when data is to be returned in
	 * the reply, ctp->iov is set up, otherwise no data is returned
	 */
	data = _DEVCTL_DATA(msg->i);
	dcmd = msg->i.dcmd;
	memset(&msg->o, 0, sizeof(msg->o));

	/* Check which command it was, and act on it. */
	switch (dcmd) {
	case DCMD_CAN_ARM:
		event =  * (sigevent*) data;
		printf("io_devctl: can_dev_arm, fd %d\n", event.sigev_code);
		return can_dev_arm(ctp, io_ocb, event);

	case DCMD_CAN_FILTER:
		/* Not implemented yet at the level of the device registers. All are
		 * received in Object 15. Later this function may include calls to
		 * device register operations. */
		filter = * (can_filter_t *) data;
		pattr->can_info.filter = filter;
		return EOK;

	case DCMD_CAN_EMPTY_Q:
		pattr->in_buff->empty();
		msg->o.nbytes = sizeof(int);
		return _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes);

	case DCMD_CAN_CLEAR_ERRS:
		perrs = (can_err_count_t *) data;
		*(perrs) = pattr->can_dev->clear_errs();
		msg->o.nbytes = sizeof(can_err_count_t);
        return _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes);

	case DCMD_CAN_GET_ERRS:
        perrs = (can_err_count_t *) data;
		*(perrs) = pattr->can_dev->get_errs();
		msg->o.nbytes = sizeof(can_err_count_t);
        return _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes);

	case DCMD_DAS_DIG_DIR:
		printf(" Digital I/O is not supported\n");
	    return EOK;

	case DCMD_DAS_DIG_IN:
		printf(" Digital I/O is not supported\n");
		msg->o.nbytes = sizeof(int);
		return _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes);

	case DCMD_DAS_DIG_OUT:
		printf(" Digital I/O is not supported\n");
		msg->o.nbytes = sizeof(msg) + sizeof(int);
		return _RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes);

	default:
		return ENOSYS;
	}
}


int io_open(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle,
		void *extra) {
	int retval = iofunc_open_default(ctp, msg, handle, extra);
	return retval;
}

