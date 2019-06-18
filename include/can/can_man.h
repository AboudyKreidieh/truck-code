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
typedef struct {
	int port;           		/**< Base address of adapter */
	int	irq;            		/**< Interrupt request line. */
	int	use_extended_frame;		/**< 1 yes, 0 no */
	int	bit_speed;				/**< bit speed of the CAN in Kb/s */
	int	intr_id;				/**< ID returned by Interrupt Attach Event */
	can_filter_t filter;		/**< Used to set filtering of CAN messages */
} can_info_t;


/** struct returned to client by can_get_errs and can_clear_errs devctl */
typedef struct {
	unsigned int shadow_buffer_count;   /**< TODO */
	unsigned int intr_in_handler_count; /**< TODO */
	unsigned int rx_interrupt_count;    /**< Rx interrupt count for the CAN card. */
	unsigned int rx_message_lost_count; /**< Number of Rx message overrun errors. */
	unsigned int tx_interrupt_count;    /**< Tx interrupt count for the CAN card. */
} can_err_count_t;


/** Information per Open Context Block.
 *
 * May be one for CAN and one for digital I/O per instance of driver; digital
 * I/O does not send notify now.
 */
typedef struct
{
	iofunc_ocb_t io_ocb;    /**< TODO */
	int rcvid;              /**< Used to notify client. */
    sigevent clt_event;     /**< Used to notify client, from client */
} can_ocb_t;


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


/** Path to the default configuration file.
 *
 * This is used when initializing the CAN connection. A typical configuration
 * file is text, and might contain lines like:
 *
 *	[first_section_name] 	\n
 *	MoreData=TRUE 			\n
 *	AnotherVariable=1.2345 	\n\n
 *
 *	[windows_section] 		\n
 *	ScreenSaveActive=1 		\n
 *	DoubleClickSpeed=452 	\n\n
 *
 *	[last_section]			\n
 *	MoreData=FALSE
 */
#define DEFAULT_CONFIG	 "realtime.ini"

/** Directory of the CAN device connection. */
#define DEFAULT_DEVICE	 "/dev/can1"

/** TODO */
#define INI_IRQ_ENTRY	 "Irq"

/** TODO */
#define INI_PORT_ENTRY	 "Port"

/** TODO */
#define INI_EXT_ENTRY	 "Ext"

/** Default Interrupt Request Line. By default, set to no interrupt. */
#define DEFAULT_IRQ		 0

/** Default address of the CAN adapter. */
#define DEFAULT_PORT	 0x210

/** Default size of the buffers for the input and output buffers, stored under
 * attr.in_buff and attr.out_buff, respectively. */
#define DEFAULT_QSIZE	 150


/** CAN Device Manager class.
 *
 * This object is responsible for initializing and interacting with the CAN
 * card. CAN messages are received through the SJA1000 chip, a blank.
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
	 * 		used to set filtering of CAN messages
	 * @return
	 * 		1 if the CAN received the interrupt, 0 otherwise
	 */
	virtual int interrupt(CircularBuffer *in_buff, CircularBuffer *out_buff,
			can_filter_t filter);

	/** Send a message to the bus.
	 *
	 * Single CAN frames or the very first Message are copied into the CAN
	 * controller using this function. After that an transmission request is set
	 * in the CAN controllers command register. After a successful transmission,
	 *  an interrupt will be generated, which will be handled in the CAN ISR
	 *  CAN_Interrupt().
	 *
	 * @param pattr
	 * 		pointer to information per device manager
	 */
	virtual void send(CircularBuffer *out_buff);

	/** Read the latest element in the buffer.
	 *
	 * This will also remove the element from the buffer.
	 *
	 * @param in_buff
	 * 		circular buffer for the input messages
	 * @return
	 * 		the front-most message.
	 */
	virtual can_msg_t read(CircularBuffer *in_buff);

	/** Write a new message to the CAN card.
	 *
	 * This method is responsible for performing the following tasks.
	 *
	 * - If a new message is provided, it is written to and stored in the
	 * 	 provided output buffer, tHen promptly sent to the CAN card.
	 * - If no message was sent to the CAN card within the past second (with
	 * 	 the assumption that we are sending heartbeat messages at the frequency
	 * 	 of 1 Hz), the can_timeout_count attribute is incremented.
	 * - If the buffer has got more than 10 messages stored, the messages are
	 * 	 flushed from the system and the number of removed messages is added to
	 * 	 the tx_buffer_flush attribute.
	 *
	 * @param out_buff
	 * 		circular buffer that stores output messages
	 * @param pmsg
	 * 		pointer to the CAN message that should be written
	 * @return
	 * 		0 for success, or -1 if an error occurs
	 */
	virtual int write(CircularBuffer *out_buff, can_msg_t *pmsg);

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
	 * @param in_buff
	 * 		circular buffer for the input messages
	 * @param filter
	 * 		used to set filtering of CAN messages
	 */
	virtual void rx_process_interrupt(CircularBuffer *in_buff,
			can_filter_t filter);

	/** Virtual destructor. */
	virtual ~CANDeviceManager();

private:
	/** bit speed of the different channels */
	int _baud[MAX_CHANNELS]					= { 0x0 };
	/** TODO */
	unsigned int _acc_code[MAX_CHANNELS]	= { 0x0 };
	/** TODO */
	unsigned int _acc_mask[MAX_CHANNELS]	= { 0x0 };

	/** Last time a CAN message was sent. */
	time_t _last_time_can_sent;

	/** Stores the error count from operations performed during any devctl
	 * procedure. */
	can_err_count_t _errs;

	/** Memory-mapped base address of the CAN registers. Used by the CANin and
	 * CANout macros to access registers. */
	canregs_t *_base_addr;

	/** Start board.
	 *
	 * @param minor
	 * 		index of the current device within the channels
	 * @return
	 * 		0 for success, or -1 if an error occurs
	 */
	virtual int _start_chip(int minor);

	/** Stop board.
	 *
	 * @param minor
	 * 		index of the current device within the channels
	 * @return
	 * 		0 for success, or -1 if an error occurs
	 */
	int _stop_chip(int minor);


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
	 * 		index of the current device within the channels
	 * @return
	 * 		0 for success, or -1 if an error occurs
	 */
	virtual int _reset_chip(int minor);

	/** Configure bit timing.
	 *
	 * Note: Chip must be in bus off state.
	 *
	 * @param minor
	 * 		index of the current device within the channels
	 * @param baud
	 * 		CAN baud rate
	 * @return
	 * 		0 for success, or -1 if an error occurs
	 */
	virtual int _set_timing(int minor, int baud);

	/** TODO
	 *
	 * @param minor
	 * 		index of the current device within the channels
	 * @param code
	 * 		TODO
	 * @param mask
	 * 		TODO
	 * @return
	 * 		0 for success, or -1 if an error occurs
	 */
	virtual int _set_mask(int minor, unsigned int code, unsigned int mask);

	/* Set value of the output control register.
	 *
	 * @param minor
	 * 		index of the current device within the channels
	 * @param arg
	 * 		value to set the output control register to
	 * @return
	 * 		0 for success, or -1 if an error occurs
	 */
	int _set_omode(int minor, int arg);

	/* Set listen-only mode.
	 *
	 * In listen-only mode, the CAN module is able to receive messages without
	 * giving an acknowledgment. Since the module does not influence the CAN bus
	 * in this mode the host device is capable of functioning like a monitor or
	 * for automatic bit-rate detection.
	 *
	 * Must be done after CMD_START(CAN_StopChip) and before
	 * CMD_START(CAN_StartChip).
	 *
	 * @param minor
	 * 		index of the current device within the channels
	 * @param arg
	 * 		if 1 the canmode variable is set, otherwise it is reset
	 * @return
	 * 		0 for success, or -1 if an error occurs
	 */
	int _set_listen_only_mode(int minor, int arg);
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
 * 		A pointer to a resmgr_context_t structure that the resource-manager
 * 		library uses to pass context information between functions.
 * @param io_ocb
 * 		Open control block (usually embedded within file system ocb)
 * @param event
 * 		CAN signal event structure, used to receive and process pulses
 * @return
 * 		0 for success, or -1 if an error occurs
 */
extern int can_dev_arm(resmgr_context_t *ctp, iofunc_ocb_t *io_ocb,
		sigevent event);


/* -------------------------------------------------------------------------- */
/* ---------------------- Implemented in can_init.cpp ----------------------- */
/* -------------------------------------------------------------------------- */


/** Attach pulses and interrupt events.
 *
 * Attach pulse to be sent by interrupt handler to event that will connected to
 * the interrupt by InterruptAttachEvent in can_dev->arm.
 *
 * @param dpp
 *		The dispatch handle, as returned by dispatch_create().
 * @param pattr
 * 		pointer to information per device manager
 */
extern void pulse_init(dispatch_t *dpp, IOFUNC_ATTR_T *pattr);


/* -------------------------------------------------------------------------- */
/* ----------------------- Implemented in io_func.cpp ----------------------- */
/* -------------------------------------------------------------------------- */


/** Handler for _IO_DEVCTL CAN messages.
 *
 * This is used to initialize resource manager function tables with CAN specific
 * function for devctl. It is responsible for handling the DCMD_* functionality.
 *
 * @param ctp
 * 		A pointer to a resmgr_context_t structure that the resource-manager
 * 		library uses to pass context information between functions.
 * @param msg
 * 		A pointer to the io_open_t structure that contains the message that the
 * 		resource manager received. For more information, see the documentation
 * 		for iofunc_open().
 * @param io_ocb
 * 		A pointer to the iofunc_ocb_t structure for the Open Control Block that
 * 		was created when the client opened the resource.
 * @return
 * 		EOK - Successful completion. \n
 * 		EINVAL - An attempt to set the flags for a resource that is
 * 		synchronized, with no mount structure defined, or no synchronized I/O
 * 		defined. \n
 * 		ENOTTY - An unsupported device control message was decoded.
 */
extern int io_devctl(resmgr_context_t *ctp, io_devctl_t *msg,
		RESMGR_OCB_T *io_ocb);


/** Handler for _IO_CONNECT of CAN messages.
 *
 * This is used to initialize resource manager function tables with CAN specific
 * function for open().
 *
 * @param ctp
 * 		A pointer to a resmgr_context_t structure that the resource-manager
 * 		library uses to pass context information between functions.
 * @param msg
 * 		A pointer to the io_open_t structure that contains the message that the
 * 		resource manager received. For more information, see the documentation
 * 		for iofunc_open().
 * @param handle
 *		A pointer to the iofunc_attr_t structure that defines the
 *		characteristics of the device that the resource manager is controlling.
 * @param extra
 * 		Extra information from the library. If you're calling
 * 		iofunc_open_default() from a resource manager's open() function (see
 * 		resmgr_connect_funcs_t), simply pass the extra argument that's passed to
 * 		open().
 * @return
 * 		EOK - Successful completion. \n
 * 		ENOSPC - There's insufficient memory to allocate the OCB.\n
 * 		ENOMEM - There's insufficient memory to allocate an internal data
 * 		structure required by resmgr_open_bind().
 */
extern int io_open(resmgr_context_t *ctp, io_open_t *msg,
	RESMGR_HANDLE_T *handle, void *extra);


#endif /* INCLUDE_JBUS_CAN_DEV_H_ */
