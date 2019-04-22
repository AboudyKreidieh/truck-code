/**\file
 *
 * can_if.cpp
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 24, 2019
 */

#include "can_man.h"
#include <devctl.h>

#undef DO_TRACE


int can_dev_arm(resmgr_context_t *ctp, iofunc_ocb_t *io_ocb, sigevent event) {
	can_ocb_t *pocb = new can_ocb_t();
	pocb->rcvid = ctp->rcvid;
	pocb->clt_event = event;

	IOFUNC_ATTR_T *pattr = (IOFUNC_ATTR_T *)io_ocb->attr;
	pattr->notify_pocb = pocb;

	return EOK;
}
