/*
 * can_struct.h
 *
 *  Created on: Feb 19, 2019
 *      Author: aboudy
 */

#ifndef INCLUDE_JBUS_CAN_STRUCT_H_
#define INCLUDE_JBUS_CAN_STRUCT_H_

#include "j1939_struct.h"
#include "utils/sys.h"
#include <string>
#include <sys/iofunc.h>
#include <sys/dispatch.h>


/** blank */
#define MAX_MSG_BUF 1000

/* Macros used when processing can messages. */
#define PATH_CAN_ID(j)  ((((j)->priority & 0x7) << 26) | \
			(((j)->reserved & 0x1) << 25) | \
			(((j)->data_page & 0x1) << 24) | \
			(((j)->pdu_format & 0xff) << 16) | \
			(((j)->pdu_specific & 0xff) << 8) | \
			(((j)->src_address & 0xff)))				/**< Get pdu identifier. */
#define PATH_CAN_PRIORITY(j)	(((j) >> 26) & 0x7)		/**< Get priority value. */
#define PATH_CAN_PF(j)		 	(((j) >> 16) & 0xff)	/**< Get pdu format value. */
#define PATH_CAN_PS(j)		 	(((j) >> 8) & 0xff)		/**< Get pdu specific value. */
#define PATH_CAN_SA(j)		 	((j) & 0xff)			/**< Get source address value */

/* Structures used when collecting data from the CAN. */
typedef unsigned char MSG_BIT;
typedef unsigned int MSG_FIELD;
typedef unsigned char MSG_BYTE;
typedef unsigned char FRAME_BIT;


/* most significant bit sets frame as extended */
#define IS_EXTENDED_FRAME(MSG)		(((MSG).id) & 0x80000000)	/**< Checks if identifier is 29 or 11 bit. */
#define SET_EXTENDED_FRAME(MSG)		(MSG).id |= 0x80000000		/**< Set the 29 bit identifier. */
#define CAN_ID(MSG)					((MSG).id & ~0x80000000)	/**< Get CAN identity number. */


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


/** This structure type is specific to the I82527 driver and not visible except
 * to routines in this file.
 */
typedef struct {
	int fd;
	int channel_id;
	int flags;
	std::string filename;
} can_dev_handle_t;


/** State information about CAN device manager */
typedef struct
{
	int port;           		/**< Base address of adapter */
	int	irq;            		/**< Interrupt request line. */
	int	use_extended_frame;		/**< 1 yes, 0, no */
	int	bit_speed;				/**< Kb/s */
	int	intr_id;;				/**< ID returned by Interrupt Attach Event */
	can_filter_t filter;		/**< blank */
} can_info_t;


/** struct returned to client by can_get_errs and can_clear_errs devctl */
typedef struct {
	unsigned int shadow_buffer_count;
	unsigned int intr_in_handler_count;
	unsigned int rx_interrupt_count;
	unsigned int rx_message_lost_count;
	unsigned int tx_interrupt_count;
} can_err_count_t;


/** J1939 Protocol Data Unit (PDU) format. TODO: merge with other thing
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


/** Information per Open Context Block; may be one for CAN and one for digital
 * I/O per instance of driver; digital I/O does not send notify now. */
typedef struct
{
	iofunc_ocb_t io_ocb;	/**< blank */
	int rcvid;              /**< Used to notify client. */
    sigevent clt_event;  	/**< Used to notify client, from client */
} can_ocb_t;


/** Information per device manager */
typedef struct
{
	iofunc_attr_t io_attr;		/**< standard system information */
	char *devname;				/**< device path name */
	can_info_t can_info;  		/**< initialization info */
	cbuff_typ in_buff;			/**< Holds CAN messages until client reads */
	cbuff_typ out_buff;			/**< Holds CAN messages until written to bus */
	can_ocb_t *notify_pocb;   	/**< OCB of client to be notified */
	bool verbose_flag;			/**< blank */
	sigevent hw_event;			/**< initialized in pulse_init */
} can_attr_t;


/* Forward declarations needed so new IOFUNC_OCB_T and IOFUNC_ATTR_T defines
 * can be used in sys/iofunc.h */

#ifndef IOFUNC_OCB_T
#define IOFUNC_OCB_T can_ocb_t
#endif

#undef IOFUNC_ATTR_T
#define IOFUNC_ATTR_T can_attr_t


#endif /* INCLUDE_JBUS_CAN_STRUCT_H_ */
