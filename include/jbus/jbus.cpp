/**\file
 *
 * jbus_send.cpp
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date January 15, 2019
 */

#include "can/can.h"
#include "can/can_man.h"
#include "jbus.h"
#include "j1939_utils.h"
#include "j1939_struct.h"
#include <string>
#include <malloc.h>
#include <fcntl.h>
#include <sys/neutrino.h>

using namespace std;

int JBus::init(string filename, int flags, void *p_other) {
	int fd;
	int channel_id;
	can_dev_handle_t *phdl = new can_dev_handle_t();
	fd = open(filename.c_str(), flags);
	if (fd == -1) {
		perror("can_open");
		return -1;
	}
	if (flags == O_RDONLY) {
		channel_id = ChannelCreate(0);
		if (channel_id == -1) {
			printf("can_open: ChannelCreate failed\n");
			return -1;
		}
		can_set_filter(fd, 0, 0); // listens to all messages
		can_empty_queue(fd);
		if (can_arm(fd, channel_id) == -1) {
			printf("can_arm failed\n");
			return -1;
		}
	}
	phdl->fd = fd;
	phdl->channel_id = channel_id;
	phdl->flags = flags;
	phdl->filename = filename;
	return (intptr_t)phdl;
}


int JBus::close_conn(int *pfd) {
	can_dev_handle_t **pphdl = (can_dev_handle_t **) pfd;
	can_dev_handle_t *phdl = *pphdl;

	if (phdl == NULL) {
		fprintf(stderr, "NULL handle passed to can_close\n");
		return -1;
	}

	// close the connection
	int retval = close(phdl->fd);

	// free up memory
  	delete phdl;
	*pphdl = NULL;

	// check whether the connection was successfully closed
	if (retval == -1)
		perror("can_close");

	return retval;
}


int JBus::receive(int fd, j1939_pdu_typ *pdu, int *extended, int *slot) {
	unsigned long id;
	char extbyte = 0;
	int retval = can_read(fd, &id, &extbyte, &pdu->data_field, 8);
	if (retval == -1) {
		return J1939_RECEIVE_MESSAGE_ERROR;
	} else {
		*extended = (int) extbyte;
		pdu->priority = PATH_CAN_PRIORITY(id);
		pdu->pdu_format = PATH_CAN_PF(id);
		pdu->pdu_specific = PATH_CAN_PS(id);
		pdu->src_address = PATH_CAN_SA(id);
		pdu->num_bytes = retval;
		return retval;
	}
}


JBus::~JBus() {}
