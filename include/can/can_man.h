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
#include "buffer.h"
#include "can_struct.h"
#include <sys/iomsg.h>
#include <sys/iofunc.h>
#include "utils/common.h"

/* Defined in can_init.cpp */  // TODO: remove globals
extern int can_notify_client_err;
extern int mask_count_non_zero;

/* Defined in can_dev.cpp */  // TODO: remove globals
extern unsigned int shadow_buffer_count;
extern unsigned int intr_in_handler_count;
extern unsigned int rx_interrupt_count;
extern unsigned int rx_message_lost_count;
extern unsigned int tx_interrupt_count;


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



/** TODO
 */
class CANDeviceManager
{
public:
	/** Initialize the Phillips SJA1000 chip to support the CAN.
	 *
	 * @param base_address
	 * 		TODO
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
	 * @param pattr
	 * 		pointer to information per device manager
	 * @return
	 * 		1 if the CAN received the interrupt, 0 otherwise
	 */
//	virtual int interrupt(IOFUNC_ATTR_T *pattr);


	/** Send a message to the bus.
	 *
	 * Gets the message from the front of the transmit queue and puts it on the
	 * CAN bus.
	 *
	 * @param pattr
	 * 		pointer to information per device manager
	 */
//	virtual void send(IOFUNC_ATTR_T *pattr);


	/** TODO
	 *
	 * @param pattr
	 * 		pointer to information per device manager
	 * @return
	 * 		TODO
	 */
//	virtual can_msg_t read(IOFUNC_ATTR_T *pattr);


	/** TODO
	 *
	 * @param pocb
	 * 		TODO
	 * @param pmsg
	 * 		pointer to the CAN message that should be written
	 * @return
	 * 		0 for success, or -1 if an error occurs
	 */
//	virtual int write(can_ocb_t *pocb, can_msg_t *pmsg);


	/** TODO
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


	/** TODO
	 *
	 * Not implemented yet at the level of the device registers All are received
	 * in Object 15. Later this function may include calls to device register
	 * operations.
	 *
	 * @param pattr
	 * 		pointer to information per device manager
	 * @param filter
	 * 		TODO
	 * @return
	 * 		0 for success, or -1 if an error occurs
	 */
//	virtual int add_filter(IOFUNC_ATTR_T *pattr, can_filter_t filter);


	/** Clear the error counts and return the old counts.
	 *
	 * @return
	 * 		old counts
	 */
//	virtual can_err_count_t clear_errs();


	/** Return the current error count. */
//	virtual can_err_count_t get_errs();


	/** Virtual destructor. */
	virtual ~CANDeviceManager();

private:
	can_err_count_t _errs;		/**< stores the error count from operations */
								/**< performed during any devctl procedure	*/
};


/* -------------------------------------------------------------------------- */
/* ----------------------- Implemented in can_if.cpp ------------------------ */
/* -------------------------------------------------------------------------- */


/** TODO
 *
 * @param pattr
 * 		pointer to information per device manager
 * @return
 * 		TODO
 */
extern can_msg_t can_dev_read(IOFUNC_ATTR_T *pattr);


/** TODO
 *
 * @param pocb
 * 		TODO
 * @param pmsg
 * 		pointer to the CAN message that should be written
 * @return
 * 		0 for success, or -1 if an error occurs
 */
extern int can_dev_write(can_ocb_t *pocb, can_msg_t *pmsg);


/** TODO
 *
 * Attach the hardware interrupt, and save the event to be used to notify the
 * client in the ocb structure.
 *
 * @param ctp
 * 		TODO
 * @param io_ocb
 * 		Open control block (usually embedded within file system ocb)
 * @param event
 * 		TODO
 * @return
 * 		0 for success, or -1 if an error occurs
 */
extern int can_dev_arm(resmgr_context_t *ctp, iofunc_ocb_t *io_ocb,
		sigevent event);


/** TODO
 *
 * Not implemented yet at the level of the device registers All are received
 * in Object 15. Later this function may include calls to device register
 * operations.
 *
 * @param pattr
 * 		pointer to information per device manager
 * @param filter
 * 		can filter type, specifying the mask and ID
 * @return
 * 		0 for success, or -1 if an error occurs
 */
extern int can_dev_add_filter(IOFUNC_ATTR_T *pattr, can_filter_t filter);


/** Add a new message if ID and MASK allow.
 *
 * @param pmsg
 * 		pointer to the new CAN message
 * @param pattr
 * 		pointer to information per device manager
 */
extern void can_new_msg(can_msg_t *pmsg, IOFUNC_ATTR_T *pattr);


/* -------------------------------------------------------------------------- */
/* ----------------------- Implemented in can_dev.cpp ----------------------- */
/* -------------------------------------------------------------------------- */


/** Clear the error counts and return the old counts.
 *
 * @return
 * 		old counts
 */
extern can_err_count_t can_dev_clear_errs();


/** Return the current error count. */
extern can_err_count_t can_dev_get_errs();


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


/** Send a message to the bus.
 *
 * Gets the message from the front of the transmit queue and puts it on the CAN
 * bus.
 *
 * @param pattr
 * 		pointer to information per device manager
 */
extern void can_dev_send(IOFUNC_ATTR_T *pattr);


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
 * @param ctp
 * 		TODO
 * @param code
 * 		TODO
 * @param flags
 * 		TODO
 * @param ptr
 * 		TODO
 * @return
 * 		TODO
 */
extern int can_handle_interrupt(message_context_t *ctp, int code,
	unsigned flags, void *ptr);


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
