/**\file
 *
 * can_man.c
 *
 * Resource manager for the SSV CAN board. FIXME
 *
 * Digital I/O on the board is also supported by the same devctls used by PATH
 * DAS (Data Acquisition System) drivers.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 26, 2019
 */

#include <can/can_man.h>
#include <iostream>
#include <sys/dispatch.h>
#include "can/can.h"
#include "utils/common.h"
#include <sys/iofunc.h>
#include <sys/dispatch.h>

using namespace std;

#define DAS_MSG_BUF     2048

#undef DO_TRACE
#define DO_TRACE

static resmgr_connect_funcs_t  	connect_func;
static resmgr_io_funcs_t	   	io_func;
static IOFUNC_ATTR_T		   	attr;
static iofunc_mount_t 			can_mount;
static iofunc_funcs_t 			can_mount_funcs;


/** External variables in ca_if.c, to be printed on exit */
extern int can_timeout_count;


IOFUNC_OCB_T *can_ocb_calloc(resmgr_context_t *ctp, iofunc_attr_t *attr) {
	return (IOFUNC_OCB_T *) calloc(1, sizeof(IOFUNC_OCB_T));
}


void can_ocb_free(IOFUNC_OCB_T *pocb) {
	delete pocb;
}


int main (int argc, char **argv) {
	bool verbose = false;

	dispatch_t *dpp;
	resmgr_attr_t resmgr_attr;
	dispatch_context_t *ctp;
	can_info_t *pinfo = &attr.can_info;

	ThreadCtl(_NTO_TCTL_IO, NULL);  /* required to access I/O ports */

	/* Create the dispatch structure. */
	if ((dpp = dispatch_create()) == NULL) {
		perror ("Unable to dispatch_create\n");
		exit (EXIT_FAILURE);
	}

	/* Initialize the various data structures. */
	memset(&resmgr_attr, 0, sizeof (resmgr_attr));
	resmgr_attr.nparts_max = 2;
	resmgr_attr.msg_max_size = DAS_MSG_BUF;

	/* Bind default functions into the outcall tables. */
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &connect_func,
		_RESMGR_IO_NFUNCS, &io_func);
	iofunc_attr_init(&attr.io_attr, S_IFNAM | 0666, 0, 0);

	/* Add allocation and deallocation for extended OCB. */
	can_mount_funcs.nfuncs = _IOFUNC_NFUNCS;
	can_mount_funcs.ocb_calloc = can_ocb_calloc;
	can_mount_funcs.ocb_free = can_ocb_free;
	memset(&can_mount, 0, sizeof(can_mount));
	can_mount.funcs = &can_mount_funcs;
	attr.io_attr.mount = &can_mount;

	/* Read configuration files and bind device-specific functions. */
	can_init(argc, argv, &connect_func, &io_func, &attr);

	/* Establish a name in the pathname space. */
	if (resmgr_attach(dpp, &resmgr_attr, attr.devname, _FTYPE_ANY,
			0, &connect_func, &io_func, (_iofunc_attr*)&attr) == -1) {  // FIXME: _iofunc_attr*
		perror ("Unable to resmgr_attach\n");
		exit (EXIT_FAILURE);
	}

	if (verbose) {
		printf("Calling can_dev_init: base address 0x%x speed %d extended %d\n",
			pinfo->port, pinfo->bit_speed, pinfo->use_extended_frame);
		fflush(stdout);
	}

	/* Initialize device. */
	CANDeviceManager can_dev;
	can_dev.init(pinfo->port, pinfo->bit_speed, pinfo->use_extended_frame);
	attr.can_dev = &can_dev;

	/* Initialize all classes within the attr object. */
	attr.in_buff = new CircularBuffer();
	attr.out_buff = new CircularBuffer();

	if (verbose) {
		printf("Attaching pulses\n");
		fflush(stdout);
	}

	/* Attach pulses and interrupt event. */
	pulse_init(dpp, &attr);

	/* Allocate dispatch context. */
	ctp = dispatch_context_alloc(dpp);

	if(setjmp(exit_env) != 0) {
		can_err_count_t err = can_dev.get_errs();
		printf("%s exiting\n", argv[0]);
		printf("can_notify_client_err %d\n", can_dev.can_notify_client_err);
		printf("mask_count_non_zero %d\n", can_dev.mask_count_non_zero);
		printf("shadow_buffer_count %d\n", err.shadow_buffer_count);
		printf("intr_in_handler_count %d\n", err.intr_in_handler_count);
		printf("rx_interrupt_count %d\n", err.rx_interrupt_count);
		printf("rx_message_lost_count %d\n", err.rx_message_lost_count);
		printf("tx_interrupt_count %d\n", err.tx_interrupt_count);
		printf("tx_buffer_flush %d\n", can_dev.tx_buffer_flush);
		printf("can_timeout_count %d\n", can_dev.can_timeout_count);
		fflush(stdout);
		exit(EXIT_SUCCESS);
	} else {
		sig_ign(sig_list, sig_hand);
	}

	/* Wait here forever, handling messages. */
	while (1) {
		if ((ctp = dispatch_block(ctp)) == NULL) {
			perror("Unable to dispatch_block\n");
			exit(EXIT_FAILURE);
		}
		dispatch_handler(ctp);
	}

	/* free up memory */
	delete attr.in_buff;
	delete attr.out_buff;
	delete attr.can_dev;
}
