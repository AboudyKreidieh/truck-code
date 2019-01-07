/**\file
 *
 * jbus_send.h
 *
 * This file contains all methods needed to send commands to the vehicle's
 * engine, engine retarder, transmission retarder, and brake via the jbus.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date July 11, 2018
 */

#ifndef SRC_JBUS_JBUS_SEND_H_
#define SRC_JBUS_JBUS_SEND_H_

#include "j1939_struct.h"


/** repetition interval should be 10 milliseconds for engine, 50 milliseconds
 * for retarder, 40 milliseconds for EBS */
#define JBUS_INTERVAL_MSECS	5


/** Safety check information
 *
 * holds info read from the database, to be used for safety checks -- in
 * particular, to ensure that the transmission retarder is not turned on during
 * acceleration.
 */
typedef struct {
	float percent_engine_torque;
	float accelerator_pedal_position;
} info_check_type;


/** Command content object
 *
 * jbus_cmd_type will hold the data for the most recently sent command
 * and be acted on by the functions in send_jbus_type.
 */
typedef struct {
	int dbv;					// database variable number of the command type
	j1939_tsc1_typ tsc1;		// current TSC1 state
	j1939_exac_typ exac;		// current EXAC state
	timeb last_time;			// last time a command was sent
	int interval;				// broadcast interval when active
	int heartbeat;				// heartbeat interval when inactive (0 if none)
	int override_limit;			// time limit for same active command
	info_check_type *pinfo;		// pointer to safety check info
} jbus_cmd_type;


/** blank
 *
 * send_jbus_type will be initialized differently for each type of command that
 * can be sent.
 */
typedef struct {
	int active;			/* if 0, send no messages of this type */
	int total_sent;		/* may wrap for long runs, for debugging */
	int slot_number;	/* needed for STB, maybe later with SSV CAN */
	int fd;				/* file descriptor of device file */
	jbus_cmd_type cmd;	/* last command of this type sent */
	int (*is_ready_to_send)(long_output_typ *ctrl, jbus_cmd_type *cmd);
	void (*update_command)(long_output_typ *ctrl, jbus_cmd_type *cmd,
			int dosend);
	void (*cmd_to_pdu)(struct j1939_pdu *pdu, void *jcmd);
} send_jbus_type;


/** blank */
#define JBUS_SEND_ENGINE			0
#define JBUS_SEND_ENGINE_RETARDER	1
#define	JBUS_SEND_EBS				2
#define	JBUS_SEND_TRANS_RETARDER	3
#define NUM_JBUS_SEND				4


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


/** blank
 *
 * blank.
 */
class JBusSender
{
public:
	jbus_cmd_type *cmd;		/* last command of this type sent */
	long_output_typ *ctrl;	/* blank */
	bool active;			/* if false, send no messages of this type */
	int total_sent;			/* may wrap for long runs, for debugging */
	int slot_number;		/* needed for STB, maybe later with SSV CAN */
	int fd;					/* file descriptor of device file */

	/* Main method during the execution loop. Calls all other methods in this
	 * class. */
	void update_cmd(long_output_typ*, jbus_cmd_type*, bool);

	/* Checks if the updated command is ready to send. */
	virtual bool ready_to_send();

	/* Converts the content of the command to its pdu format. */
	virtual j1939_pdu_typ *cmd_to_pdu();

	/* Sends the command to the jbus. */
	virtual void send_cmd(j1939_pdu_typ*);

	/* Publishes the command for use by other processes. */
	virtual void publish();
};


void JBusSender::update_cmd(long_output_typ *ctrl,
		jbus_cmd_type *cmd,
		bool dosend) {
	// update the command
	JBusSender::cmd = cmd;
	JBusSender::ctrl = ctrl;

	if (dosend && active) {
		// check if the message is ready to send
		if (JBusSender::ready_to_send()) {
			// convert the message to its pdu format
			j1939_pdu_typ *pdu = JBusSender::cmd_to_pdu();

			// send the message
			JBusSender::send_cmd(pdu);											// TODO(ak); check for success, use in total_sent
			total_sent += 1;
		}
	}

	// publish the command to the pub/sub server
	JBusSender::publish();
}


/** blank */
class EngineSender : JBusSender
{
	virtual j1939_pdu_typ *cmd_to_pdu();
	virtual bool ready_to_send();
};


/** blank */
class EngineRetarderSender : JBusSender
{
	virtual j1939_pdu_typ *cmd_to_pdu();
	virtual bool ready_to_send();
};


/** blank */
class EBSSender : JBusSender
{
	virtual j1939_pdu_typ *cmd_to_pdu();
	virtual bool ready_to_send();
};


/** blank */
class TransRetarderSender : JBusSender
{
	virtual j1939_pdu_typ *cmd_to_pdu();
	virtual bool ready_to_send();
};


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


/* update_command functions */
extern void update_engine_tsc1(long_output_typ *ctrl, jbus_cmd_type *cmd,
		int dosend);
extern void update_engine_retarder_tsc1(long_output_typ *ctrl,
		jbus_cmd_type *cmd, int dosend);
extern void update_brake_exac(long_output_typ *ctrl, jbus_cmd_type *cmd,
		int dosend);
extern void update_trans_retarder_tsc1(long_output_typ *ctrl,
		jbus_cmd_type *cmd, int dosend);

/* overall initialization and exit functions */
extern int send_jbus_init(jbus_func_t *pjbf,
		send_jbus_type *msg, info_check_type *pinfo,
		int active_mask, char *engine_file,
		char *brake_file, char *trans_file);
extern void send_jbus_exit(jbus_func_t *pjbf, send_jbus_type *msg);


#endif /* SRC_JBUS_JBUS_SEND_H_ */
