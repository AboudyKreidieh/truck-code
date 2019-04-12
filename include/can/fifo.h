/**\file
 *
 * fifo.h
 *
 *  Created on: Apr 9, 2019
 *      Author: aboudy
 */

#ifndef INCLUDE_CAN_FIFO_H_
#define INCLUDE_CAN_FIFO_H_

#include "sja1000.h"


#define MAX_BUFSIZE		1	/**< Maximum number of elements to place in the */
							/**< CAN Rx/Tx buffer */
#define CAN_MAX_OPEN	1	/**< Maximum number of CAN cards to receive */
							/**< messages from */
#define BUF_EMPTY		0	/**< Specifies that the FIFO buffer is empty */
#define BUF_OK			1	/**< Specifies that the FIFO buffer is full */
#define BUF_OVERRUN		2	/**< Specifies whether the FIFO buffer has been */
							/**< over-filled */


/** FIFO object for storing CAN messages.
 *
 * Elements are added to and removed from the buffer in a circular fashion; i.e.
 * when the previous head in at index MAX_BUFFSIZE, the new head will be placed
 * at index 0.
 */
typedef struct {
	int head;					/**< the index of the head CAN message in the */
								/**< buffer */
	int tail;					/**< the index of the tail CAN message in the */
								/**< buffer */
	int status;					/**< status of the buffer, one of BUF_EMPTY, */
								/**< BUF_OK, or BUF_OVERRUN */
	int active;					/**< TODO */
	char free[MAX_BUFSIZE];		/**< TODO */
	canmsg_t data[MAX_BUFSIZE];	/**< stores the content of the CAN messages */
								/**< in the FIFO buffer */
} msg_fifo_t;


/* TODO: remove globals */
//extern canmsg_t last_Tx_object[MAX_CHANNELS];
//extern msg_fifo_t Rx_Buf[MAX_BUFSIZE][CAN_MAX_OPEN];
//extern msg_fifo_t Tx_Buf[MAX_BUFSIZE];
extern int IRQ_requested[];								/**< TODO */
extern int Can_minors[];                        		/**< used as IRQ dev_id */
extern int selfreception[MAX_CHANNELS][CAN_MAX_OPEN];	/**< flag 1 = On */
extern int use_timestamp[MAX_CHANNELS];                 /**< flag  1 = On */
extern int wakeup[];									/**< flag  1 = On */
extern int listenmode;									/**< true for listen only */
typedef int wait_queue_head_t;
extern wait_queue_head_t CanWait[MAX_CHANNELS][CAN_MAX_OPEN];
extern wait_queue_head_t CanOutWait[MAX_CHANNELS];
extern int CanWaitFlag[MAX_CHANNELS][CAN_MAX_OPEN];


#endif /* INCLUDE_CAN_FIFO_H_ */
