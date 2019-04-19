/**\file
 *
 * sja1000.h
 *
 * Simplified from can4linux 3.5.4 to match only Janus MM board
 * Timing values only for 8MHz included (assume 16MHz oscillator
 * on Janus MM is halved to be 8MHz, if not we need to change.)
 *
 * This includes device-dependent CAN definitions for the Phillips SJA 1000
 * driver. Assumes board is memory mapped at address given during initialization.
 *
 *  Created on: Apr 6, 2019
 *      Author: aboudy
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
 * This includes the CAN message identifier and the data field.
 *
 * This object contains two substructures:
 * 	- extframe: 29-bit message format
 * 	- stdframe: 11-bit message format
 */
union frame {
	struct {
		BYTE canid1;		/**< bits 1-8 of the message identifier */
	    BYTE canid2;		/**< bits 9-16 of the message identifier */
	    BYTE canid3;		/**< bits 17-24 of the message identifier */
	    BYTE canid4;		/**< bits 25-29 of the message identifier */
	    BYTE canxdata[8];	/**< message field (up to 8 bytes) */
	} extframe;
	struct {
		BYTE canid1;		/**< bits 1-8 of the message identifier */
	    BYTE canid2;		/**< bits 9-11 of the message identifier */
	    BYTE candata[8];	/**< message field (up to 8 bytes) */
	} stdframe;
};

/** TODO
 *
 */
typedef struct canregs {
	BYTE canmode;			/* 0 */
	BYTE cancmd;
	BYTE canstat;
	BYTE canirq;
	BYTE canirq_enable;
	BYTE reserved1;			/* 5 */
	BYTE cantim0;
	BYTE cantim1;
	BYTE canoutc;
	BYTE cantest;
	BYTE reserved2;			/* 10 */
	BYTE arbitrationlost;	/* read only */
	BYTE errorcode;			/* read only */
	BYTE errorwarninglimit;
	BYTE rxerror;
	BYTE txerror;			/* 15 */
	BYTE frameinfo;
	union frame frame;
	BYTE reserved3;
	BYTE canrxbufferadr;	/* 30 */
	BYTE canclk;
} __attribute__((packed)) canregs_t;

#define CAN_RANGE 0x20		/* default: 32 registers */


/* --- Mode Register -----------------------------------------------------------
 *
 * PeliCAN
 * TODO
 */

#define CAN_SLEEP_MODE				 0x10   /**< Sleep Mode */
#define CAN_ACC_FILT_MASK			 0x08   /**< Acceptance Filter Mask */
#define CAN_SELF_TEST_MODE			 0x04   /**< Self test mode */
#define CAN_LISTEN_ONLY_MODE		 0x02   /**< Listen only mode */
#define CAN_RESET_REQUEST			 0x01	/**< Reset mode */
#define CAN_MODE_DEF	CAN_ACC_FILT_MASK 	/**< Default ModeRegister Value */


/* --- Interrupt Enable Register -----------------------------------------------
 *
 * Bit 0 : blank
 * Bit 1 : blank
 * Bit 2 : blank
 * Bit 3 : blank
 * Bit 4 : blank
 * Bit 5 : blank
 * Bit 6 : blank
 * Bit 7 : blank
 */

#define CAN_ERROR_BUSOFF_INT_ENABLE		(1<<7)	/**< TODO */
#define CAN_ARBITR_LOST_INT_ENABLE		(1<<6)	/**< TODO */
#define CAN_ERROR_PASSIVE_INT_ENABLE	(1<<5)	/**< TODO */
#define CAN_WAKEUP_INT_ENABLE			(1<<4)	/**< TODO */
#define CAN_OVERRUN_INT_ENABLE			(1<<3)	/**< TODO */
#define CAN_ERROR_INT_ENABLE			(1<<2)	/**< TODO */
#define CAN_TRANSMIT_INT_ENABLE			(1<<1)	/**< TODO */
#define CAN_RECEIVE_INT_ENABLE			(1<<0)	/**< TODO */


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
 * Bit 0 : blank
 * Bit 1 : blank
 * Bit 2 : blank
 * Bit 3 : blank
 * Bit 4 : blank
 */

#define CAN_GOTO_SLEEP				(1<<4)	/**< TODO */
#define CAN_CLEAR_OVERRUN_STATUS	(1<<3)	/**< TODO */
#define CAN_RELEASE_RECEIVE_BUFFER	(1<<2)	/**< TODO */
#define CAN_ABORT_TRANSMISSION		(1<<1)	/**< TODO */
#define CAN_TRANSMISSION_REQUEST	(1<<0)	/**< TODO */


/* --- Status Register ---------------------------------------------------------
 *
 * Bit 0 : blank
 * Bit 1 : blank
 * Bit 2 : blank
 * Bit 3 : blank
 * Bit 4 : blank
 * Bit 5 : blank
 * Bit 6 : blank
 * Bit 7 : blank
 */

#define CAN_BUS_STATUS 						(1<<7)	/**< TODO */
#define CAN_ERROR_STATUS					(1<<6)	/**< TODO */
#define CAN_TRANSMIT_STATUS					(1<<5)	/**< TODO */
#define CAN_RECEIVE_STATUS					(1<<4)	/**< TODO */
#define CAN_TRANSMISSION_COMPLETE_STATUS	(1<<3)	/**< TODO */
#define CAN_TRANSMIT_BUFFER_ACCESS			(1<<2)	/**< TODO */
#define CAN_DATA_OVERRUN					(1<<1)	/**< TODO */
#define CAN_RECEIVE_BUFFER_STATUS			(1<<0)	/**< TODO */


/* --- Interrupt Register ------------------------------------------------------
 *
 * Bit 0 : blank
 * Bit 1 : blank
 * Bit 2 : blank
 * Bit 3 : blank
 * Bit 4 : blank
 */

#define CAN_WAKEUP_INT		(1<<4)
#define CAN_OVERRUN_INT		(1<<3)
#define CAN_ERROR_INT		(1<<2)
#define CAN_TRANSMIT_INT	(1<<1)
#define CAN_RECEIVE_INT 	(1<<0)


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
 * In normal Mode Voltage Output Levels depend on Driver Characteristic: OCTPx,
 * OCTNx and programmed Output Polarity: OCPOLx
 *
 *	Driver Characteristic
 *	OCTPx	OCTNx
 *	0	 	0		always Floating Outputs,
 *	0	 	1		Pull Down
 *	1	 	0		Pull Up
 *	1	 	1		Push Pull
 */

#define CAN_OCTP1		(1<<7)
#define CAN_OCTN1		(1<<6)
#define CAN_OCPOL1		(1<<5)
#define CAN_OCTP0		(1<<4)
#define CAN_OCTN0		(1<<3)
#define CAN_OCPOL0		(1<<2)
#define CAN_OCMODE1		(1<<1)
#define CAN_OCMODE0		(1<<0)


/* --- Clock Divider register ---------------------------------*/

#define CAN_MODE_BASICCAN	(0x00)	/**< TODO */
#define CAN_MODE_PELICAN	(0xC0)	/**< TODO */

/* Original code from can4linux only used CAN_MODE_CLK, which was
 *  defined to the CAN_MODE_CLK1 value shown here. */
#define CAN_MODE_CLK1			(0x07)		/**< CLK-out = Fclk */
#define CAN_MODE_CLK2			(0x00)		/**< CLK-out = Fclk/2 */

/** On the Janus-MM board the correct CLKout is Fclk/2 */
#define CAN_MODE_CLK	CAN_MODE_CLK2


/** --- Remote Request ---------------------------------------------------------
 *
 * Notes:
 * - Basic CAN: RTR is Bit 4 in TXDES1.
 * - Peli  CAN: RTR is Bit 6 in frameinfo.
 */
# define CAN_RTR				(1<<6)


/* these timings are valid for clock 8Mhz */

#define CAN_TIM0_10K	49
#define CAN_TIM1_10K	0x1c
#define CAN_TIM0_20K	24
#define CAN_TIM1_20K	0x1c
#define CAN_TIM0_40K	0x89    /* Old Bit Timing Standard of port */
#define CAN_TIM1_40K	0xEB    /* Old Bit Timing Standard of port */
#define CAN_TIM0_50K	9
#define CAN_TIM1_50K	0x1c
#define CAN_TIM0_100K	4		/* sp 87%, 16 abtastungen, sjw 1 */
#define CAN_TIM1_100K	0x1c
#define CAN_TIM0_125K	3
#define CAN_TIM1_125K	0x1c
#define CAN_TIM0_250K	1
#define CAN_TIM1_250K	0x1c
#define CAN_TIM0_500K	0
#define CAN_TIM1_500K	0x1c
#define CAN_TIM0_800K	0
#define CAN_TIM1_800K	0x16
#define CAN_TIM0_1000K	0
#define CAN_TIM1_1000K	0x14


/* External structures may not be used, declared in can_dev.c for compatibility
 * with can4linux sja1000.cpp.
 *
 * For PATH driver, each channel has a separate driver, so MAX_CHANNELS is 1
 */
#define MAX_CHANNELS	1
#define MY_CHANNEL		0

extern int IRQ[MAX_CHANNELS];
extern int Timeout[MAX_CHANNELS];
extern int Outc[MAX_CHANNELS];
extern int TxErr[MAX_CHANNELS];
extern int RxErr[MAX_CHANNELS];
extern int Overrun[MAX_CHANNELS];
extern int Options[MAX_CHANNELS];
extern atomic_t Can_isopen[MAX_CHANNELS];

// value for Janus MM board
#define CAN_OUTC_VAL	0xda

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

#define iminor(x)	((intptr_t) (x))	/**< TODO */

#define local_irq_save(flags)			/** < TODO */
#define local_irq_restore(flags)		/** < TODO */
#define schedule()						/** < TODO */
#define need_resched				0	/** < TODO */

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
