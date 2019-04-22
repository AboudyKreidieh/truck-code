/**\file
 *
 * sja1000.cpp
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date April 6, 2019
 */

#include "sja1000.h"
#include "utils/common.h"
#include "utils/timestamp.h"
#include <cstring>			/* memcpy */

using namespace std;


#if defined(CAN_INDEXED_PORT_IO) || defined(CAN_INDEXED_MEM_IO)
canregs_t* regbase = 0;
#endif


///** Read back as many status information as possible
// *
// * Because the CAN protocol itself describes different kind of information
// * already, and the driver has some generic information
// * (e.g about it's buffers)
// * we can define a more or less hardware independent format.
// *
// * exception:
// * ERROR WARNING LIMIT REGISTER (EWLR)
// * The SJA1000 defines a EWLR, reaching this Error Warning Level
// * an Error Warning interrupt can be generated.
// * The default value (after hardware reset) is 96. In reset
// * mode this register appears to the CPU as a read/write
// * memory. In operating mode it is read only.
// * Note, that a content change of the EWLR is only possible,
// * if the reset mode was entered previously. An error status
// * change (see status register; Table 14) and an error
// * warning interrupt forced by the new register content will not
// * occur until the reset mode is cancelled again.
// */
//int can_GetStat(inode *inode, file *file, CanStatusPar_t *stat)
//{
//	unsigned int board = iminor(inode);
//	msg_fifo_t *Fifo;
//	unsigned long flags;
//	int rx_fifo = ((struct _instance_data *)(file->private_data))->rx_index;
//
//    stat->baud = Baud[board];
//    stat->status = CANin(board, canstat);
//    stat->error_warning_limit = CANin(board, errorwarninglimit);
//    stat->rx_errors = CANin(board, rxerror);
//    stat->tx_errors = CANin(board, txerror);
//    stat->error_code= CANin(board, errorcode);
//
//    local_irq_save(flags);
//
//    Fifo = &Rx_Buf[board][rx_fifo];
//    stat->rx_buffer_size = MAX_BUFSIZE;	/**< size of rx buffer  */
//
//    /* number of messages */
//    stat->rx_buffer_used =
//    	(Fifo->head < Fifo->tail)
//    	? (MAX_BUFSIZE - Fifo->tail + Fifo->head) : (Fifo->head - Fifo->tail);
//    Fifo = &Tx_Buf[board];
//    stat->tx_buffer_size = MAX_BUFSIZE;	/* size of tx buffer  */
//
//    /* number of messages */
//    stat->tx_buffer_used =
//    	(Fifo->head < Fifo->tail)
//    	? (MAX_BUFSIZE - Fifo->tail + Fifo->head) : (Fifo->head - Fifo->tail);
//
//    local_irq_restore(flags);
//    return 0;
//}


/*
 * The plain SJA1000 interrupt
 *
 *				RX ISR           TX ISR
 *                              8/0 byte
 *                               _____            _   ___
 *                         _____|     |____   ___| |_|   |__
 *---------------------------------------------------------------------------
 * 1) CPUPentium 75 - 200
 *  75 MHz, 149.91 bogomips
 *  AT-CAN-MINI                 42/27µs            50 µs
 *  CPC-PCI		        35/26µs
 * ---------------------------------------------------------------------------
 * 2) AMD Athlon(tm) Processor 1M
 *    2011.95 bogomips
 *  AT-CAN-MINI  std            24/12µs            ?? µs
 *               ext id           /14µs
 *
 *
 * 1) 1Byte = (42-27)/8 = 1.87 µs
 * 2) 1Byte = (24-12)/8 = 1.5  µs
 *
 *
 *
 * RX Int with to Receive channels:
 * 1)                _____   ___
 *             _____|     |_|   |__
 *                   30    5  20  µs
 *   first ISR normal length,
 *   time between the two ISR -- short
 *   sec. ISR shorter than first, why? it's the same message
 */

//#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,18)
//irqreturn_t CAN_Interrupt(int irq, void *dev_id)
//#else
//irqreturn_t CAN_Interrupt( int irq, void *dev_id, struct pt_regs *notused)
//#endif
//{
//	int minor;
//	int i = 0;
//	int rx_fifo;
//	timeval timestamp;
//	unsigned long flags;
//	int ext;				/* flag for extended message format */
//	int irqsrc;
//	int	dummy;
//	msg_fifo_t *RxFifo;
//	msg_fifo_t *TxFifo;
//#if CAN_USE_FILTER
//	msg_filter_t *RxPass;
//unsigned int id;
//#endif
//#if 1
//	int first = 0;
//#endif
//
//#if CONFIG_TIME_MEASURE
//    /* set port to high */
//    set_measure_pin();
//#endif
//
//    minor = *(int *)dev_id;
//
//    RxFifo = &Rx_Buf[minor][0];
//    TxFifo = &Tx_Buf[minor];
//#if CAN_USE_FILTER
//    RxPass = &Rx_Filter[minor];
//#endif
//
//    irqsrc = CANin(minor, canirq);
//    if(irqsrc == 0) {
//         /* first call to ISR, it's not for me ! */
//#if CONFIG_TIME_MEASURE
//    	reset_measure_pin();
//#endif
//#if LINUX_VERSION_CODE >= 0x020500
//    	return IRQ_RETVAL(IRQ_NONE);
//#else
//    	goto IRQdone_doneNothing;
//#endif
//    }
//
//#if defined(CCPC104)
//	pc104_irqack();
//#endif
//
//    do {
//		/* loop as long as the CAN controller shows interrupts */
//		DBGprint(DBG_DATA, (" => got IRQ[%d]: 0x%0x", minor, irqsrc));
//
//		/* fill timestamp as first action.
//		 * Getting a precises time takes a lot of time
//		 * (additional 7 µs of ISR time )
//		 * if a time stamp is not needed, it can be switched of
//		 * by ioctl() */
//		if (use_timestamp[minor]) {
//			do_gettimeofday(&timestamp);
//		} else {
//			timestamp.tv_sec = 0;
//			timestamp.tv_usec = 0;
//		}
//
//		for(rx_fifo = 0; rx_fifo < CAN_MAX_OPEN; rx_fifo++) {
//			RxFifo = &Rx_Buf[minor][rx_fifo];
//			RxFifo->data[RxFifo->head].timestamp = timestamp;
//
//			/* preset flags */
//			(RxFifo->data[RxFifo->head]).flags =
//					(RxFifo->status & BUF_OVERRUN ? MSG_BOVR : 0);
//		}
//
//#if 1
//    /* how often do we loop through the ISR ? */
//    /* if(first) printk("n = %d\n", first); */
//    first++;
//    if (first > 10) return IRQ_RETVAL(IRQ_HANDLED);
//#endif
//
//    /*========== receive interrupt */
//    if( irqsrc & CAN_RECEIVE_INT ) {
//    	int length;
//
//        /* printk(" CAN RX %d\n", minor); */
//	dummy  = CANin(minor, frameinfo );
//
//	/* ---------- fill frame data -------------------------------- */
//
//	/* handle all subscribed rx fifos */
//    for(rx_fifo = 0; rx_fifo < CAN_MAX_OPEN; rx_fifo++) {
//    	/* for every rx fifo */
//		if (CanWaitFlag[minor][rx_fifo] == 1) {
//			/* this FIFO is in use */
//			RxFifo = &Rx_Buf[minor][rx_fifo];  /* prepare buffer to be used */
//
//			if( dummy & CAN_RTR ) {
//				(RxFifo->data[RxFifo->head]).flags |= MSG_RTR;
//			}
//
//			if( dummy & CAN_EFF ) {
//				(RxFifo->data[RxFifo->head]).flags |= MSG_EXT;
//			}
//
//			ext = (dummy & CAN_EFF);	/* check whether extended format */
//			if (ext) {
//				(RxFifo->data[RxFifo->head]).id =
//				  ((unsigned int)(CANin(minor, frame.extframe.canid1)) << 21)
//				  + ((unsigned int)(CANin(minor, frame.extframe.canid2)) << 13)
//				  + ((unsigned int)(CANin(minor, frame.extframe.canid3)) << 5)
//				  + ((unsigned int)(CANin(minor, frame.extframe.canid4)) >> 3);
//			} else {
//				(RxFifo->data[RxFifo->head]).id =
//				  ((unsigned int)(CANin(minor, frame.stdframe.canid1 )) << 3)
//				  + ((unsigned int)(CANin(minor, frame.stdframe.canid2 )) >> 5);
//			}
//
//			/* get message length */
//			length = dummy & 0x0f;			/* strip length code */
//			(RxFifo->data[RxFifo->head]).length = length;
//
//			length %= 9;	/* limit count to 8 bytes */
//			for( i = 0; i < length; i++) {
//				if (ext) {
//					(RxFifo->data[RxFifo->head]).data[i] =
//							CANin(minor, frame.extframe.canxdata[R_OFF * i]);
//				} else {
//					(RxFifo->data[RxFifo->head]).data[i] =
//							CANin(minor, frame.stdframe.candata[R_OFF * i]);
//				}
//			}
//
//			/* mark just written entry as OK and full */
//			RxFifo->status = BUF_OK;
//
//			/* Handle buffer wrap-around */
//			RxFifo->head = (RxFifo->head + 1) % MAX_BUFSIZE;
//			if(RxFifo->head == RxFifo->tail) {
//				printk("CAN[%d][%d] RX: FIFO overrun\n", minor, rx_fifo);
//				RxFifo->status = BUF_OVERRUN;
//			}
//
//			/*---------- kick the select() call  -*/
//			/* This function will wake up all processes
//			   that are waiting on this event queue,
//			   that are in interruptible sleep
//			*/
//		/* printk(KERN_INFO " should wake [%d][%d]\n", minor, rx_fifo); */
//			wake_up_interruptible(&CanWait[minor][rx_fifo]);
//		}
//    }
///* ---------- / fill frame data -------------------------------- */
//
//        /* release the CAN controller now */
//        CANout(minor, cancmd, CAN_RELEASE_RECEIVE_BUFFER );
//        if(CANin(minor, canstat) & CAN_DATA_OVERRUN ) {
//        	printk("CAN[%d] Rx: Overrun Status \n", minor);
//        	CANout(minor, cancmd, CAN_CLEAR_OVERRUN_STATUS );
//        }
//   }
//
//    /*========== transmit interrupt */
//    if( irqsrc & CAN_TRANSMIT_INT ) {
//	/* CAN frame successfully sent */
//    BYTE tx2reg;
//	unsigned int id;
//
//	/* use time stamp sampled with last INT */
//	last_Tx_object[minor].timestamp = timestamp;
//
//	/* Depending on the number of open processes the TX data has to be copied
//	 * in different Rx fifos. */
//	for(rx_fifo = 0; rx_fifo < CAN_MAX_OPEN; rx_fifo++) {
//	    /* for every rx fifo */
//	    if (CanWaitFlag[minor][rx_fifo] == 1) {
//			/* Don't copy the message in the receive queue of the process that
//			 * sent the message unless this process requested self-reception. */
//	    	if ((last_Tx_object[minor].cob == rx_fifo) &&
//	    			(selfreception[minor][rx_fifo] == 0)) {
//	    		continue;
//	    	}
//
//#ifdef VERBOSE
//	    	printk("CAN[%d][%d] Copy message from %d in queue id 0x%lx 0x%x\n",
//	    			minor, rx_fifo,
//			last_Tx_object[minor].cob,
//			last_Tx_object[minor].id,
//			last_Tx_object[minor].data[0]);
//#endif
//			/* prepare buffer to be used */
//			RxFifo = &Rx_Buf[minor][rx_fifo];
//
//			memcpy((void *)&RxFifo->data[RxFifo->head],
//					(void *)&last_Tx_object[minor],
//					sizeof(canmsg_t));
//
//		/* Mark message as 'self sent/received' */
//		RxFifo->data[RxFifo->head].flags |= MSG_SELF;
//
//		/* increment write index */
//		RxFifo->status = BUF_OK;
//		RxFifo->head = ++(RxFifo->head) % MAX_BUFSIZE;
//
//		if(RxFifo->head == RxFifo->tail) {
//		    printk("CAN[%d][%d] RX: FIFO overrun\n", minor, rx_fifo);
//		    RxFifo->status = BUF_OVERRUN;
//		}
//		/*---------- kick the select() call  -*/
//		/* This function will wake up all processes
//		   that are waiting on this event queue,
//		   that are in interruptible sleep
//		*/
//		wake_up_interruptible(&CanWait[minor][rx_fifo]);
//	    } /* this FIFO is in use */
//	} /* end for loop filling all rx-fifos */
//
//
//	if( TxFifo->free[TxFifo->tail] == BUF_EMPTY ) {
//	    /* TX FIFO empty, nothing more to sent */
//	    /* printk("TXE\n"); */
//	    TxFifo->status = BUF_EMPTY;
//            TxFifo->active = 0;
//	    /* This function will wake up all processes
//	       that are waiting on this event queue,
//	       that are in interruptible sleep
//	    */
//	    wake_up_interruptible(&CanOutWait[minor]);
//	    goto Tx_done;
//	}
//
//        /* enter critical section */
//	local_irq_save(flags);
//
//	/* The TX message FIFO contains other CAN frames to be sent
//	 * The next frame in the FIFO is copied into the last_Tx_object
//	 * and directly into the hardware of the CAN controller
//	 */
//	memcpy(
//		(void *)&last_Tx_object[minor],
//		(void *)&TxFifo->data[TxFifo->tail],
//		sizeof(canmsg_t));
//
//	tx2reg = (TxFifo->data[TxFifo->tail]).length;
//	if( (TxFifo->data[TxFifo->tail]).flags & MSG_RTR) {
//		tx2reg |= CAN_RTR;
//	}
//
//	ext = (TxFifo->data[TxFifo->tail]).flags & MSG_EXT;
//	id = (TxFifo->data[TxFifo->tail]).id;
//	if(ext) {
//	    DBGprint(DBG_DATA, ("---> send ext message"));
//	    CANout(minor, frameinfo, CAN_EFF + tx2reg);
//	    CANout(minor, frame.extframe.canid1, (BYTE)(id >> 21));
//	    CANout(minor, frame.extframe.canid2, (BYTE)(id >> 13));
//	    CANout(minor, frame.extframe.canid3, (BYTE)(id >> 5));
//	    CANout(minor, frame.extframe.canid4, (BYTE)(id << 3) & 0xff);
//	} else {
//	    DBGprint(DBG_DATA, ("---> send std message"));
//	    CANout(minor, frameinfo, CAN_SFF + tx2reg);
//	    CANout(minor, frame.stdframe.canid1, (BYTE)((id) >> 3) );
//	    CANout(minor, frame.stdframe.canid2, (BYTE)(id << 5 ) & 0xe0);
//	}
//
//	tx2reg &= 0x0f;  /* restore length only */
//	if (ext) {
//	    for(i=0; i<tx2reg; i++) {
//			CANout(minor, frame.extframe.canxdata[R_OFF * i],
//				(TxFifo->data[TxFifo->tail]).data[i]);
//	    }
//	} else {
//	    for(i=0; i<tx2reg; i++) {
//			CANout(minor, frame.stdframe.candata[R_OFF * i],
//				(TxFifo->data[TxFifo->tail]).data[i]);
//	    }
//	}
//
//	CANout(minor, cancmd, CAN_TRANSMISSION_REQUEST);
//
//	TxFifo->free[TxFifo->tail] = BUF_EMPTY; /* now this entry is EMPTY */
//	TxFifo->tail = ++(TxFifo->tail) % MAX_BUFSIZE;
//
//        /* leave critical section */
//	local_irq_restore(flags);
//    }
//
//    Tx_done:
//
//	/*========== error status */
//	if(irqsrc & CAN_ERROR_INT) {
//		int status;
//		int flags = 0;
//
//		TxErr[minor]++;
//
//       /* insert error */
//       status = CANin(minor,canstat);
//       if (status & CAN_BUS_STATUS ) {
//    	   flags += MSG_BUSOFF;
//    	   TxErr[minor]++;
//    	   (RxFifo->data[RxFifo->head]).flags += MSG_BUSOFF;
//       }
//
//       if(status & CAN_ERROR_STATUS) {
//    	   flags += MSG_PASSIVE;
//    	   TxErr[minor]++;
//    	   (RxFifo->data[RxFifo->head]).flags += MSG_PASSIVE;
//       }
//
//       for(rx_fifo = 0; rx_fifo < CAN_MAX_OPEN; rx_fifo++) {
//    	   /* for every rx fifo */
//    	   if (CanWaitFlag[minor][rx_fifo] == 1) {
//    		   /* this FIFO is in use */
//    		   RxFifo = &Rx_Buf[minor][rx_fifo]; /* prepare buffer to be used */
//    		   (RxFifo->data[RxFifo->head]).flags += flags;
//    		   (RxFifo->data[RxFifo->head]).id = CANDRIVERERROR;
//    		   (RxFifo->data[RxFifo->head]).length = 0;
//    		   RxFifo->status = BUF_OK;
//
//    		   /* handle fifo wrap around */
//    		   RxFifo->head = ++(RxFifo->head) % MAX_BUFSIZE;
//
//    		   if(RxFifo->head == RxFifo->tail) {
//    			   printk("CAN[%d][%d] RX: FIFO overrun\n", minor, rx_fifo);
//    			   RxFifo->status = BUF_OVERRUN;
//    		   }
//
//    		   /* tell someone that there is a new error message */
//    		   wake_up_interruptible(&CanWait[minor][rx_fifo]);
//    	   }
//       }
//   }
//
//   if( irqsrc & CAN_OVERRUN_INT ) {
//   int status;
//	printk("CAN[%d]: controller overrun!\n", minor);
//        Overrun[minor]++;
//
//        /* insert error */
//        status = CANin(minor,canstat);
//        RxErr[minor]++;
//
//	for(rx_fifo = 0; rx_fifo < CAN_MAX_OPEN; rx_fifo++) {
//	    /* for every rx fifo */
//	    if (CanWaitFlag[minor][rx_fifo] == 1) {
//		/* this FIFO is in use */
//		if(status & CAN_DATA_OVERRUN)
//		    (RxFifo->data[RxFifo->head]).flags += MSG_OVR;
//
//		(RxFifo->data[RxFifo->head]).id = 0xFFFFFFFF;
//		(RxFifo->data[RxFifo->head]).length = 0;
//		RxFifo->status = BUF_OK;
//		RxFifo->head = ++(RxFifo->head) % MAX_BUFSIZE;
//		if(RxFifo->head == RxFifo->tail) {
//		    printk("CAN[%d][%d] RX: FIFO overrun\n", minor, rx_fifo);
//		    RxFifo->status = BUF_OVERRUN;
//		}
//
//		/* tell someone that there is a new error message */
//		wake_up_interruptible(&CanWait[minor][rx_fifo]);
//	    }
//	}
//
//	CANout(minor, cancmd, CAN_CLEAR_OVERRUN_STATUS);
//   }
//   } while((irqsrc = CANin(minor, canirq)) != 0);
//
//    /* IRQdone: */
//    DBGprint(DBG_DATA, (" => leave IRQ[%d]", minor));
//
//#if LINUX_VERSION_CODE < 0x020500
//    IRQdone_doneNothing:
//#endif
//
//#if CONFIG_TIME_MEASURE
//    /* outb(0x00, 0x378);   */
//    reset_measure_pin();
//#endif
//
//    return IRQ_RETVAL(IRQ_HANDLED);
//}
