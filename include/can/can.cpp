/**\file
 *
 * can.cpp
 *
 * Implements methods in can.h
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date January 14, 2019
 */

#include <iostream>
#include <csetjmp>
#include <string>
#include "can.h"
#include "can_man.h"
#include "jbus/j1939_struct.h"
#include "utils/sys.h"
#include "utils/common.h"
#include "das_clt.h"
#include <sys/types.h>
#include <sys/neutrino.h>
#include <sys/iomsg.h>
#include <stdio.h>
#include <unistd.h>
#include <devctl.h>
#include <errno.h>


void can_init(int argc, char *argv[], resmgr_connect_funcs_t *pconn,
	resmgr_io_funcs_t *pio, IOFUNC_ATTR_T *pattr)
{
	int opt;
	// These are set to 1 if specified by command line arguments
	int arg_port = 0;
	int arg_irq = 0;

	char *pconfig;
	FILE *pfile;
	can_info_t *pinfo = &pattr->can_info;

	// Setup default parameters.
	pconfig = (char*)DEFAULT_CONFIG;
	pattr->verbose_flag = false;
	pattr->devname = (char*)DEFAULT_DEVICE;
	pattr->notify_pocb = NULL;
	pinfo->use_extended_frame = 1;	// by default, use extended frame
	pinfo->irq = DEFAULT_IRQ;
	pinfo->port = DEFAULT_PORT;
	pinfo->filter.id = 0;			// set up to accept all messages from all
	pinfo->filter.mask = 0xff;
	pinfo->bit_speed = 250;

	// If arguments are specified, they override config file
	while((opt = getopt(argc, argv, "e:f:i:n:p:s:v?")) != EOF) {
		switch (opt) {
		case 'e':
			pinfo->use_extended_frame = atoi(optarg);
			break;
		case 'f':
			pconfig = strdup(optarg);
			break;
		case 'i':
			pinfo->irq = atoi(optarg);
			arg_irq = 1;
			break;
		case 'n':
			pattr->devname = strdup(optarg);
			break;
		case 'p':
			pinfo->port = strtol(optarg, (char**)NULL, 0);
			printf("I/O port for base address 0x%x\n", pinfo->port);
			arg_port = 1;
			break;
		case 's':
			pinfo->bit_speed = atoi(optarg);
			printf("Bit speed set to %d\n", pinfo->bit_speed);
			break;
		case 'v':
			pattr->verbose_flag = true;
			break;
		case '?':
			fprintf(stderr, "%s:\t-[pfv]\n", argv[0]);
			fprintf(stderr, "\t\tp\tPath name (%s).\n", DEFAULT_DEVICE);
			fprintf(stderr, "\t\tf\tConfiguration file (%s).\n", DEFAULT_CONFIG);
			fprintf(stderr, "\t\tv\tVerbose mode.\n");
			fprintf(stderr, "\t\t?\tPrints this message.\n");
			exit(EXIT_SUCCESS);
		default:
			break;
		}
	}

	/* Initialize from config file, if found. */
	if ((pfile = get_ini_section(pconfig, pattr->devname)) == NULL) {
		printf("No section %s in %s file found, using args, defaults\n",
			pattr->devname, pconfig);
		fflush(stdout);
	} else {
		if (!arg_irq)
			pinfo->irq = get_ini_long(pfile, INI_IRQ_ENTRY, DEFAULT_IRQ);
		if (!arg_port)
			pinfo->port = get_ini_hex(pfile, INI_PORT_ENTRY, DEFAULT_PORT);
		fclose(pfile);
	}

	/* Initialize circular buffers for CAN read and write. */
	pattr->in_buff->init(DEFAULT_QSIZE, sizeof(can_msg_t));
	pattr->out_buff->init(DEFAULT_QSIZE, sizeof(can_msg_t));
	printf("Circular buffers initialized\n");
	fflush(stdout);

	/* Initialize resource manager function tables with CAN specific function
	 * for devctl. */
	pio->devctl = io_devctl;

	/* Initialize resource manager function tables with CAN specific function
	 * for open. */
	pconn->open = io_open;

	/* Set up exit environment for kill signals. */
	if (setjmp(exit_env) != 0) {
		exit(EXIT_SUCCESS);
	} else
		sig_ign(sig_list, sig_hand);

	printf("Leaving caninit\n");
	fflush(stdout);
}


int can_set_filter(int fd, unsigned long id, unsigned long mask) {
	can_filter_t filter_data;
	filter_data.id = id;
	filter_data.mask = mask;

	return(devctl(fd, DCMD_CAN_FILTER, (void *) &filter_data,
			sizeof(filter_data), NULL));
}


int can_empty_queue(int fd) {
	int num_dropped = -1;
	devctl(fd, DCMD_CAN_EMPTY_Q, (void *) &num_dropped, sizeof(int), NULL);
	return num_dropped;
}


int can_arm(int fd, int channel_id) {
	int coid;
    static sigevent event;

    /* We need a connection to that channel for the pulse to be delivered on. */
    coid = ConnectAttach(0, 0, channel_id, _NTO_SIDE_CHANNEL, 0);

    /* Fill in the event structure for a pulse; use the file descriptor of the
     * CAN device so that this pulse can be distinguished from other devices
     * that may be sending pulses to this client. */
    SIGEV_PULSE_INIT(&event, coid, SIGEV_PULSE_PRIO_INHERIT, fd, 0);

    return devctl(fd, DCMD_CAN_ARM, (void *) &event,
    		sizeof(struct sigevent), NULL);
}


int can_read(intptr_t fd, unsigned long *id, char *extended, void *data,
		BYTE size)
{
	can_msg_t msg;
	int status;
	can_dev_handle_t *phdl = (can_dev_handle_t *) fd;
	int real_fd = phdl->fd;
	int channel_id = phdl->channel_id;
	char msg_buf[MAX_MSG_BUF];
	struct _msg_info msginfo;
	int rcvid;
	memset(&msg, 0, sizeof(msg));

	rcvid = MsgReceive(channel_id, msg_buf, MAX_MSG_BUF, &msginfo);

	if (rcvid == 0) {
		status = devctl(real_fd, DCMD_CAN_I82527_READ, (void *) &msg,
                        sizeof(msg), NULL);
	} else {
		printf("rcvid %d channel_id %d chid %d pid %d ",
			rcvid, channel_id, msginfo.chid, msginfo.pid);
 		printf("msglen %d coid %d scoid %d\n",
			msginfo.msglen, msginfo.coid, msginfo.scoid);
		perror("MsgReceive");
		return -1;
	}

	if (status != EOK) {
		printf("can_read: devctl error %d\n", status);
		return -1;
	}
	if (msg.error != 0) {
#ifdef DO_TRACE
		printf("can_read: failed to pop msg from in buffer\n");
#endif
		return -1;
	}

	if (id != NULL)
		*id = CAN_ID(msg);
	if (extended != NULL) {
		if (IS_EXTENDED_FRAME(msg))
			*extended = 1;
		else
			*extended = 0;
	}
	memcpy(data, msg.data, size > 8 ? 8 : size);

#ifdef DO_TRACE
	printf("can_read: msg.id 0x%08x msg.size %hhd\n", msg.id, msg.size);
#endif
	return msg.size;
}


int can_write(intptr_t fd, unsigned long id, char extended, void *data,
		BYTE size) {
	can_dev_handle_t *phdl = (can_dev_handle_t*) fd;
	int real_fd = phdl->fd;
	can_msg_t msg;

	msg.size = size > 8 ? 8 : size;
	msg.id = id;

	if (extended) SET_EXTENDED_FRAME(msg);

	memcpy(msg.data, data, msg.size);

	return devctl(real_fd, DCMD_CAN_I82527_WRITE, (void *) &msg,
			sizeof(msg), NULL);
}


int can_send(int fd, j1939_pdu_typ *pdu) {
	int status;  /* used to check whether the can_write procedure passed */
	unsigned long id = PATH_CAN_ID(pdu);

#ifdef DEBUG
	printf("send_can: PATH_CAN_ID 0x%x, num_bytes %d ", id, pdu->num_bytes);
	for (int i = 0; i < 8; i++)
		printf("%d ", pdu->data_field[i]);
	printf("\n");
#endif

	status = can_write(fd, id, 1, &pdu->data_field,
		(BYTE) (pdu->num_bytes & 0xff));

	if (status == -1) {
		fprintf(stderr, "send_can: can_write failed\n");
		return 0;
	} else
		return 1;
}
