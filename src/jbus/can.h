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

#include "j1939_struct.h"
#include <string>


/** blank */
#define MAX_MSG_BUF 1000

/* Macros used when processing can messages. */
#define PATH_CAN_ID(j)  ((((j)->priority & 0x7) << 26) | \
			(((j)->reserved & 0x1) << 25) | \
			(((j)->data_page & 0x1) << 24) | \
			(((j)->pdu_format & 0xff) << 16) | \
			(((j)->pdu_specific & 0xff) << 8) | \
			(((j)->src_address & 0xff)))				/**< blank */
#define PATH_CAN_PRIORITY(j)	(((j) >> 26) & 0x7)		/**< blank */
#define PATH_CAN_PF(j)		 	(((j) >> 16) & 0xff)	/**< blank */
#define PATH_CAN_PS(j)		 	(((j) >> 8) & 0xff)		/**< blank */
#define PATH_CAN_SA(j)		 	((j) & 0xff)			/**< blank */

/* Structures used when collecting data from the CAN. */
typedef unsigned char MSG_BIT;		/**< blank */
typedef unsigned int MSG_FIELD;		/**< blank */
typedef unsigned char MSG_BYTE;		/**< blank */
typedef unsigned char FRAME_BIT;	/**< blank */


/* most significant bit sets frame as extended */
#define IS_EXTENDED_FRAME(MSG)		(((MSG).id) & 0x80000000)
#define SET_EXTENDED_FRAME(MSG)		(MSG).id |= 0x80000000
#define CAN_ID(MSG)			((MSG).id & ~0x80000000)


/** Type used in devctl for CAN I/O. */
typedef struct {
	unsigned long id; 		/**< CAN device id on bus */
	unsigned char size;		/**< number of data bytes (0-8) */
	char data[8];			/**< data field */
	int error;				/**< set to non-zero if error on read or write */
} can_msg_t;


/** Used to set filtering of CAN messages. */
typedef struct {
	unsigned long id;		/**< 0 any message id */
	unsigned long mask;		/**< 0xff all messages */
} can_filter_t;


/**
 *
 * This structure type is specific to the I82527 driver and not visible except
 * to routines in this file.
 */
typedef struct {
	int fd;
	int channel_id;
	int flags;
	std::string filename;
} can_dev_handle_t;


/** J1939 Protocol Data Unit (PDU) format.
 *
 * This is the format the messages are collected from the CAN card. This is then
 * processed into message formats located in j1939_struct.h
 *
 * See SAE J1939, 3.1.2
 */
typedef struct {
	MSG_FIELD priority;		/**< bits 3:1, frame bits 2:4; ID 28:26, used for */
							/**< arbitration, lower priority for data not time */
							/**< critical; 000 highest priority */
	MSG_BIT reserved;		/**< frame bit 5;ID 25; reserved for future use */
	MSG_BIT data_page;		/**< frame bit 6;ID 24; 0 currently  */
	MSG_FIELD pdu_format;	/**< Protocol Data Unit Format (PF); bits 8:1; */
							/**< frame bits 7:12, 15:16; ID 23:16 */
	MSG_FIELD pdu_specific; /**< PDU Specific (PS); bits 8:1, frame bits 18:24; */
							/**< ID 15:8 if 0<=PF<=239, destination address */
							/**< (DA); if 240<=PF<=255, group extension (GE); */
	MSG_FIELD src_address;	/**< bits 8:1; frame bits 25:32; ID 7:0 */
	MSG_BYTE data_field[8];	/**< 64 bits maximum */
	int num_bytes;			/**< number of bytes in data_field */
} j1939_pdu;


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
 * @param fd blank
 * @param id blank
 * @param extended updated to be 1 if the identifier is in extended (29 bit)
 * format, 0 if it is in the unextended (11 bit) format
 * @param data blank
 * @param size blank
 * @return -1 if error encountered, otherwise number of bytes in the data
 * segment
 */
extern int can_read(int fd, unsigned long *id, char *extended, void *data,
		unsigned char size);


extern int can_write(int fd, unsigned long id, char extended, void *data,
		unsigned char size);


/** PATH CAN driver does not yet use state code or slot arguments, provided
 * for compatibility with B&B STB converter send routine.
 *
 * @return 0 on error, 1 on success.
 */
extern int can_send(int fd, j1939_pdu *pdu, int slot);


#endif /* SRC_JBUS_CAN_H_ */
