/**\file
 *
 * can_struct.h
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 19, 2019
 */

#ifndef INCLUDE_JBUS_CAN_STRUCT_H_
#define INCLUDE_JBUS_CAN_STRUCT_H_

#include "jbus/j1939_struct.h"
#include "utils/sys.h"
#include "utils/common.h"
#include <string>
#include <sys/iofunc.h>
#include <sys/dispatch.h>


/** Largest number of element allows in the CAN Rx buffers */
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


/* most significant bit sets frame as extended */
#define IS_EXTENDED_FRAME(MSG)		((MSG).id &  0x80000000)	/**< Checks if identifier is 29 or 11 bit. */
#define SET_EXTENDED_FRAME(MSG)		((MSG).id |= 0x80000000)	/**< Set the 29 bit identifier. */
#define CAN_ID(MSG)					((MSG).id & ~0x80000000)	/**< Get CAN identity number. */


/** Type used in devctl for CAN I/O. */
typedef struct {
	unsigned long id; 		/**< CAN device id on bus */
	BYTE size;				/**< number of data bytes (0-8) */
	BYTE data[8];			/**< data field (up to 8 bytes) */
	int error;				/**< set to non-zero if error on read or write */
} can_msg_t;


/** Used to set filtering of CAN messages. */
typedef struct {
	unsigned long id;		/**< 0 any message id */
	unsigned long mask;		/**< 0xff all messages */
} can_filter_t;


/** This structure type is specific to the I82527 driver and not visible except
 * to routines in this file. */
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
	int	use_extended_frame;		/**< 1 yes, 0 no */
	int	bit_speed;				/**< bit speed of the CAN in Kb/s */
	int	intr_id;				/**< ID returned by Interrupt Attach Event */
	can_filter_t filter;		/**< Used to set filtering of CAN messages */
} can_info_t;


/** struct returned to client by can_get_errs and can_clear_errs devctl */
typedef struct {
	unsigned int shadow_buffer_count;		/**< TODO */
	unsigned int intr_in_handler_count;		/**< TODO */
	unsigned int rx_interrupt_count;		/**< TODO */
	unsigned int rx_message_lost_count;		/**< TODO */
	unsigned int tx_interrupt_count;		/**< TODO */
} can_err_count_t;


/** Information per Open Context Block.
 *
 * May be one for CAN and one for digital I/O per instance of driver; digital
 * I/O does not send notify now.
 */
typedef struct
{
	iofunc_ocb_t io_ocb;	/**< TODO */
	int rcvid;              /**< Used to notify client. */
    sigevent clt_event;  	/**< Used to notify client, from client */
} can_ocb_t;


/** Circular buffer type used with data item saving.
 *
 * Data from last "data_size" samples can be saved in this structure.
 */
typedef struct {
  void *data_array;     /**< data elements */
  int data_size;  		/**< number of structures malloced */
  int data_count; 		/**< control intervals saved (<=data_size) */
  int data_start; 		/**< index of oldest item in the circular buffer */
} cbuff_typ;


/** Information per device manager */
typedef struct
{
	iofunc_attr_t io_attr;		/**< standard system information */
	char *devname;				/**< device path name */
	can_info_t can_info;  		/**< initialization info */
	cbuff_typ in_buff;			/**< Holds CAN messages until client reads */
	cbuff_typ out_buff;			/**< Holds CAN messages until written to bus */
	can_ocb_t *notify_pocb;   	/**< OCB of client to be notified */
	bool verbose_flag;			/**< verbose flag */
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
