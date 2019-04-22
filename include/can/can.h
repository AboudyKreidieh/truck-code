/**\file
 *
 * can.h
 *
 * This file contains utility methods needed to read and write messages to the
 * Control Area Network (CAN) card.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date January 14, 2019
 */

#ifndef SRC_JBUS_CAN_H_
#define SRC_JBUS_CAN_H_

#include "utils/common.h"		/* BYTE */
#include "jbus/j1939_struct.h"
#include <string>


/** Initialize the CAN driver.
 *
 * This function is called from the main() during the initialization of the CAN
 * driver to read the .ini file and initialize the can_info structure, and
 * initialize the function tables.
 *
 * Additional Arguments:
 *
 * - -e: specifies extended frame
 * - -f: specifies whether to create a copy of pconfig (not modify the original)
 * - -i: interrupt request line
 * - -n: device name
 * - -p: port
 * - -s: bit speed
 * - -v: verbose
 *
 * @param argc
 * 		see additional arguments
 * @param argv
 * 		see additional arguments
 * @param pconn
 * 		table of the POSIX-level connect functions that are used by a resource
 * 		manager. The open handle is updated to support opening the CAN card port
 * @param pio
 * 		table of the POSIX-level I/O functions that are used by a resource
 * 		manager. The devctl handle is updated to support interrupt handling on
 * 		the CAN card port
 * @param pattr
 * 		pointer to information per device manager
 */
extern void can_init(int argc, char *argv[], resmgr_connect_funcs_t *pconn,
	resmgr_io_funcs_t *pio, IOFUNC_ATTR_T *pattr);


/** Set the CAN filter.
 *
 * Internal to the driver, set as part of open for read
 *
 * @param fd
 * 		file descriptor for the location of the CAN card
 * @param id
 * 		CAN message format ID (standard or extended)
 * @param mask
 * 		requested filter mask
 * @return
 * 		EOK    - Success.\n
 * 		EAGAIN - The devctl() command couldn't be completed because the device
 * 		         driver was in use by another process, or the driver was unable
 * 		         to carry out the request due to an outstanding command in
 * 		         progress.\n
 * 		EBADF  - Invalid open file descriptor, filedes.\n
 * 		EINTR  - The devctl() function was interrupted by a signal.\n
 * 		EINVAL - The device driver detected an error in dev_data_ptr or
 * 		         n_bytes.\n
 * 		EIO    - The devctl() function couldn't complete because of a hardware
 * 		         error.\n
 * 		ENOSYS - The device doesn't support the dcmd command.\n
 * 		ENOTTY - The dcmd argument isn't a valid command for this device.\n
 * 		EPERM  - The process doesn't have sufficient permission to carry out the
 * 			     requested command.
 */
extern int can_set_filter(int fd, unsigned long id, unsigned long mask);


/** Empty the queue of messages in the CAN driver.
 *
 * This is internal to the driver, and done as part of open for read
 *
 * @param fd
 * 		file descriptor for the location of the CAN card
 * @return
 * 		number of messages that were dropped from the queue
 */
extern int can_empty_queue(int fd);


/** Enable pulses from the CAN driver to the client process.
 *
 * Pulses are waited for with MsgReceive or IP_Receive. Internal to the driver,
 * done as part of open for read.
 *
 * @param fd
 * 		file descriptor for the location of the CAN card
 * @param channel_id
 * 		The ID of the channel that can be used to receive messages and pulses
 * @return
 * 		EOK    - Success.\n
 * 		EAGAIN - The devctl() command couldn't be completed because the device
 * 		         driver was in use by another process, or the driver was unable
 * 		         to carry out the request due to an outstanding command in
 * 		         progress.\n
 * 		EBADF  - Invalid open file descriptor, filedes.\n
 * 		EINTR  - The devctl() function was interrupted by a signal.\n
 * 		EINVAL - The device driver detected an error in dev_data_ptr or
 * 				 n_bytes.\n
 * 		EIO    - The devctl() function couldn't complete because of a hardware
 * 				 error.\n
 * 		ENOSYS - The device doesn't support the dcmd command.\n
 * 		ENOTTY - The dcmd argument isn't a valid command for this device.\n
 * 		EPERM  - The process doesn't have sufficient permission to carry out the
 * 				 requested command.
 */
extern int can_arm(int fd, int channel_id);


/** Read information from the CAN card.
 *
 * This message will update the ID, message format, and data field of a CAN
 * message.
 *
 * @param fd
 * 		file descriptor for the location of the CAN card
 * @param id
 * 		CAN message format ID
 * @param extended
 * 		updated to be 1 if the identifier is in extended (29 bit) format, 0 if
 * 		it is in the unextended (11 bit) format
 * @param data
 * 		the content of the data field. This element will be modified by this
 * 		method.
 * @param size
 * 		number of bytes in the CAN data field
 * @return
 * 		number of bytes in the data segment; -1 if error encountered
 */
extern int can_read(intptr_t fd, unsigned long *id, char *extended, void *data,
	BYTE size);


/** Write information to the CAN card.
 *
 * @param fd
 * 		file descriptor for the location of the CAN card
 * @param id
 * 		CAN device id on bus
 * @param extended
 * 		0 for standard (11 bit) format, 1 for extended (29 bit) format
 * @param data
 * 		the content of the data field
 * @param size
 * 		number of bytes to be added to the data field
 * @return
 * 		EOK	   - Success.\n
 * 		EAGAIN - The devctl() command couldn't be completed because the device
 * 		         driver was in use by another process, or the driver was unable
 * 		         to carry out the request due to an outstanding command in
 * 		         progress.\n
 * 		EBADF  - Invalid open file descriptor, filedes.\n
 * 		EINTR  - The devctl() function was interrupted by a signal.\n
 * 		EINVAL - The device driver detected an error in dev_data_ptr or
 * 		         n_bytes.\n
 * 		EIO    - The devctl() function couldn't complete because of a hardware
 * 				 error.\n
 * 		ENOSYS - The device doesn't support the dcmd command.\n
 * 		ENOTTY - The dcmd argument isn't a valid command for this device.\n
 * 		EPERM  - The process doesn't have sufficient permission to carry out the
 * 				 requested command.\n
 */
extern int can_write(intptr_t fd, unsigned long id, char extended, void *data,
	BYTE size);


/** FIXME (merge with can_write)
 *
 * PATH CAN driver does not yet use state code or slot arguments, provided
 * for compatibility with B&B STB converter send routine.
 *
 * @param fd
 * 		file descriptor that is used when sending the message
 * @param pdu
 * 		message to send
 * @return
 * 		0 on error, 1 on success.
 */
extern int can_send(int fd, j1939_pdu_typ *pdu);


#endif /* SRC_JBUS_CAN_H_ */
