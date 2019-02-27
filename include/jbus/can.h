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

#include "can_struct.h"
#include "j1939_struct.h"
#include <string>


/** can_init()
 *
 * This function is called from the main() during the initialization of the CAN
 * driver to read the .ini file and initialize the can_info structure, and
 * initialize the function tables.
 */
extern void can_init(int argc, char *argv[], resmgr_connect_funcs_t *pconn,
	resmgr_io_funcs_t *pio, IOFUNC_ATTR_T *pattr);


/** We never actually used this function on QNX4, so it was
 *  untested at PATH with QNX4.
 *	Internal to the driver, set as part of open for read
 */
extern int can_set_filter(int fd, unsigned long id, unsigned long mask);


/* Call this function to empty the queue of messages in
 * the CAN driver; returns number of messages that were dropped
 * from the queue.
 *	Internal to the driver, done as part of open for read
 */
extern int can_empty_queue(int fd);


/** Enable pulses from the CAN driver to the client process.
 *
 * Pulses are waited for with MsgReceive or IP_Receive. channel_id can be one
 * obtained from DB_CHANNEL(pclt) on a the pointer returned by the database
 * clt_login. Internal to the driver, done as part of open for read
 *
 * @param fd blank
 * @param channel_id blank
 * @return blank
 */
extern int can_arm(int fd, int channel_id);


/** Read information from the CAN card.
 *
 * The new data in store in **blank**.
 *
 * @param fd file descriptor for the location of the CAN card
 * @param id blank
 * @param extended updated to be 1 if the identifier is in extended (29 bit)
 * format, 0 if it is in the unextended (11 bit) format
 * @param data blank
 * @param size blank
 * @return number of bytes in the data segment; -1 if error encountered
 */
extern int can_read(int fd, unsigned long *id, char *extended, void *data,
		unsigned char size);


extern int can_write(int fd, unsigned long id, char extended, void *data,
		unsigned char size);


/** PATH CAN driver does not yet use state code or slot arguments, provided
 * for compatibility with B&B STB converter send routine.
 *
 * @param fd file descriptor that is used when sending the message
 * @param pdu message to send
 * @param slot blank
 * @return 0 on error, 1 on success.
 */
extern int can_send(int fd, j1939_pdu *pdu, int slot);


#endif /* SRC_JBUS_CAN_H_ */
