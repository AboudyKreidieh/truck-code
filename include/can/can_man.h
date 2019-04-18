/**\file
 *
 * can_man.h
 *
 * Structures, definitions, and prototypes for the CAN driver.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 19, 2019
 */

#ifndef INCLUDE_JBUS_CAN_DEV_H_
#define INCLUDE_JBUS_CAN_DEV_H_

#include "das_clt.h"
#include "sja1000.h"
#include <sys/iomsg.h>
#include <sys/iofunc.h>
#include "utils/common.h"
#include "utils/buffer.h"


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


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


/* DAS-style initialization file constant, which may be used for the digital I/O
 * on the SSV CAN card. */

#define DEFAULT_CONFIG	 "realtime.ini"	/**< TODO */
#define DEFAULT_DEVICE	 "/dev/can1"	/**< directory of the CAN device */
#define INI_IRQ_ENTRY	 "Irq"			/**< TODO */
#define INI_PORT_ENTRY	 "Port"			/**< TODO */
#define INI_EXT_ENTRY	 "Ext"			/**< TODO */
#define DEFAULT_IRQ		 0				/**< default Interrupt Request Line. */
										/**< By default, set to no interrupt. */
#define DEFAULT_PORT	 0x210			/**< TODO */
#define DEFAULT_PRIORITY 19				/**< TODO */
#define DEFAULT_QSIZE	 150			/**< TODO */



/** CAN Device Manager class.
 *
 * This object is responsible for initializing and interacting with the CAN
 * card. TODO: method the register and SJA1000.
 */
class CANDeviceManager
{
public:
	/** Number of times a heartbeat was not received from the CAN for over 1
	 * consecutive second. */
	int can_timeout_count = 0;

	/** Number of messages that were flushed from the output buffer without ever
	 * being sent. */
	int tx_buffer_flush = 0;

	/** TODO */
	int can_notify_client_err = 0;

	/** TODO */
	int mask_count_non_zero = 0;

	/** Initialize the Phillips SJA1000 chip to support the CAN.
	 *
	 * @param base_address
	 * 		memory-mapped base address of the CAN registers. Used by the CANin
	 * 		and CANout macros to access registers.
	 * @param bit_speed
	 * 		CAN bit speed, in Kb/s
	 * @param extended_frame
	 * 		specifies whether the CAN is using the standard or extended frame
	 * 		format
	 */
	virtual void init(unsigned int base_address, unsigned int bit_speed,
			BYTE extended_frame);


	/** Interrupt Request, ISR
	 *
	 * Called by can_handle_interrupt.
	 *
	 * @param in_buff
	 * 		circular buffer for the input messages
	 * @param out_buff
	 * 		circular buffer for the output messages
	 * @param filter
	 * 		TODO
	 * @return
	 * 		1 if the CAN received the interrupt, 0 otherwise
	 */
	virtual int interrupt(CircularBuffer *in_buff, CircularBuffer *out_buff,
			can_filter_t filter);


	/** Send a message to the bus.
	 *
	 * Gets the message from the front of the transmit queue and puts it on the
	 * CAN bus.
	 *
	 * @param pattr
	 * 		pointer to information per device manager
	 */
	virtual void send(CircularBuffer *out_buff);


	/** TODO
	 *
	 * @param in_buff
	 * 		circular buffer for the input messages
	 * @return
	 * 		TODO
	 */
	virtual can_msg_t read(CircularBuffer *in_buff);


	/** TODO
	 *
	 * @param out_buff
	 * 		circular buffer that stores output messages
	 * @param pmsg
	 * 		pointer to the CAN message that should be written
	 * @return
	 * 		0 for success, or -1 if an error occurs
	 */
	virtual int write(CircularBuffer *out_buff, can_msg_t *pmsg);


	/** Arm the CAN device manager.
	 *
	 * Attach the hardware interrupt, and save the event to be used to notify
	 * the client in the ocb structure.
	 *
	 * @param ctp
	 * 		TODO
	 * @param io_ocb
	 * 		Open control block (usually embeded within file system ocb)
	 * @param event
	 * 		TODO
	 * @return
	 * 		0 for success, or -1 if an error occurs
	 */
//	virtual int arm(resmgr_context_t *ctp, iofunc_ocb_t *io_ocb,
//			sigevent event);


	/** Clear the error counts and return the old counts.
	 *
	 * @return
	 * 		old counts
	 */
	virtual can_err_count_t clear_errs();


	/** Return the current error count. */
	virtual can_err_count_t get_errs();


	/** Send a new message after notification of transmission of old one.
	 *
	 * @param out_buff
	 * 		circular buffer for the output messages
	 */
	virtual void tx_process_interrupt(CircularBuffer *out_buff);


	/** Read message from chip and queue for the resource manager.
	 *
	 * @param TODO
	 * @param TODO
	 */
	virtual void rx_process_interrupt(CircularBuffer *in_buff,
			can_filter_t filter);


	/** Virtual destructor. */
	virtual ~CANDeviceManager();

private:
	int _baud[MAX_CHANNELS]					= { 0x0 };	/**< TODO */
	unsigned int _acc_code[MAX_CHANNELS]	= { 0x0 };	/**< TODO */
	unsigned int _acc_mask[MAX_CHANNELS]	= { 0x0 };	/**< TODO */
	int _outc[MAX_CHANNELS]    				= { 0x0 };	/**< TODO */
	int _tx_err[MAX_CHANNELS]   			= { 0x0 };	/**< TODO */
	int _rx_err[MAX_CHANNELS]   			= { 0x0 };	/**< TODO */

	/** Last time a CAN message was sent. */
	time_t _last_time_can_sent;

	/** Stores the error count from operations performed during any devctl
	 * procedure. */
	can_err_count_t _errs;

	/** Memory-mapped base address of the CAN registers. Used by the CANin and
	 * CANout macros to access registers. */
	canregs_t *_base_addr;

	/** TODO
	 *
	 */
	virtual int _start_chip(int minor);


	/** Reset board.
	 *
	 * This performs the following procedure:
	 *
	 * 1. Set Reset Request
	 * 2. Wait for Rest request is changing - used to see if board is available
	 *    initialize board (with valuse from /proc/sys/Can)
	 * 3. Set output control register
	 * 4. Set timings
	 * 5. Set acceptance mask
	 *
	 * @param minor
	 * 		TODO
	 * @return
	 * 		0 for success, or -1 if an error occurs
	 */
	virtual int _reset_chip(int minor);


	/** Configure bit timing.
	 *
	 * Note: Chip must be in bus off state.
	 *
	 * @param minor
	 * 		TODO
	 * @param baud
	 * 		CAN baud rate
	 * @return
	 * 		TODO
	 */
	virtual int _set_timing(int minor, int baud);


	virtual int _set_mask(int minor, unsigned int code, unsigned int mask);
};


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


/** Information per device manager */
typedef struct
{
	CANDeviceManager *can_dev;	/**< CAN device manager class */
	iofunc_attr_t io_attr;		/**< standard system information */
	char *devname;				/**< device path name */
	can_info_t can_info;  		/**< initialization info */
	CircularBuffer *in_buff;	/**< Holds CAN messages until client reads */
	CircularBuffer *out_buff;	/**< Holds CAN messages until written to bus */
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


/* -------------------------------------------------------------------------- */
/* ----------------------- Implemented in can_if.cpp ------------------------ */
/* -------------------------------------------------------------------------- */


/** Arm the CAN device manager.
 *
 * Attach the hardware interrupt, and save the event to be used to notify the
 * client in the ocb structure.
 *
 * @param ctp
 * 		TODOIOFUNC_
 * @param io_ocb
 * 		Open control block (usually embedded within file system ocb)
 * @param event
 * 		TODO
 * @return
 * 		0 for success, or -1 if an error occurs
 */
extern int can_dev_arm(resmgr_context_t *ctp, iofunc_ocb_t *io_ocb,
		sigevent event);


/* -------------------------------------------------------------------------- */
/* ----------------------- Implemented in can_dev.cpp ----------------------- */
/* -------------------------------------------------------------------------- */


/** Interrupt Request, ISR
 *
 * Called by can_handle_interrupt.
 *
 * @param pattr
 * 		pointer to information per device manager
 * @return
 * 		1 if the CAN received the interrupt, 0 otherwise
 */
extern int can_dev_interrupt(IOFUNC_ATTR_T *pattr);


/* -------------------------------------------------------------------------- */
/* ---------------------- Implemented in can_init.cpp ----------------------- */
/* -------------------------------------------------------------------------- */


/** TODO
 *
 * When pulse is received by the resource manager as a result of the
 * InterruptAttachEvent in pulse_init the device-specific routine
 * can_dev_interrupt is called to reset any interrupt registers, etc.
 *
 * Furthermore, any event registered by the client with can_arm is delivered to
 * the client, who will then do a read to get the data that has been copied into
 * the message buffer.
 *
 * @param ptr
 * 		TODO
 * @return
 * 		TODO
 */
extern int can_handle_interrupt(void *ptr);
//extern int can_handle_interrupt(message_context_t *ctp, int code,
//	unsigned flags, void *ptr);


/** Attach pulses and interrupt event.
 *
 * Attach pulse to be sent by interrupt handler to event that will connected to
 * the interrupt by InterruptAttachEvent in can_dev_arm.
 *
 * @param dpp
 * 		TODO
 * @param pattr
 * 		pointer to information per device manager
 */
extern void pulse_init(dispatch_t *dpp, IOFUNC_ATTR_T *pattr);


/* -------------------------------------------------------------------------- */
/* ----------------------- Implemented in io_func.cpp ----------------------- */
/* -------------------------------------------------------------------------- */


/** TODO
 *
 */
extern int io_devctl(resmgr_context_t *ctp, io_devctl_t *msg,
	RESMGR_OCB_T *io_ocb);


/** TODO
 *
 */
extern int io_open(resmgr_context_t *ctp, io_open_t *msg,
	RESMGR_HANDLE_T *handle, void *extra);


#endif /* INCLUDE_JBUS_CAN_DEV_H_ */
