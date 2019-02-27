/**\file
 *
 * can_dev.cpp
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 22, 2019
 */

#include "can_dev.h"
#include "utils/sys.h"
#include <sys/mman.h>


/* Definition the Registers of 82527 */
/*-----------------------------------*/
#define CONTROL_REG      0x00  /* Address Kontroll-Reg.                */
#define STATUS_REG       0x01  /* Address Status-Reg.                  */
#define CPU_IF_REG       0x02  /* Address CPU-Interface-Register       */
#define HIGH_SPEED_READ	 0x04  /* Has result of last read, for fast CPU */
#define G_MASK_S_REG0    0x06  /* Address Global Mask Standard Reg. 0  */
#define G_MASK_S_REG1    0x07  /* Address Global Mask Standard Reg. 1  */
#define G_MASK_E_REG0    0x08  /* Address Global Mask Extended Reg. 0  */
#define G_MASK_E_REG1    0x09  /* Address Global Mask Extended Reg. 1  */
#define G_MASK_E_REG2    0x0A  /* Address Global Mask Extended Reg. 2  */
#define G_MASK_E_REG3    0x0B  /* Address Global Mask Extended Reg. 3  */
#define M_MASK_REG0      0x0C  /* Address Message Mask Reg. 0          */
#define M_MASK_REG1      0x0D  /* Address Message Mask Reg. 1          */
#define M_MASK_REG2      0x0E  /* Address Message Mask Reg. 2          */
#define M_MASK_REG3      0x0F  /* Address Message Mask Reg. 3          */
#define CLKOUT_REG  	 0x1F  /* Clock Out Reg..      */
#define BUS_CON_REG      0x2F  /* Address Bus Konfigurations Reg.      */
#define BIT_TIMING_REG0  0x3F  /* Address Bit Timing Reg. 0            */
#define BIT_TIMING_REG1  0x4F  /* Address Bit Timing Reg. 1            */
#define INT_REG          0x5F  /* Address Interrupt Reg.               */
#define P1CONF_REG       0x9F  /* Address Port1 Konfig. Reg.           */
#define PCONF_REG       0x9F  /* Address Port1 Konfig. Reg.           */
#define PIN		       0xBF  /* Address Port1 Konfig. Reg.           */
#define POUT		       0xDF  /* Address Port1 Konfig. Reg.           */
#define MSG_1            0x10  /* Address Message 1;   0x10-0x1E       */
#define MSG_2            0x20  /* Address Message 2;   0x20-0x2E       */
#define MSG_3            0x30  /* Address Message 3;   0x30-0x3E       */
#define MSG_4            0x40  /* Address Message 4;   0x40-0x4E       */
#define MSG_5            0x50  /* Address Message 5;   0x50-0x5E       */
#define MSG_6            0x60  /* Address Message 6;   0x60-0x6E       */
#define MSG_7            0x70  /* Address Message 7;   0x70-0x7E       */
#define MSG_8            0x80  /* Address Message 8;   0x80-0x8E       */
#define MSG_9            0x90  /* Address Message 9;   0x90-0x9E       */
#define MSG_10           0xA0  /* Address Message 10;  0xA0-0xAE       */
#define MSG_11           0xB0  /* Address Message 11;  0xB0-0xBE       */
#define MSG_12           0xC0  /* Address Message 12;  0xC0-0xCE       */
#define MSG_13           0xD0  /* Address Message 13;  0xD0-0xDE       */
#define MSG_14           0xE0  /* Address Message 14;  0xE0-0xEE       */
#define MSG_15           0xF0  /* Address Message 15;  0xF0-0xFE       */

#define CTRL_0_REG       0x00
#define CTRL_1_REG       0x01
#define DATA0_REG 	0x07
#define CONF_REG 	0x06


#define _82527_XTD_FRAME 4
#define _DIR_TX 8




/*\file
 *
 * Device level code, based on sample code provided by SSV, originally
 * ported to QNX4 by Jorge M. Estrela da Silva, 2002  ISEP, Porto, Portugal.
 *
 * Filtering into different message registers is not implemented.
 * All transmit is done from MSG1, all received in MSG15.
 *
 */

#include "can.h"

#undef  DO_TRACE

unsigned int shadow_buffer_count = 0;
unsigned int intr_in_handler_count = 0;
unsigned int rx_interrupt_count = 0;
unsigned int rx_message_lost_count = 0;
unsigned int tx_interrupt_count = 0;

/**
 *      Clear the error counts and return the old counts.
 */
can_err_count_t can_dev_clear_errs()
{
        can_err_count_t errs;
        errs.intr_in_handler_count = intr_in_handler_count;
        errs.tx_interrupt_count = tx_interrupt_count;
        errs.rx_interrupt_count = rx_interrupt_count;
        errs.shadow_buffer_count = shadow_buffer_count;
        errs.rx_message_lost_count = rx_message_lost_count;
        intr_in_handler_count = 0;
        tx_interrupt_count = 0;
        rx_interrupt_count = 0;
        shadow_buffer_count = 0;
        rx_message_lost_count = 0;
        return (errs);
}

/**
 *      Return the current error count .
 */
can_err_count_t can_dev_get_errs()
{
        can_err_count_t errs;
        errs.intr_in_handler_count = intr_in_handler_count;
        errs.tx_interrupt_count = tx_interrupt_count;
        errs.rx_interrupt_count = rx_interrupt_count;
        errs.shadow_buffer_count = shadow_buffer_count;
        errs.rx_message_lost_count = rx_message_lost_count;
        return (errs);
}

/** Redefine I/O port macros in terms of QNX6 versions
 */
#define inportb(ARG) in8(ARG)
#define inport(ARG) in8(ARG)
#define outportb(ARG1,ARG2) out8(ARG1,ARG2)
#define outport(ARG1,ARG2) out8(ARG1,ARG2)

typedef unsigned char BYTE;
typedef unsigned int WORD;


/** Global Variables for device-level code
 */

WORD BaseAddress;    // Base-Address CAN
WORD Speed;          // Speed, to get bit rate
BYTE ID[4];	     // CAN device ID
BYTE STATUS;         // STATUS, current value of reg.
///static BYTE Interrupt_REG;   /* Interrupt_REG, current value of reg. */
BYTE INTERRUPT_VALUE;      /* Interrupt_VALUE, STATUS, current  */
BYTE DEBUG_BYTE;		/* used to read from registers during debug */
#undef DEBUG

time_t last_time_can_sent; 	//last time a message was sent
BYTE set_extended_frame;	//sets listening to extended or standard messages


/** Device level function prototype
 */
void Interrupt_Request_Rx(IOFUNC_ATTR_T *pattr);

/***************************************************************************/
/* function   : Interrupt_Request_Tx                                      */
/***************************************************************************/
void Interrupt_Request_Tx(IOFUNC_ATTR_T *pattr)
{
	can_cq_pop_first(&pattr->out_buff);

	if (pattr->out_buff.data_count != 0)
		can_send(pattr);
	else
		;//printf("no more messages waiting to be sent\n");
}
/* Interrupt_Request_Tx END */
/***************************************************************************/


void Set_Bit_Speed(WORD BS);
void Write_REG(BYTE Addr,BYTE value);
BYTE Read_REG(BYTE Addr);

#ifdef SSV_CAN
/*------------------------------------------------------------------------*/
/***************************************************************************/
/* function  : Write_REG, writes a value to address                        */
/* Par. var. : - Addr, address of reg. which must be written               */
/*             - Value, value which must be written into a reg.            */
/***************************************************************************/
void Write_REG(BYTE Addr,BYTE value)
{
   outportb(BaseAddress,Addr);     /* select CAN-Control-Reg. */
   outportb(BaseAddress+1,value);  /* write Reg.              */
}
/* Write_REG END */
/***************************************************************************/

/***************************************************************************/
/* function  : Read_REG, writes a value to address                         */
/* Par. var. : Addr, address of reg. which must be read                    */
/* return    : value which was in reg.                                     */
/***************************************************************************/
BYTE Read_REG(BYTE Addr)
{
   outportb(BaseAddress,Addr);
   return(inport(BaseAddress+1));
}
/* Read_REG END */
/***************************************************************************/

#else
#define I82527_MAP_SIZE 256
/** Start of read/write register code for ECAN527, with non-multiplexed 8 bit bus
 */
void Write_REG(BYTE Addr,BYTE value)
{
	volatile unsigned char *preg = (volatile unsigned char *)
			 (BaseAddress + Addr);
	*preg = value;
}

BYTE Read_REG(BYTE Addr)
{
	volatile unsigned char *preg = (volatile unsigned char *)
			 (BaseAddress + Addr);
	return *preg;
//	return(* (volatile unsigned char *) (BaseAddress + HIGH_SPEED_READ));
}
/** End of ECAN527 section
 */
#endif //SSV_CAN

/***************************************************************************/
/* function  : Init_CAN, initializes the CAN                               */
/* (renamed can_dev_init, given parameters)				   				   */
/* Par. var. : ch, to save and compare current value of reg.               */
/***************************************************************************/
void can_dev_init(unsigned int base_address, unsigned int bit_speed,
		unsigned char extended_frame)
{
   BYTE ch;
#ifdef SSV_CAN
   printf("Using SSV_CAN memory access\n");
   BaseAddress = base_address;
#else
   printf("Using ECAN527 memory access\n");
   BaseAddress = (uint64_t) mmap_device_memory(NULL,  // TODO: was unsigned int
        I82527_MAP_SIZE,
        PROT_READ | PROT_WRITE | PROT_NOCACHE,
        0, base_address);
#endif

   Speed = bit_speed;
   set_extended_frame = extended_frame;

   //ThreadCtl(_NTO_TCTL_IO, NULL); /// this has already been done in can_man
#ifdef DO_TRACE
	printf("BaseAddress 0x%02x, Speed %d, %s\n",
		BaseAddress, Speed,
		set_extended_frame?"extended frame":"standard frame");
	fflush(stdout);
#endif

   /* wait until Reset */
	while ((ch & 0x80) != 0x00) {
      ch = Read_REG(CPU_IF_REG);
         /* 0x1XXX XXXX-- CEn
              |||| |||--- 0
              |||| ||---- MUX
              |||| |----- Sleep
              ||||------- PwD
              |||-------- DMC
              ||--------- DSC
              |---------- RstST = 0, normal operation */
   }

#ifdef DO_TRACE
   STATUS=Read_REG(STATUS_REG);
   printf("can_dev_init: STATUS 0x%02x\n", STATUS);
#endif

   Write_REG(CONTROL_REG,0x41);
   /* 0x0100 0001 - Init (Software-) Enabled
             |||--- IE Disabled
             ||---- SIE Disabled, no interrupt will be generated
             |      if a bus error
             |----- EIE Disabled, no interrupt will be generated
                    if a change in error status */

	//sleep(1);//for Busoff

#ifdef SSV_CAN
   Write_REG(CPU_IF_REG,0x60);
   /* 0x0110 0000 - CEn disabled, Clock out signal
        |||| |||--- Reserved
        |||| ||---- MUX, normal operation
        |||| |----- Sleep ==>
        ||||------- PwD   ==> both power down & sleep mode not active
        |||-------- DMC, MCLK is equal to SCLK/2
        ||--------- DSC, SCLK is equal to XTAL/2
        |---------- normal operation */
#else
   Write_REG(CPU_IF_REG, 0);
#endif

#ifdef SSV_CAN
   Write_REG(BUS_CON_REG,0x40);
#else
   Write_REG(CLKOUT_REG, 0x0);
   Write_REG(BUS_CON_REG,0x0);
#endif
   /* 0x0100 0000 - DcR0, disconnect RX0 input Enabled
        |||| |||--- DcR1, disconnect RX1 input Enabled
        |||| ||---- A zero must always be written to this bit
        |||| |----- DcT1, disconnect TX1 output driver Enabled
        ||||------- A zero must always be written to this bit
        |||-------- POL
        ||--------- CoBy, normal operation, RX0-1 are the inputs to the
        |           input comparator
        |---------- A zero must always be written to this bit */


   Write_REG(STATUS_REG,0x00);
   /* 0x0000 0000 -  ==>
        |||| |||--- LEC 0-2, No Error
        |||| |----- TXOK, No messege succesfully transmitted
        ||||------- RXOK, No message succesfully received
        |||-------- Wake, Wake Up interrupt is reset by reading status Reg.
        ||--------- Warn, there is no abnormal accurrence of errors
        |---------- BOff, 82527 is not bus off */


   Set_Bit_Speed(Speed);

   /* Define alll MSG as invalid */
   Write_REG(MSG_1+CTRL_0_REG,0x55);
   Write_REG(MSG_2+CTRL_0_REG,0x55);
   Write_REG(MSG_3+CTRL_0_REG,0x55);
   Write_REG(MSG_4+CTRL_0_REG,0x55);
   Write_REG(MSG_5+CTRL_0_REG,0x55);
   Write_REG(MSG_6+CTRL_0_REG,0x55);
   Write_REG(MSG_7+CTRL_0_REG,0x55);
   Write_REG(MSG_8+CTRL_0_REG,0x55);
   Write_REG(MSG_9+CTRL_0_REG,0x55);
   Write_REG(MSG_10+CTRL_0_REG,0x55);
   Write_REG(MSG_11+CTRL_0_REG,0x55);
   Write_REG(MSG_12+CTRL_0_REG,0x55);
   Write_REG(MSG_13+CTRL_0_REG,0x55);
   Write_REG(MSG_14+CTRL_0_REG,0x55);
   Write_REG(MSG_15+CTRL_0_REG,0x55);


   /* don't apply global accepting filters*/
   Write_REG(G_MASK_S_REG0,0xFF);
   Write_REG(G_MASK_S_REG1,0xFF);
   Write_REG(G_MASK_E_REG0,0xFF);
   Write_REG(G_MASK_E_REG1,0xFF);
   Write_REG(G_MASK_E_REG2,0xFF);
   Write_REG(G_MASK_E_REG3,0xFF);
   /* Receive ALL in Object 15 */
   Write_REG(M_MASK_REG0,0x00);
   Write_REG(M_MASK_REG1,0x00);
   Write_REG(M_MASK_REG2,0x00);
   Write_REG(M_MASK_REG3,0x00);


   /* Reveive conf. */
   Write_REG(MSG_15+CTRL_0_REG,0x99);
   /* 0x1001 1001 - ==>
        |||| |||--- ==> IntPnd, MSB=0 no interrupt was generated by message
        |||| ||---- ==>
        |||| |----- ==> RXIE, MSB=1 an interrupt will be generated by reception
        ||||------- ==>
        |||-------- ==> TXIE, MSB=0 no interrupt will be generated by transmit
        ||--------- ==>
        |---------- ==> MsgVal, MSB=1 message valid */


   Write_REG(MSG_15+CTRL_1_REG,0x55);
   /* 0x0101 0101 - ==>
        |||| |||--- ==> NewDat, MSB=0 no new Data
        |||| ||---- ==>
        |||| |----- ==> MsgLst, MSB=0 no message was lost
        ||||------- ==>
        |||-------- ==> TxRqst, MSB=0 no waiting for transmit
        ||--------- ==>
        |---------- ==> RmtPnd, MSB=0 no waiting for remote request */


	if(set_extended_frame)
   		Write_REG(MSG_15+0x06,0x84);
	      /* 0x1000 0100 - 0x84
        	   |||| |||--- ==> Reserved
        	   |||| ||---- ==> 1- extended identifier 29 / 0 - standard identifier 11-bit
        	   |||| |----- ==> Dir, 0 direction receive
        	   ||||------- ==>
        	   |||-------- ==>
        	   ||--------- ==>
        	   |---------- ==> DLC, 8 Data Length Code */
	else
		Write_REG(MSG_15+0x06,0x80);

   	Write_REG(MSG_1+0x06,0x88);
      /* 0x1000 1000 - 0x88
	   |||| |||--- ==> Reserved
	   |||| ||---- ==> standard identifier 11-bit
           |||| |----- ==> Dir, 0 direction receive
           ||||------- ==>
           |||-------- ==>
           ||--------- ==>
           |---------- ==> DLC, 8 Data Length Code */
#undef DEBUG_SIE
#ifdef DEBUG_SIE
	Write_REG(CONTROL_REG,0x4E); /* EIE, SIE,IE Enabled */
#else
	Write_REG(CONTROL_REG,0x4A); /* EIE enabled, IE Enabled */
#endif
    /* 0x0100 1010 - Init (Software-) disabled
              |||--- IE enabled
              ||---- SIE enabled, an interrupt will be generated
              |      if a change in error status
              |----- EIE enabled, an interrupt will be generated
                     if  a bus error*/
}
/* Init_CAN END */
/***************************************************************************/


/***************************************************************************/
/* function  : Set_Bit_Speed, to set the Bit Rate for the transmission     */
/* Par. var. : - BS, bit rate, dufault value = 20 kBuad                    */
/*             - BTR -0,-1, to write the bit timing reg.                   */
/***************************************************************************/
void Set_Bit_Speed(WORD BS)
{
   BYTE BTR0,BTR1;
   switch (BS)
   {
      case 1000: BTR0=0x00; BTR1=0x14; break;
      case  800: BTR0=0x00; BTR1=0x16; break;
      case  500: BTR0=0x00; BTR1=0x1C; break;
// case  250: BTR0=0x01; BTR1=0x1C; break;//original
      case  250: BTR0=0x41; BTR1=0xD8; break;
     case  125: BTR0=0xc3; BTR1=0xc9; break;//alterei o valor BTR1 de 1c para 0xC9, BTR0 de 0x03 para 0xC3
//      case  125: BTR0=0x83; BTR1=0xd8; break;
      case  100: BTR0=0x04; BTR1=0x1C; break;
      case   50: BTR0=0x09; BTR1=0x1C; break;
      case   25: BTR0=0x13; BTR1=0x1C; break;
      case   20: BTR0=0x18; BTR1=0x1C; break;
      case   10: BTR0=0x31; BTR1=0x1C; break;
      default  : BTR0=0x18; BTR1=0x1C; Speed=20;
   }

   /* write the bit timing reg. */
   Write_REG(BIT_TIMING_REG0, BTR0);
   Write_REG(BIT_TIMING_REG1, BTR1);
}
/* Set_Bit_Speed End */
/***************************************************************************/


/***************************************************************************/
/* function   : Interrupt_Request, ISR                                      */
/* (renamed can_dev_interrupt, called by can_handle_interrupt) 		    */
/***************************************************************************/
int can_dev_interrupt(IOFUNC_ATTR_T *pattr)
{
	int i=0;
	int s;
	int retval = 0;	// set to 1 for receive
#ifdef DO_TRACE
	struct timeb current;
	ftime(&current);
	printf("interrupt: sec %d ms %3d\n", current.time, current.millitm);
	fflush(stdout);
#endif
	while((INTERRUPT_VALUE=Read_REG(INT_REG)))
	{

	if(i>0) {
		intr_in_handler_count++;
#ifdef DO_TRACE
		printf("i %d:%s intval=%d\n",i,pattr->devname,INTERRUPT_VALUE);
#endif
	}
	++i;

	STATUS=Read_REG(STATUS_REG);
#ifdef DO_TRACE
	printf("interrupt: value 0x%02x, status 0x%02x\n",
			INTERRUPT_VALUE, STATUS);
	fflush(stdout);
#endif
	switch(INTERRUPT_VALUE)
	{
	case 0x01:
		if((STATUS & 0x80)!=0)
		{
			printf("Bus off: error counter=256\n");
			fflush(stdout);
			//no sei se ir funcionar
			can_dev_init(BaseAddress,Speed,set_extended_frame);
			return 0; //ou exit() se o passo anterior no funcionar
		}
		if((STATUS & 0x40)!=0)
		{
			if(BaseAddress == 1074790400)
				fprintf(stderr,"Warning: error counter>95, broken can1 cable\n");
			if(BaseAddress == 1074790656)
				fprintf(stderr,"Warning: error counter>95, broken can2 cable\n");
			fflush(stdout);
			STATUS = STATUS & ~0x40;	//Limpa Warning
			Write_REG(STATUS_REG,STATUS);
		}

		break;
	case 0x02:
   		s=0;
   		while (Read_REG(MSG_15+CTRL_1_REG) & 0x02)
   		{
   			if(s>0) {
				shadow_buffer_count++;
#ifdef DO_TRACE
   				printf("shadow buffer: %d\n",s);
				fflush(stdout);
#endif
			}
   			++s;
   			retval = 1;
			rx_interrupt_count++;
#ifdef DO_TRACE
			printf("RX interrupt\n");
			fflush(stdout);
#endif
   			Interrupt_Request_Rx(pattr);
			Write_REG(MSG_15+CTRL_0_REG,0xFD);  /* Reset IntPnd */
			Write_REG(MSG_15+CTRL_1_REG,0xFD);  /* 11111101 Reset NewDat*/
		}

		//este teste consome algum tempo que pode ser precioso.
		//Pode indicar mensagens perdidas que realmente no foram perdidas
		if (Read_REG(MSG_15+CTRL_1_REG) & 0x08) /* MsgLst */
		{
			Write_REG(MSG_15+CTRL_1_REG,0xF7); /* Reset MsgLst */
			rx_message_lost_count++;
#ifdef DO_TRACE_RX
			printf("message lost\n");
			fflush(stdout);
#endif
		}

		break;
	case 0x03:
   		Write_REG(MSG_1+CTRL_0_REG,0xFD);  /* Reset IntPnd */
#ifdef DO_TRACE_TX
		printf("TX interrupt\n");
		fflush(stdout);
#endif
		tx_interrupt_count++;
		Interrupt_Request_Tx(pattr);
		break;
	default:
		printf("Unexpected interrupt: %d\n",INTERRUPT_VALUE);
		fflush(stdout);
		// Should add code to clear interrupt
		if (i > 20) return (retval);
		break;
	}

	}
    return (retval);
}
/***************************************************************************/

/***************************************************************************/
/* fuction   : Interrupt_Request_Rx											*/
/* Par., var.: i, counter                                                  */
/***************************************************************************/
void Interrupt_Request_Rx(IOFUNC_ATTR_T *pattr)
{
	//printf("receiving\n");

	can_msg_t msg;
	BYTE conf;

   	int i;
   	for (i=7;i<15;i++)
      		msg.data[i-7]=Read_REG(MSG_15+i);

   	ID[3] = Read_REG(MSG_15+0x02);
	ID[2] = Read_REG(MSG_15+0x03);
   	ID[1] = Read_REG(MSG_15+0x04);
   	ID[0] = Read_REG(MSG_15+0x05);
   	msg.id = *((unsigned long *) ID);
#ifdef DO_TRACE_RX
	for (i = 0; i < 4; i++)
		printf("ID[%d] 0x%02x; ", i, ID[i]);
	printf("msg.id 0x%08x\n", msg.id);
#endif

	conf = Read_REG(MSG_15+CONF_REG);
	if(conf & _82527_XTD_FRAME)
		msg.id = (msg.id>>3) | 0x80000000;
	else
		msg.id = msg.id>>21;

	msg.size = conf>>4 & 0x0F;
#ifdef DO_TRACE_RX
	printf("Rx: newMsg\n");
	fflush(stdout);
#endif
	can_new_msg(&msg, pattr);
}
/* Interrupt_Request_Rx END */
/***************************************************************************/


/***************************************************************************/
/* function  : Send_Message, sends a message to the bus                    */
/*             - M_ADDR, address of the data reg. of Tx-Buffer             */
/*             - i, counter                                                */
/***************************************************************************/
void can_send(IOFUNC_ATTR_T *pattr)
{
	int i;
	can_msg_t *msg = can_cq_read_first(&pattr->out_buff);

#ifdef DO_TRACE_TX
	printf("sending\n");
	fflush(stdout);
#endif

	Write_REG(MSG_1+CTRL_0_REG,0x7D); //01111101 msgval=0, txie=x, rxie=x, intpnd=0
	Write_REG(MSG_1+CTRL_1_REG,0x59); //01011001 RmtPnd =0, Txrqst=0, CpuUpd=1,NewDat=0


	/* Arbitration, Message-ID */
	if(IS_EXTENDED_FRAME(*msg))
	{
		*((unsigned long *) ID) = msg->id<<3;
	}
	else
		*((unsigned long *) ID) = msg->id<<21;

    	Write_REG(MSG_1+0x02,ID[3]);
    	Write_REG(MSG_1+0x03,ID[2]);
    	Write_REG(MSG_1+0x04,ID[1]);
    	Write_REG(MSG_1+0x05,ID[0]);

	for (i=0;i<msg->size;i++)
	{
		Write_REG(MSG_1+DATA0_REG+i,msg->data[i]);
	}

    	//bits 7,6, 5, 4 - length; bit 3 - direction; bit 2 - extended frame
    	Write_REG(MSG_1+CONF_REG,
    		(msg->size<<4) | _DIR_TX | (IS_EXTENDED_FRAME(*msg) ? _82527_XTD_FRAME : 0) );

	/*-- Transmit --*/

	Write_REG(MSG_1+CTRL_0_REG,0xA5); //10100101 msgval=1, txie=1, rxie=0, intpnd=0 (just to be sure)
	Write_REG(MSG_1+CTRL_1_REG,0x66); //01100110 RmtPnd =0, Txrqst=1,CpuUpd=0,NewDat=1
	/*----------------------------*/

#ifdef DO_TRACE_TX
	{
		unsigned char ctrl0, ctrl1;
		ctrl0 = Read_REG(MSG_1+CTRL_0_REG);
		ctrl1 = Read_REG(MSG_1+CTRL_1_REG);
		printf("msg object 1 ctrl0 0x%02x, ctrl1 0x%02x\n", ctrl0, ctrl1);
		fflush(stdout);
	}
#endif

	last_time_can_sent = time((time_t *)NULL);
}






/*
 *
 * Digital Ports (port 0 and port 1)
 *
 */

static BYTE old_bits[]= {0, 0};

int digital_dir(int port, long bits)
{
	Write_REG(PCONF_REG+port*16, (BYTE)bits);
	return EOK;
}

int digital_in(int port, long *pbits)
{
	*pbits = (long) Read_REG(PIN+port*16);
	return EOK;
}

int digital_out(int port, long mask, long bits,
				 long *pold_bits, long *pnew_bits )
{
	long new_bits;

	new_bits = ( old_bits[port] & ~mask ) | ( bits & mask );

	*pold_bits = old_bits[port];
	*pnew_bits = new_bits;
	Write_REG(POUT+port*16,(BYTE) new_bits);

	//printf("wrote %d on port %d\n",new_bits,port+1);
	return EOK;
}
