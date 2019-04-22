/**\file
 *
 * can_dev.cpp
 *
 * Device level code, based on device-level can4linux-3.5.4 code for the
 * Phillips SJA 1000.
 *
 * See SJA1000_3.pdf and AN9076.pdf in ../doc and documentation in ./sja1000.h
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 7, 2019
 */

#include <sys/mman.h>
#include "can_man.h"
#include "utils/buffer.h"
#include "sja1000.h"
#include "delay.h"		/* atomic_t */


canregs_t *can_base_addr;	//FIXME

/* timing values */
static BYTE CanTiming[10][2] = {
	{CAN_TIM0_10K,  CAN_TIM1_10K},
	{CAN_TIM0_20K,  CAN_TIM1_20K},
	{CAN_TIM0_50K,  CAN_TIM1_50K},
	{CAN_TIM0_100K, CAN_TIM1_100K},
	{CAN_TIM0_125K, CAN_TIM1_125K},
	{CAN_TIM0_250K, CAN_TIM1_250K},
	{CAN_TIM0_500K, CAN_TIM1_500K},
	{CAN_TIM0_800K, CAN_TIM1_800K},
	{CAN_TIM0_1000K,CAN_TIM1_1000K}
};


void CANDeviceManager::init(unsigned int base_address, unsigned int bit_speed,
		BYTE extended_frame) {
	this->_base_addr = (canregs_t *) mmap_device_memory(NULL,
		SJA1000_MAP_SIZE, PROT_READ | PROT_WRITE | PROT_NOCACHE,
		0, base_address);
	can_base_addr = this->_base_addr;	// FIXME

	printf("Using memory mapped access, 0x%08x mapped to 0x%08x\n",
		base_address, (intptr_t) this->_base_addr);
	printf("speed %d, %s\n", bit_speed,
		extended_frame?"extended frame":"standard frame");
	fflush(stdout);

	/** Set up variables used to set up timing and acceptance. */
	this->_baud[MY_CHANNEL] = bit_speed;
	this->_acc_code[MY_CHANNEL] = 0x00000000;
	this->_acc_mask[MY_CHANNEL] = 0xFFFFFFFF;  // Accept everything

	if (this->_reset_chip(MY_CHANNEL) < 0)
		printf("Error returned from SJA1000 reset\n");
	else
		this->_start_chip(MY_CHANNEL);

	printf("Phillips SJA1000 started, mode 0x%08x\n", CANin(0, canmode));
}


/* -------------------------------------------------------------------------- */
/* ---------------- Operations related to the SJA1000 chip. ----------------- */
/* -------------------------------------------------------------------------- */


int CANDeviceManager::_start_chip(int minor) {
    DBGin();
    DBGprint(DBG_DATA, ("[%d] CAN_mode 0x%x", minor, CANin(minor, canmode)));
    udelay(10);

    /* clear interrupts, value not used */
    CANin(minor, canirq);

    /* Interrupts on Rx, TX, any Status change and data overrun */
    CANset(minor, canirq_enable, (CAN_OVERRUN_INT_ENABLE +
    							  CAN_ERROR_INT_ENABLE +
                                  CAN_TRANSMIT_INT_ENABLE +
                                  CAN_RECEIVE_INT_ENABLE));

    CANreset(minor, canmode, CAN_RESET_REQUEST);
    DBGprint(DBG_DATA,("start mode=0x%x", CANin(minor, canmode)));

    DBGout();
    return 0;
}


int CANDeviceManager::_stop_chip(int minor) {
    CANset(minor, canmode, CAN_RESET_REQUEST);
    return 0;
}


int CANDeviceManager::_reset_chip(int minor) {
    DBGin();
    DBGprint(DBG_DATA,(" INT 0x%x", CANin(minor, canirq)));

    CANout(minor, canmode, CAN_RESET_REQUEST);

    udelay(10);

    CANin(minor, canstat);

    DBGprint(DBG_DATA, ("status=0x%x mode=0x%x", status,
	    CANin(minor, canmode) ));

    DBGprint(DBG_DATA, ("[%d] CAN_mode 0x%x", minor, CANin(minor, canmode)));

    /* select mode: Basic or PeliCAN */
    CANout(minor, canclk, CAN_MODE_PELICAN + CAN_MODE_CLK);
    CANout(minor, canmode, CAN_RESET_REQUEST + CAN_MODE_DEF);

    /* Board specific output control (Janus MM board) */
    CANout(minor, canoutc, 0xda);

    this->_set_timing(minor, this->_baud[minor]);
    this->_set_mask(minor, this->_acc_code[minor], this->_acc_mask[minor]);
    DBGprint(DBG_DATA, ("[%d] CAN_mode 0x%x", minor, CANin(minor, canmode)));

    DBGout();
    return 0;
}

/* -------------------------------------------------------------------------- */
/* ----------------------------- Miscellaneous ------------------------------ */
/* -------------------------------------------------------------------------- */


CANDeviceManager::~CANDeviceManager() {};


/** Global per channel variables from sja1000funcs.c
 *  For PATH driver, each channel has a separate driver, so MAX_CHANNELS is 1
 *  Most of these are not really used in the PATH CAN driver for QNX6.
 */
atomic_t Can_isopen[MAX_CHANNELS];


can_err_count_t CANDeviceManager::clear_errs() {
	this->_errs.intr_in_handler_count = 0;
	this->_errs.tx_interrupt_count = 0;
	this->_errs.rx_interrupt_count = 0;
	this->_errs.shadow_buffer_count = 0;
	this->_errs.rx_message_lost_count = 0;
	return this->_errs;
}


can_err_count_t CANDeviceManager::get_errs() {
	return this->_errs;
}


int CANDeviceManager::interrupt(CircularBuffer *in_buff,
								CircularBuffer *out_buff,
								can_filter_t filter)
{
	int i=0;
	int retval = 0;	// set to 1 for receive
#ifdef DO_TRACE
	struct timeb current;
	ftime(&current);
	printf("interrupt: sec %d ms %3d\n", current.time, current.millitm);
	fflush(stdout);
#endif
	BYTE status_val;
	BYTE ir_val = CANin(MY_CHANNEL, canirq);
	BYTE rmc = CANin(MY_CHANNEL, reserved3);

	while (ir_val) {
		if (i > 0)
			this->_errs.intr_in_handler_count++;
		++i;

#ifdef DO_TRACE
		status_val = CANin(MY_CHANNEL, canstat);
		printf("interrupt: value 0x%02x, status 0x%02x\n",
			ir_val, status_val);
		fflush(stdout);
#endif

		if (ir_val & CAN_RECEIVE_INT) {
			retval = 1;
			this->_errs.rx_interrupt_count++;
#ifdef DO_TRACE_RX
			printf("RX interrupt\n");
			fflush(stdout);
#endif
   			this->rx_process_interrupt(in_buff, filter);

			CANout(MY_CHANNEL, cancmd, CAN_RELEASE_RECEIVE_BUFFER);
		}

		if (ir_val & CAN_OVERRUN_INT) {
			CANout(minor, cancmd, CAN_CLEAR_OVERRUN_STATUS);
			this->_errs.rx_message_lost_count++;
		}

		if (ir_val & CAN_TRANSMIT_INT) {
#ifdef DO_TRACE_TX
			printf("TX interrupt\n");
			fflush(stdout);
#endif
			this->_errs.tx_interrupt_count++;
			this->tx_process_interrupt(out_buff);
		}

		if (ir_val & CAN_ERROR_INT) {
			if ((status_val & CAN_BUS_STATUS)) {
				printf("Bus off; try to reset SJA1000\n");
				fflush(stdout);
				/* Try resetting the chip. */
				if (this->_reset_chip((MY_CHANNEL) < 0))
					printf("Error on reset\n");
				else
					this->_start_chip(MY_CHANNEL);
				return 0;
			}
			if((status_val & CAN_ERROR_STATUS)!=0) {
				printf("Warning: error status.\n");
				fflush(stdout);
			}
		}

		if (ir_val & CAN_WAKEUP_INT) {
			printf("Unexpected interrupt: %d\n", ir_val);
			fflush(stdout);
		}

		/* Return if have received all messages there at start. Don't want to
		 * stay here to long and starve receiver. */
		if (i > rmc) return retval;

		ir_val = CANin(MY_CHANNEL, canirq);
	}
	return retval;
}


can_msg_t CANDeviceManager::read(CircularBuffer *in_buff) {
	can_msg_t msg;
	can_msg_t *pmsg;

	memset(&msg, 0, sizeof(can_msg_t));

	pmsg = (can_msg_t*) in_buff->pop_first();

#ifdef DO_TRACE
	printf("can_dev_read 0x%x\n", (unsigned int) pmsg);
#endif
	if (pmsg == NULL)
		msg.error = 1;
	else {
		msg = *pmsg;
		msg.error = 0;
	}
#ifdef DO_TRACE
	print_can_msg(&msg);
	fflush(stdout);
#endif
	return msg;
}


void CANDeviceManager::rx_process_interrupt(CircularBuffer *in_buff,
		can_filter_t filter)
{
	can_msg_t msg;
	BYTE frm_info = CANin(MY_CHANNEL, frameinfo);
	int ext = frm_info & CAN_EFF;
   	int i;
	unsigned int ID[4];

#ifdef DO_TRACE_RX
	printf("RX_INTERRUPT: frm_info 0x%08x\n", frm_info);
#endif
	msg.size = frm_info & 0x0f;
	if (ext) {
		for (i=0; i < msg.size && i < 8; i++)
			msg.data[i] = CANin(MY_CHANNEL,
					frame.extframe.canxdata[i]);

		ID[0] = CANin(MY_CHANNEL, frame.extframe.canid1);
		ID[1] = CANin(MY_CHANNEL, frame.extframe.canid2);
		ID[2] = CANin(MY_CHANNEL, frame.extframe.canid3);
		ID[3] = CANin(MY_CHANNEL, frame.extframe.canid4);
#ifdef DO_TRACE_RX
		printf("ID: %08x %08x %08x %08x\n", ID[0], ID[1], ID[2], ID[3]);
#endif
		msg.id = (ID[0] << 21) | (ID[1] << 13) |
				 (ID[2] << 5) | (ID[3] >> 3);
#ifdef DO_TRACE_RX
		printf("msg.id: %08x\n", msg.id);
#endif
		SET_EXTENDED_FRAME(msg);
	} else {
		for (i=0; i < msg.size && i < 8; i++)
			msg.data[i] = CANin(MY_CHANNEL,
					frame.stdframe.candata[i]);
		ID[0] = CANin(MY_CHANNEL, frame.stdframe.canid1) << 3;
		ID[1] = CANin(MY_CHANNEL, frame.extframe.canid2) >> 5;
		msg.id = ID[0] | ID[1];
	}
#ifdef DO_TRACE_RX
	printf("RX_INTERRUPT: msg.id %d, msg.size %d, data[0] 0x%02x\n",
			CAN_ID(msg), msg.size, msg.data[0]);
#endif

	/* Add a new message if ID and MASK allow. */
	if ((filter.id & filter.mask) == (msg.id & filter.mask))
		in_buff->add(&msg);
}


void CANDeviceManager::tx_process_interrupt(CircularBuffer *out_buff) {
	out_buff->pop_first();

	/* check is anymore messages are waiting to be sent */
	if (out_buff->get_count() != 0)
		this->send(out_buff);
}


int CANDeviceManager::write(CircularBuffer *out_buff, can_msg_t *pmsg)
{
	pmsg->error = 0;
#ifdef DO_TRACE
	printf("can_dev_write: buff 0x%x, msg 0x%x\n",
			(unsigned int) pattr->out_buff.data_array,
			(unsigned int) pmsg);
	print_can_msg(pmsg);
	fflush(stdout);
#endif

	/* add the new element to the output buffer */
	out_buff->add(pmsg);

#ifdef DO_TRACE_TX
	printf("write id 0x%x ", (unsigned int) pmsg->id);
	for (int j = 0; j < pmsg->size; j++)
		printf(" %d", pmsg->data[j]);
	printf("\n");
	printf("data count: %d\n", out_buff->get_count());
	fflush(stdout);
#endif

	if (out_buff->get_count() == 1) {
		this->send(out_buff);
	}
	else {
		/* Assumes message sends are at least 1 Hz */
		if ((time((time_t *)NULL) - this->_last_time_can_sent) > 1) {
			this->can_timeout_count++;
			this->send(out_buff);
		}

		if (out_buff->get_count() > 10) {
			/* must have missed an interrupt, throw old stuff out */
			this->tx_buffer_flush += out_buff->empty();
		}
	}

	return EOK;
}


void CANDeviceManager::send(CircularBuffer *out_buff) {
	int i;
	DBGin();

	/* Grab the most recent element. */
	can_msg_t *tx = (can_msg_t*) out_buff->read_first();

	/* Info and identifier fields */
	BYTE tx2reg = tx->size;

	DBGprint(DBG_DATA, ("CAN[%d]: tx.flags=%d tx.id=0x%lx tx.length=%d stat=0x%x",
			minor, tx->flags, tx->id, tx->length, stat));

	if (IS_EXTENDED_FRAME(*tx)) {
		CANout(MY_CHANNEL, frameinfo, CAN_EFF + tx2reg);
		CANout(MY_CHANNEL, frame.extframe.canid1, (BYTE)(tx->id >> 21));
		CANout(MY_CHANNEL, frame.extframe.canid2, (BYTE)(tx->id >> 13));
		CANout(MY_CHANNEL, frame.extframe.canid3, (BYTE)(tx->id >> 5));
		CANout(MY_CHANNEL, frame.extframe.canid4, (BYTE)(tx->id << 3) & 0xff);
		for(i=0; i<tx->size; i++)
			CANout(MY_CHANNEL, frame.extframe.canxdata[i], tx->data[i]);
//			CANout(MY_CHANNEL, frame.extframe.canxdata[R_OFF * i], tx->data[i]);
	} else {
		CANout(MY_CHANNEL, frameinfo, CAN_SFF + tx2reg);
		CANout(MY_CHANNEL, frame.stdframe.canid1, (BYTE)((tx->id) >> 3) );
		CANout(MY_CHANNEL, frame.stdframe.canid2, (BYTE)(tx->id << 5 ) & 0xe0);
		for (i=0; i<tx->size; i++)
			CANout(MY_CHANNEL, frame.stdframe.candata[i], tx->data[i]);
//			CANout(MY_CHANNEL, frame.stdframe.candata[R_OFF * i], tx->data[i]); FIXME
	}

	CANout(MY_CHANNEL, cancmd, CAN_TRANSMISSION_REQUEST);
	this->_last_time_can_sent = time((time_t *)NULL);

	DBGout();
}


int CANDeviceManager::_set_timing(int minor, int baud) {
	int i = 5;
	int custom = 0;

    DBGin();

    int isopen = atomic_read(&Can_isopen[minor]);
    if ((isopen > 1) && (this->_baud[minor] != baud)) {
		DBGprint(DBG_DATA, ("isopen = %d", isopen));
		DBGprint(DBG_DATA, ("refused baud[%d]=%d already set to %d",
						minor, baud, Baud[minor]));
		return -1;
    }

    DBGprint(DBG_DATA, ("baud[%d]=%d", minor, baud));
    switch (baud) {
		case   10: i = 0; break;
		case   20: i = 1; break;
		case   50: i = 2; break;
		case  100: i = 3; break;
		case  125: i = 4; break;
		case  250: i = 5; break;
		case  500: i = 6; break;
		case  800: i = 7; break;
		case 1000: i = 8; break;
		default  : custom=1;
    }

    /* select mode: Basic or PeliCAN */
    CANout(minor, canclk, CAN_MODE_PELICAN + CAN_MODE_CLK);
    if (custom) {
    	CANout(minor, cantim0, (BYTE) (baud >> 8) & 0xff);
    	CANout(minor, cantim1, (BYTE) (baud & 0xff ));
    } else {
    	CANout(minor, cantim0, (BYTE) CanTiming[i][0]);
    	CANout(minor, cantim1, (BYTE) CanTiming[i][1]);
    }

    DBGprint(DBG_DATA, ("tim0=0x%x tim1=0x%x",
    		CANin(minor, cantim0), CANin(minor, cantim1)));

    DBGout();
    return 0;
}


int CANDeviceManager::_set_mask(int minor, unsigned int code, unsigned int mask)
{
    DBGin();
    DBGprint(DBG_DATA,("[%d] acc=0x%x mask=0x%x", minor, code, mask));
    CANout(minor, frameinfo,
    		(BYTE)((unsigned int)(code >> 24) & 0xff));
    CANout(minor, frame.extframe.canid1,
    		(BYTE)((unsigned int)(code>>16) & 0xff));
    CANout(minor, frame.extframe.canid2,
    		(BYTE)((unsigned int)(code>>8) & 0xff));
    CANout(minor, frame.extframe.canid3,
    		(BYTE)((unsigned int)(code>>0) & 0xff));
    CANout(minor, frame.extframe.canid4,
    		(BYTE)((unsigned int)(mask>>24) & 0xff));
    CANout(minor, frame.extframe.canxdata[0 * R_OFF],
    		(BYTE)((unsigned int)(mask >> 16) & 0xff));
    CANout(minor, frame.extframe.canxdata[1 * R_OFF],
			(BYTE)((unsigned int)(mask >>8) & 0xff));
    CANout(minor, frame.extframe.canxdata[2 * R_OFF],
			(BYTE)((unsigned int)(mask >> 0) & 0xff));

    DBGout();
    return 0;
}


int CANDeviceManager::_set_omode(int minor, int arg) {
    DBGin();
	DBGprint(DBG_DATA,("[%d] outc=0x%x", minor, arg));
	CANout(minor, canoutc, arg);
    DBGout();

    return 0;
}


int CANDeviceManager::_set_listen_only_mode(int minor, int arg) {
	if (arg)
		CANset(minor, canmode, CAN_LISTEN_ONLY_MODE);
    else
    	CANreset(minor, canmode, CAN_LISTEN_ONLY_MODE);

	return 0;
}

