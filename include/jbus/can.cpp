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

#include "can.h"
#include "j1939_struct.h"
#include "das_clt.h"
#include <sys/types.h>
#include <sys/neutrino.h>
#include <stdio.h>
#include <unistd.h>
#include <devctl.h>
#include <errno.h>



int can_set_filter(int fd, unsigned long id, unsigned long mask) {
	can_filter_t filter_data;
	filter_data.id = id;
	filter_data.mask = mask;

	return(devctl(fd, DCMD_CAN_FILTER, (void *) &filter_data,
			sizeof(filter_data), NULL));
}


int can_empty_queue(int fd) {
	int num_dropped = -1;
	(void) devctl(fd, DCMD_CAN_EMPTY_Q, (void *) &num_dropped,
			sizeof(int), NULL);
	return num_dropped;
}


int can_arm(int fd, int channel_id) {
	int coid;
    static sigevent event;

    // we need a connection to that channel for the pulse to be delivered on
    coid = ConnectAttach(0, 0, channel_id, _NTO_SIDE_CHANNEL, 0);

    // fill in the event structure for a pulse; use the file descriptor of the
    // CAN device so that this pulse can be distinguished from other devices
    // that may be sending pulses to this client
    SIGEV_PULSE_INIT(&event, coid, SIGEV_PULSE_PRIO_INHERIT, fd, 0);

    return devctl(fd, DCMD_CAN_ARM, (void *) &event,
    		sizeof(struct sigevent), NULL);
}


int can_read(int fd, unsigned long *id, char *extended, void *data,
		unsigned char size) {
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
		return (-1);
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
	return(msg.size);
}


int can_write(int fd, unsigned long id, char extended, void *data,
		unsigned char size) {
	can_dev_handle_t *phdl = (can_dev_handle_t *) fd;
	int real_fd = phdl->fd;
	can_msg_t msg;

	msg.size = size > 8 ? 8 : size;
	msg.id = id;

	if (extended) SET_EXTENDED_FRAME(msg);

	memcpy(msg.data, data, msg.size);


	return(devctl(real_fd, DCMD_CAN_I82527_WRITE, (void *) &msg,
			sizeof(msg), NULL));
}


int can_send(int fd, j1939_pdu *pdu, int slot) {
	unsigned long id = PATH_CAN_ID(pdu);
#ifdef DEBUG
	int i;
	printf("send_can: PATH_CAN_ID 0x%x, num_bytes %d ", id, pdu->num_bytes);
	for (i = 0; i < 8; i++)
		printf("%d ", pdu->data_field[i]);
	printf("\n");
#endif
	if (can_write(fd, id, 1, &pdu->data_field,
		 (unsigned char) (pdu->num_bytes & 0xff)) == -1) {
		fprintf(stderr, "send_can: can_write failed\n");
		return 0;
	} else
		return 1;
}

