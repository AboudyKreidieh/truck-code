/**\file
 *
 * sja1000.h
 *
 * This includes device-dependent CAN definitions for the Phillips SJA 1000
 * driver. Assumes board is memory mapped at address given during initialization.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date April 6, 2018
 */

#include <sys/time.h>
#include "utils/common.h"
#include "utils/timestamp.h"
#include "delay.h"				/* atomic_t */

#ifndef INCLUDE_CAN_SJA1000_H_
#define INCLUDE_CAN_SJA1000_H_

/** TODO */
#define SJA1000_MAP_SIZE 128


/* --- Standard definition with offset 1 -------------------------------------*/

/** Object containing relevant data from a CAN message.
 *
 * This includes the CAN message identifier and the data field. This object
 * contains two substructures:
 *
 * - extframe: 29-bit message format
 * - stdframe: 11-bit message format
 */
union frame {
	struct {
		BYTE canid1;		/**< bits 1-8 of the message identifier */
	    BYTE canid2;		/**< bits 9-16 of the message identifier */
	    BYTE canid3;		/**< bits 17-24 of the message identifier */
	    BYTE canid4;		/**< bits 25-29 of the message identifier */
	    BYTE canxdata[8];	/**< message field (up to 8 bytes) */
	} extframe;				/**< 29-bit message format */
	struct {
		BYTE canid1;		/**< bits 1-8 of the message identifier */
	    BYTE canid2;		/**< bits 9-11 of the message identifier */
	    BYTE candata[8];	/**< message field (up to 8 bytes) */
	} stdframe;				/**< 11-bit message format */
};

/** PeliCAN address allocation for the CAN controller.
 *
 * Each address denotes the location of a byte, whose elements are later
 * described throughout this file. The addresses are in incrementing order
 * starting from canmode (address=0) to canclk (address=31).
 */
typedef struct canregs {
	BYTE canmode;			/**<  0: mode register 							*/
	BYTE cancmd;			/**<  1: command register						*/
	BYTE canstat;			/**<  2: status register						*/
	BYTE canirq;			/**<  3: interrupt register						*/
	BYTE canirq_enable;		/**<  4: interrupt enable register				*/
	BYTE reserved1;			/**<  5: reserved register (read only)			*/
	BYTE cantim0;			/**<  6: bus timing 0 register					*/
	BYTE cantim1;			/**<  7: bus timing 1 register					*/
	BYTE canoutc;			/**<  8: output control register				*/
	BYTE cantest;			/**<  9: command register						*/
	BYTE reserved2;			/**< 10: reserved register (read only)			*/
	BYTE arbitrationlost;	/**< 11: arbitration lost capture only 			*/
	BYTE errorcode;			/**< 12: error code capture (read only) 		*/
	BYTE errorwarninglimit;	/**< 13: error warning limit register			*/
	BYTE rxerror;			/**< 14: RX error counter						*/
	BYTE txerror;			/**< 15: RX error counter 						*/
	BYTE frameinfo;			/**< TODO 										*/
	union frame frame;		/**< TODO 										*/
	BYTE reserved3;			/**< 29: RX message counter (read only)			*/
	BYTE canrxbufferadr;	/**< 30: RX buffer start address (read only)	*/
	BYTE canclk;			/**< 31: clock divider							*/
} __attribute__((packed)) canregs_t;

#define CAN_RANGE 0x20		/* default: 32 registers */


/* --- Mode Register -----------------------------------------------------------
 *
 * The contents of the mode register are used to change the behavior of the CAN
 * controller. Bits may be set or reset by the CPU which uses the control
 * register as a read/write memory. Reserved bits are read as logic 0.
 *
 * Bit 0
 * -----
 * 	0	normal; on the ‘1-to-0’ transition of the reset mode bit, the CAN
 * 	    controller returns to the operating mode
 * 	1	reset; detection of a set reset mode bit results in aborting the current
 * 	    transmission/reception of a message and entering the reset mode
 *
 * Bit 1
 * -----
 * 	0	normal
 * 	1	listen only; in this mode the CAN controller would give no acknowledge
 * 	    to the CAN-bus, even if a message is received successfully; the error
 * 	    counters are stopped at the current value
 *
 * Bit 2
 * -----
 * 	0	normal; an acknowledge is required for successful transmission
 * 	1	self test; in this mode a full node test is possible without any other
 * 	    active node on the bus using the self reception request command; the CAN
 * 	    controller will perform a successful transmission, even if there is no
 * 	    acknowledge received
 *
 * Bit 3
 * -----
 * 	0	dual; the dual acceptance filter option is enabled (two filters, each
 * 	    with the length of 16 bit are active)
 * 	1	single; the single acceptance filter option is enabled (one filter with
 * 	    the length of 32 bit is active)
 *
 * Bit 4
 * -----
 * 	0	wake-up; the CAN controller wakes up if sleeping
 * 	1 	sleep; the CAN controller enters sleep mode if no CAN interrupt
 * 	    is pending and if there is no bus activity
 */

#define CAN_SLEEP_MODE				 0x10   /**< Sleep Mode */
#define CAN_ACC_FILT_MASK			 0x08   /**< Acceptance Filter Mask */
#define CAN_SELF_TEST_MODE			 0x04   /**< Self test mode */
#define CAN_LISTEN_ONLY_MODE		 0x02   /**< Listen only mode */
#define CAN_RESET_REQUEST			 0x01	/**< Reset mode */
#define CAN_MODE_DEF	CAN_ACC_FILT_MASK 	/**< Default ModeRegister Value */


/* --- Interrupt Enable Register -----------------------------------------------
 *
 * The register allows to enable different types of interrupt sources which are
 * indicated to the CPU. A value of 0 implies that the interrupt is disabled,
 * while a value of 1 implies the interrupt is enabled.
 *
 * The interrupt enable register appears to the CPU as a read/write memory.
 *
 * Bit 0 : when the receive buffer status is 'full' the CAN controller requests
 *         the respective interrupt
 * Bit 1 : when a message has been successfully transmitted or the transmit
 *         buffer is accessible again (e.g. after an abort transmission
 *         command), the CAN controller requests the respective interrupt
 * Bit 2 : if the error or bus status change (see status register; Table 14),
 *         the CAN controller requests the respective interrupt
 * Bit 3 : if the data overrun status bit is set (see status register; Table
 *         14), the CAN controller requests the respective interrupt
 * Bit 4 : if the sleeping CAN controller wakes up, the respective interrupt is
 *         requested
 * Bit 5 : if the error status of the CAN controller changes from error active
 *         to error passive or vice versa, the respective interrupt is requested
 * Bit 6 : if the CAN controller has lost arbitration, the respective interrupt
 *         is requested
 * Bit 7 : if an bus error has been detected, the CAN controller requests the
 *         respective interrupt
 */

#define CAN_ERROR_BUSOFF_INT_ENABLE		(1<<7)	/**< bit 7 in Interrupt Enable Register (see */
												/**< include/can/sja1000.h for details) */
#define CAN_ARBITR_LOST_INT_ENABLE		(1<<6)	/**< bit 6 in Interrupt Enable Register (see */
												/**< include/can/sja1000.h for details) */
#define CAN_ERROR_PASSIVE_INT_ENABLE	(1<<5)	/**< bit 5 in Interrupt Enable Register (see */
												/**< include/can/sja1000.h for details) */
#define CAN_WAKEUP_INT_ENABLE			(1<<4)	/**< bit 4 in Interrupt Enable Register (see */
												/**< include/can/sja1000.h for details) */
#define CAN_OVERRUN_INT_ENABLE			(1<<3)	/**< bit 3 in Interrupt Enable Register (see */
												/**< include/can/sja1000.h for details) */
#define CAN_ERROR_INT_ENABLE			(1<<2)	/**< bit 2 in Interrupt Enable Register (see */
												/**< include/can/sja1000.h for details) */
#define CAN_TRANSMIT_INT_ENABLE			(1<<1)	/**< bit 1 in Interrupt Enable Register (see */
												/**< include/can/sja1000.h for details) */
#define CAN_RECEIVE_INT_ENABLE			(1<<0)	/**< bit 0 in Interrupt Enable Register (see */
												/**< include/can/sja1000.h for details) */


/* --- Frame information register ----------------------------------------------
 *
 * 0x00 : CAN message format = standard frame
 *
 * 		Bits    1 : Denotes the start of frame transmission
 * 		Bits   11 : A (unique) identifier which also represents the message
 * 		  		    priority.
 * 		Bits    1 : Must be dominant (0) for data frames and recessive (1) for
 * 		            remote request frames (see Remote Frame, below).
 * 		Bits    1 : Must be dominant (0) for base frame format with 11-bit
 * 				    identifiers
 * 		Bits    1 : Reserved bit. Must be dominant (0), but accepted as either
 * 		            dominant or recessive.
 * 		Bits    4 : Number of bytes of data (0–8 bytes)[a]
 * 		Bits 0–64 : Data to be transmitted (length in bytes dictated by DLC
 * 					field)
 * 		Bits   15 :	Cyclic redundancy check
 * 		Bits    1 : Must be recessive (1)
 * 		Bits    1 :	Transmitter sends recessive (1) and any receiver can assert
 * 					a dominant (0)
 * 		Bits    1 :	ACK delimiter, Must be recessive (1)
 *		Bits 	7 :	End-of-frame (EOF), Must be recessive (1)
 *
 * 0x80 : CAN message format = extended frame
 *
 *		Bits	1 :	Denotes the start of frame transmission
 *		Bits   11 :	First part of the (unique) identifier which also represents
 *					the message priority.
 *		Bits 	1 :	Must be recessive (1)
 *		Bits 	1 :	Must be recessive (1) for extended frame format with 29-bit
 *					identifiers.
 *		Bits   18 :	Second part of the (unique) identifier which also represents
 *					the message priority.
 *		Bits	1 :	Must be dominant (0) for data frames and recessive (1) for
 *					remote request frames (see Remote Frame, below).
 *		Bits	2 :	Reserved bits which must be set dominant (0), but accepted
 *					as either dominant or recessive
 *		Bits	4 :	Number of bytes of data (0–8 bytes)[a]
 *		Bits 0–64 : Data to be transmitted (length dictated by DLC field)
 *		Bits   15 :	Cyclic redundancy check
 *		Bits	1 :	Must be recessive (1)
 *		Bits	1 :	Transmitter sends recessive (1) and any receiver can assert
 *					a dominant (0)
 *		Bits	1 :	ACK delimiter, Must be recessive (1)
 *		Bits	7 :	End-of-frame (EOF), Must be recessive (1)
 */

#define CAN_EFF		0x80	/**< extended frame */
#define CAN_SFF		0x00	/**< standard fame format */


/* --- Command Register --------------------------------------------------------
 *
 * A command bit initiates an action within the transfer layer of the SJA1000.
 * The command register appears to the microcontroller as a write only memory.
 * If a read access is performed to this address the byte '11111111' is
 * returned. Between two commands at least one internal clock cycle is needed to
 * process. The internal clock is divided by two from the external oscillator
 * frequency.
 *
 * Note: For bits 0-3, if the value is set to 0, no action is performed.
 *
 * Bit 0 : If set to 0, no action is performed. If set to 1, a message will be
 *         transmitted.
 * Bit 1 : If set to 0, no action is performed. If set to 1, if not already in
 *         progress, a pending transmission request is cancelled.
 * Bit 2 : If set to 0, no action is performed. If set to 1, the receive buffer,
 *         representing the message memory space in the RXFIFO is released.
 * Bit 3 : If set to 0, no action is performed. If set to 1, the data overrun
 *         status bit is cleared.
 * Bit 4 : If set to 0, the SJA1000 wakes up, i.e. operates normally. If set to
 *         1, the SJA1000 enters sleep mode if no CAN interrupt is pending and
 *         there is no bus activity
 */

#define CAN_GOTO_SLEEP				(1<<4)	/**< bit 4 in Command Register (see */
											/**< include/can/sja1000.h for details) */
#define CAN_CLEAR_OVERRUN_STATUS	(1<<3)	/**< bit 3 in Command Register (see */
											/**< include/can/sja1000.h for details) */
#define CAN_RELEASE_RECEIVE_BUFFER	(1<<2)	/**< bit 2 in Command Register (see */
											/**< include/can/sja1000.h for details) */
#define CAN_ABORT_TRANSMISSION		(1<<1)	/**< bit 1 in Command Register (see */
											/**< include/can/sja1000.h for details) */
#define CAN_TRANSMISSION_REQUEST	(1<<0)	/**< bit 0 in Command Register (see */
											/**< include/can/sja1000.h for details) */


/* --- Status Register ---------------------------------------------------------
 *
 * The content of the status register reflects the status of the SJA1000. The
 * status register appears to the microcontroller as a read only memory.
 *
 * Bit 0
 * -----
 * 	0	empty; no message is available
 * 	1	full; one or more messages are available in the RXFIFO
 *
 * Bit 1
 * -----
 * 	0	absent; no data overrun has occurred since the last clear data overrun
 * 		command was given
 * 	1	overrun; a message was lost because there was not enough space for that
 * 		message in the RXFIFO
 *
 * Bit 2
 * -----
 * 	0	locked; the CPU cannot access the transmit buffer; a message is waiting
 * 		for transmission or is already in process
 * 	1	released; the CPU may write a message into the transmit buffer
 *
 * Bit 3
 * -----
 * 	0	incomplete; the previously requested transmission is not yet completed
 * 	1	complete; the last requested transmission has been successfully
 * 		completed
 *
 * Bit 4
 * -----
 * 	0	idle; no receive message is in progress
 * 	1	receive; the SJA1000 is receiving a messages
 *
 * Bit 5
 * -----
 * 	0	idle; no transmit message is in progress
 * 	1	transmit; the SJA1000 is transmitting a message
 *
 * Bit 6
 * -----
 * 	0	ok; both error counters are below the warning limit
 * 	1	error; at least one of the error counters has reached or exceeded the
 * 		CPU warning limit
 *
 * Bit 7
 * -----
 * 	0	bus-on; the SJA1000 is involved in bus activities
 * 	1	bus-off; the SJA1000 is not involved in bus activities
 */

#define CAN_BUS_STATUS 						(1<<7)	/**< bit 7 in Status Register (see */
													/**< include/can/sja1000.h for details) */
#define CAN_ERROR_STATUS					(1<<6)	/**< bit 6 in Status Register (see */
													/**< include/can/sja1000.h for details) */
#define CAN_TRANSMIT_STATUS					(1<<5)	/**< bit 5 in Status Register (see */
													/**< include/can/sja1000.h for details) */
#define CAN_RECEIVE_STATUS					(1<<4)	/**< bit 4 in Status Register (see */
													/**< include/can/sja1000.h for details) */
#define CAN_TRANSMISSION_COMPLETE_STATUS	(1<<3)	/**< bit 3 in Status Register (see */
													/**< include/can/sja1000.h for details) */
#define CAN_TRANSMIT_BUFFER_ACCESS			(1<<2)	/**< bit 2 in Status Register (see */
													/**< include/can/sja1000.h for details) */
#define CAN_DATA_OVERRUN					(1<<1)	/**< bit 1 in Status Register (see */
													/**< include/can/sja1000.h for details) */
#define CAN_RECEIVE_BUFFER_STATUS			(1<<0)	/**< bit 0 in Status Register (see */
													/**< include/can/sja1000.h for details) */


/* --- Interrupt Register ------------------------------------------------------
 *
 * Bit 0
 * -----
 * 	0	reset; this bit is cleared by any read access of the microcontroller
 * 	1	set; this bit is set while the receive FIFO is not empty and the receive
 * 		interrupt enable bit is set to logic 1 (enabled)
 *
 * Bit 1
 * -----
 * 	0	reset; this bit is cleared by any read access of the microcontroller
 * 	1	set; this bit is set whenever the transmit buffer status changes from
 * 		logic 0 to logic 1 (released) and transmit interrupt enable is set to
 * 		logic 1 (enabled)
 *
 * Bit 2
 * -----
 * 	0	reset; this bit is cleared by any read access of the microcontroller
 * 	1	set; this bit is set on a change of either the error status or bus
 * 		status bits if the error interrupt enable is set to logic 1 (enabled)
 *
 * Bit 3
 * -----
 * 	0	reset; this bit is cleared by any read access of the microcontroller
 * 	1	set; this bit is set on a ‘0-to-1’ transition of the data overrun status
 * 		bit, when the data overrun interrupt enable is set to logic 1 (enabled)
 *
 * Bit 4
 * -----
 * 	0	reset; this bit is cleared by any read access of the microcontroller
 * 	1	set; this bit is set when the sleep mode is left
 */

#define CAN_WAKEUP_INT		(1<<4)	/**< bit 4 in Interrupt Register (see */
									/**< include/can/sja1000.h for details) */
#define CAN_OVERRUN_INT		(1<<3)	/**< bit 3 in Interrupt Register (see */
									/**< include/can/sja1000.h for details) */
#define CAN_ERROR_INT		(1<<2)	/**< bit 2 in Interrupt Register (see */
									/**< include/can/sja1000.h for details) */
#define CAN_TRANSMIT_INT	(1<<1)	/**< bit 1 in Interrupt Register (see */
									/**< include/can/sja1000.h for details) */
#define CAN_RECEIVE_INT 	(1<<0)	/**< bit 0 in Interrupt Register (see */
									/**< include/can/sja1000.h for details) */


/* --- Output Control Register -------------------------------------------------
 *
 *	7		6		5		4		3		2		1		0
 * 	OCTP1	OCTN1	OCPOL1	OCTP0	OCTN0	OCPOL0	OCMODE1	OCMODE0
 *	----------------------  ----------------------  ---------------
 *	TX1 Output		    	TX0 Output		  		Programmable
 *	Driver Control	  		Driver Control	  		Output Functions
 *
 *	MODE
 *	OCMODE1	OCMODE0
 *	0	  	1		Normal Mode; TX0, TX1 bit sequence TXData
 *	1	  	1		Normal Mode; TX0 bit sequence, TX1 busclock TXCLK
 *	0	  	0		Biphase Mode
 *	1	  	0		Test Mode; TX0 bit sequence, TX1 COMPOUT
 *
 * 	In normal Mode Voltage Output Levels depend on Driver Characteristic: OCTPx,
 * 	OCTNx and programmed Output Polarity: OCPOLx
 *
 *	Driver Characteristic
 *	OCTPx	OCTNx
 *	0	 	0		always Floating Outputs,
 *	0	 	1		Pull Down
 *	1	 	0		Pull Up
 *	1	 	1		Push Pull
 */

#define CAN_OCTP1		(1<<7)	/**< bit 8 in Output Control Register (see */
								/**< include/can/sja1000.h for details) */
#define CAN_OCTN1		(1<<6)	/**< bit 7 in Output Control Register (see */
								/**< include/can/sja1000.h for details) */
#define CAN_OCPOL1		(1<<5)	/**< bit 6 in Output Control Register (see */
								/**< include/can/sja1000.h for details) */
#define CAN_OCTP0		(1<<4)	/**< bit 5 in Output Control Register (see */
								/**< include/can/sja1000.h for details) */
#define CAN_OCTN0		(1<<3)	/**< bit 4 in Output Control Register (see */
								/**< include/can/sja1000.h for details) */
#define CAN_OCPOL0		(1<<2)	/**< bit 3 in Output Control Register (see */
								/**< include/can/sja1000.h for details) */
#define CAN_OCMODE1		(1<<1)	/**< bit 2 in Output Control Register (see */
								/**< include/can/sja1000.h for details) */
#define CAN_OCMODE0		(1<<0)	/**< bit 1 in Output Control Register (see */
								/**< include/can/sja1000.h for details) */


/* --- Clock Divider Register --------------------------------------------------
 *
 * The clock divider register controls the CLKOUT frequency for the
 * microcontroller and allows to deactivate the CLKOUT pin. Additionally a
 * dedicated receive interrupt pulse on TX1, a receive comparator bypass and the
 * selection between BasicCAN mode and PeliCAN mode is made here. The default
 * state of the register after hardware reset is divide-by-12 for Motorola mode
 * (00000101) and divide-by-2 for Intel mode (00000000).
 *
 * On software reset (reset request/reset mode) this register is not influenced.
 *
 * The reserved bit (CDR.4) will always reflect a logic 0. The application
 * software should always write a logic 0 to this bit in order to be compatible
 * with future features, which may be 1-active using this bit.
 */

#define CAN_MODE_BASICCAN	(0x00)	/**< Specifies BasicCAN Mode */
#define CAN_MODE_PELICAN	(0xC0)	/**< Specifies Pelican Mode */

/* Original code from can4linux only used CAN_MODE_CLK, which was
 *  defined to the CAN_MODE_CLK1 value shown here. */
#define CAN_MODE_CLK1		(0x07)		/**< CLK-out = Fclk */
#define CAN_MODE_CLK2		(0x00)		/**< CLK-out = Fclk/2 */

/** On the Janus-MM board the correct CLKout is Fclk/2 */
#define CAN_MODE_CLK		CAN_MODE_CLK2


/** --- Remote Request ---------------------------------------------------------
 *
 * Notes:
 *
 * - Basic CAN: RTR is Bit 4 in TXDES1.
 * - Peli  CAN: RTR is Bit 6 in frameinfo.
 */
# define CAN_RTR		(1<<6)


/* these timings are valid for clock 8Mhz */

#define CAN_TIM0_10K	49			/**< BTR0 value for timing of 10 kHz */
#define CAN_TIM1_10K	0x1c		/**< BTR1 value for timing of 10 kHz */
#define CAN_TIM0_20K	24			/**< BTR0 value for timing of 20 kHz */
#define CAN_TIM1_20K	0x1c		/**< BTR1 value for timing of 20 kHz */
#define CAN_TIM0_40K	0x89    	/**< BTR0 value for timing of 40 kHz */
#define CAN_TIM1_40K	0xEB    	/**< BTR1 value for timing of 40 kHz */
#define CAN_TIM0_50K	9			/**< BTR0 value for timing of 50 kHz */
#define CAN_TIM1_50K	0x1c		/**< BTR1 value for timing of 50 kHz */
#define CAN_TIM0_100K	4			/**< BTR0 value for timing of 100 kHz */
#define CAN_TIM1_100K	0x1c		/**< BTR1 value for timing of 100 kHz */
#define CAN_TIM0_125K	3			/**< BTR0 value for timing of 125 kHz */
#define CAN_TIM1_125K	0x1c		/**< BTR1 value for timing of 125 kHz */
#define CAN_TIM0_250K	1			/**< BTR0 value for timing of 250 kHz */
#define CAN_TIM1_250K	0x1c		/**< BTR1 value for timing of 250 kHz */
#define CAN_TIM0_500K	0			/**< BTR0 value for timing of 500 kHz */
#define CAN_TIM1_500K	0x1c		/**< BTR1 value for timing of 500 kHz */
#define CAN_TIM0_800K	0			/**< BTR0 value for timing of 800 kHz */
#define CAN_TIM1_800K	0x16		/**< BTR1 value for timing of 800 kHz */
#define CAN_TIM0_1000K	0			/**< BTR0 value for timing of 1000 kHz */
#define CAN_TIM1_1000K	0x14		/**< BTR1 value for timing of 1000 kHz */


/* External structures may not be used, declared in can_dev.c for compatibility
 * with can4linux sja1000.cpp.
 *
 * For PATH driver, each channel has a separate driver, so MAX_CHANNELS is 1
 */
#define MAX_CHANNELS	1
#define MY_CHANNEL		0

/**
 *	Fancy leveled debugging not really needed in
 *	simpler QNX driver environment.
 */
#define DBGprint(ms,ar) { }
#define DBGin() { }
#define DBGout()        { }
#define DEBUG_TTY(n, args...)
extern unsigned int dbgMask;

/** IOCTL generic CAN controller status request parameter structure */
typedef struct CanStatusPar {
    unsigned int baud;                  /**< actual baud rate */
    unsigned int status;                /**< CAN controller status register */
    unsigned int error_warning_limit;   /**< the error warning limit */
    unsigned int rx_errors;             /**< content of RX error counter */
    unsigned int tx_errors;             /**< content of TX error counter */
    unsigned int error_code;            /**< content of error code register */
    unsigned int rx_buffer_size;        /**< size of rx buffer  */
    unsigned int rx_buffer_used;        /**< number of messages */
    unsigned int tx_buffer_size;        /**< size of tx buffer  */
    unsigned int tx_buffer_used;        /**< number of messages */
    unsigned long retval;               /**< return value */
} CanStatusPar_t;

/** More Linux-specific defines included for can4linux compatibility.
 *
 * Used so that all of sja1000.cpp can compile without change, even  the parts
 * that aren't used by our driver.
 */
struct inode {
	int dummy;
};

struct _instance_data {
	int rx_index;
};

struct file {
	void *private_data;
};

#define MSG_ACTIVE      (0)             /**< Controller Error Active */
#define MSG_BASE        (0)             /**< Base Frame Format */
#define MSG_RTR         (1<<0)          /**< RTR Message */
#define MSG_OVR         (1<<1)          /**< CAN controller Msg overflow error */
#define MSG_EXT         (1<<2)          /**< extended message format */
#define MSG_SELF        (1<<3)          /**< message received from own tx */
#define MSG_PASSIVE     (1<<4)          /**< controller in error passive */
#define MSG_BUSOFF      (1<<5)          /**< controller Bus Off */
#define MSG_WARNING     (1<<6)          /**< CAN Warning Level reached */
#define MSG_BOVR        (1<<7)          /**< receive/transmit buffer overflow */

/** mask used for detecting CAN errors in the canmsg_t flags field */
#define MSG_ERR_MASK    (MSG_OVR+MSG_PASSIVE+MSG_BUSOFF+MSG_BOVR+MSG_WARNING)

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK	0x000007FFU     /**< Standard Frame Format (SFF) 	*/
#define CAN_EFF_MASK	0x1FFFFFFFU     /**< Extended Frame Format (EFF) 	*/
#define CANDRIVERERROR  0xFFFFFFFFul    /**< Invalid CAN ID == Error 		*/


/* Bogus definitions to get simplest case to make Linux ISR in sja1000.cpp
 * compile -- not used. */
#define LINUX_VERSION_CODE		1
#define KERNEL_VERSION(X,Y,Z)	0
typedef int irqreturn_t;
#define IRQ_NONE				0
#define IRQ_HANDLED				1
#define IRQ_RETVAL(x)			(x)


/* -------------------------------------------------------------------------- */
/* -------------------- Imported from sja1000/can_dev.h --------------------- */
/* -------------------------------------------------------------------------- */

/** This must be initialized to the mapped address of the CAN channel.
 */
extern canregs_t *can_base_addr;

/** Read from byte register on CAN chip at address "addr" and return the value.
 */
static inline unsigned char can_read_reg(BYTE *addr)
{
	volatile BYTE *preg = (volatile BYTE*) (addr);
	return *preg;
}

/** Write "value" to byte register on CAN chip at address "addr".
 */
static inline void can_write_reg(BYTE *addr, BYTE value)
{
	volatile BYTE *preg = (volatile BYTE*) (addr);
    *preg = value;
}

/** Set bits in "mask" in byte register on CAN chip at address "addr"
 *
 * Register must be readable and then writable.
 */
static inline void can_set_reg(BYTE *addr, BYTE mask)
{
	volatile BYTE *preg = (volatile BYTE*) (addr);
	*preg |= mask;
}

/** Clear the bits in "mask" in byte register on CAN chip at address "addr"
 *
 * Register must be readable and then writable.
 */
static inline void can_reset_reg(BYTE *addr, BYTE mask)
{
	volatile BYTE *preg = (volatile BYTE *) (addr);
	*preg &= ~(mask);
}

/* Board access macros, as used in can4linux, rely on a packed structure for
 * canregs_t that mirrors the offsets of the actual registers.
 *
 * Unlike can4linux, each instance of the can_man resource manager corresponds
 * to one SJA 1000 chip and one CAN port. Unused bd (board) parameter to macros
 * is retained for compatibility in sja1000.cpp.
 */
#define CANin(bd,adr) can_read_reg(&can_base_addr->adr)
#define CANout(bd,adr,v) can_write_reg(&can_base_addr->adr, v)
#define CANset(bd,adr,m) can_set_reg(&can_base_addr->adr, m)
#define CANreset(bd,adr,m) can_reset_reg(&can_base_addr->adr, m)


#ifdef CPC_PCI
#define R_OFF 4 /* offset 4 for the EMS CPC-PCI card */
#else
#define R_OFF 1
#endif

#endif /* INCLUDE_CAN_SJA1000_H_ */
