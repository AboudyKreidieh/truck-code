/**\file
 *
 * jbus_send.cpp
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date July 15, 2018
 */

#include "jbus_send.h"
#include "j1939_struct.h"


j1939_pdu_typ *EngineSender::cmd_to_pdu() {
	// collect the TSC1 command from the control message
	timeb current;
	j1939_tsc1_typ *tsc = &cmd->cmd.tsc1;
	static int engine_speed_mode = 0;
	static timeb last_mode_change;

	/* There is an override time limit only for engine speed control */
	if (ctrl->engine_command_mode == TSC_SPEED_CONTROL) {
		if (engine_speed_mode &&
			TIMEB_SUBTRACT(&last_mode_change, &current) >
				cmd->override_limit) {
			engine_speed_mode = 0;
			tsc->override_control_modes = TSC_OVERRIDE_DISABLED;
		} else {
			engine_speed_mode = 1;
			tsc->override_control_modes = TSC_SPEED_CONTROL;;
		}
	} else {
		engine_speed_mode = 0;
		tsc->override_control_modes = ctrl->engine_command_mode;
	}
	tsc->requested_speed_or_limit = ctrl->engine_speed;

	/* convert torque to percent */
	tsc->requested_torque_or_limit =
		(100.0 * ctrl->engine_torque/max_engine_torque);

	if (dosend) {																// TODO(ak): figure out the point of this
		ftime(&current);
		cmd->last_time = current;
		if (engine_speed_mode == 0)
			last_mode_change = current;
	}

	// convert the TSC1 message to a PDU message
	j1939_pdu_typ *pdu = new j1939_pdu_typ();									// TODO(ak): do this

	return pdu;
}


bool EngineSender::ready_to_send() {
	timeb current;
	timeb *last_sent = &cmd->last_time;
	j1939_tsc1_typ *tsc = &cmd->cmd.tsc1;
	int new_mode = ctrl->engine_command_mode;
	int old_mode = tsc->override_control_modes;

	ftime(&current);

	return  // send heartbeat message
		(((TIMEB_SUBTRACT(last_sent, &current) >= cmd->heartbeat)
				&& cmd->heartbeat))
		||
		// send message to disable control
		(new_mode == TSC_OVERRIDE_DISABLED
			 && old_mode != TSC_OVERRIDE_DISABLED)
		||
		// called at 10 ms intervals, always send active engine msg
        	(new_mode != TSC_OVERRIDE_DISABLED)
		;
}


j1939_pdu_typ *EngineRetarderSender::cmd_to_pdu() {
	// collect the TSC1 command from the control message
	timeb current;
	j1939_tsc1_typ *tsc1 = &cmd->cmd.tsc1;

	tsc1->override_control_modes = ctrl->engine_retarder_command_mode;
	tsc1->requested_torque_or_limit =
			(100.0 * ctrl->engine_retarder_torque/max_retarder_torque);

	if (dosend) {																// TODO(ak): figure out the point of this
		ftime(&current);
		cmd->last_time = current;
	}

	// convert the TSC1 message to a PDU message
	j1939_pdu_typ *pdu = new j1939_pdu_typ();									// TODO(ak): do this

	return pdu;
}


bool EngineRetarderSender::ready_to_send() {
	timeb current;
	timeb *last_sent = &cmd->last_time;
	j1939_tsc1_typ *tsc = &cmd->cmd.tsc1;
	int new_mode = ctrl->engine_retarder_command_mode;
	int old_mode = tsc->override_control_modes;

	ftime(&current);

	return  // no heartbeat message for engine retarder
		// send message to disable control
		((new_mode == TSC_OVERRIDE_DISABLED
			 && old_mode != TSC_OVERRIDE_DISABLED)
		||
		// send active message every 50 milliseconds
        	((new_mode != TSC_OVERRIDE_DISABLED)
		&& (TIMEB_SUBTRACT(last_sent, &current) >= cmd->interval)))
		;
}


j1939_pdu_typ *EBSSender::cmd_to_pdu() {
	// collect the EXAC command from the control message
	timeb current;
	j1939_exac_typ *exac = &cmd->cmd.exac;

	exac->external_deceleration_control_mode = ctrl->brake_command_mode;
#ifdef DEBUG_BRAKE
	printf("long_output send %d ebs deceleration %.3f\n", dosend,
				ctrl->ebs_deceleration);
#endif
	exac->requested_deceleration_to_ebs = ctrl->ebs_deceleration;

	if (dosend) {																// TODO(ak): figure out the point of this
		exac->alive_signal++;
		ftime(&current);
		cmd->last_time = current;
	}

	// convert the EXAC message to a PDU message
	j1939_pdu_typ *pdu = new j1939_pdu_typ();									// TODO(ak): do this

	return pdu;
}


bool EBSSender::ready_to_send() {
	timeb current;
	timeb *last_sent = &cmd->last_time;

	ftime(&current);

	/* message sent every 40 ms whether active or inactive */
	return (TIMEB_SUBTRACT(last_sent, &current) >= cmd->interval);
}


j1939_pdu_typ *TransRetarderSender::cmd_to_pdu() {
	// collect the TSC1 command from the control message
	timeb current;
	j1939_tsc1_typ *tsc1 = &cmd->cmd.tsc1;

	tsc1->override_control_modes = ctrl->trans_retarder_command_mode;

	// since we do not have a maximum reference torque available, let the
	// controller decide how to set this as a percent
	tsc1->requested_torque_or_limit =ctrl->trans_retarder_value;

	if (dosend) {																// TODO(ak): figure out the point of this
		ftime(&current);
		cmd->last_time = current;
	}

	// convert the TSC1 message to a PDU message
	j1939_pdu_typ *pdu = new j1939_pdu_typ();									// TODO(ak): do this

	return pdu;
}


/** Maximum idle percent torque value; greater value indicates transmission
 * retarder should not be enabled. */
#define IDLE_PERCENT_TORQUE	20.0


/** Positive value for long_output_typ acc_pedal_control indicates transmission
 * retarder should not be enabled. */
#define ACC_OFF_VOLTAGE	0.0


bool TransRetarderSender::ready_to_send() {
	timeb current;
	timeb *last_sent = &cmd->last_time;
	j1939_tsc1_typ *tsc = &cmd->cmd.tsc1;
	int new_mode = ctrl->trans_retarder_command_mode;
	int old_mode = tsc->override_control_modes;

	ftime(&current);

#if 0
	// make sure not to turn on during acceleration, and to turn off if active
	// during acceleration.
	if (new_mode == TSC_TORQUE_CONTROL) {
		if (pinfo->percent_engine_torque > IDLE_PERCENT_TORQUE ||
		    pinfo->accelerator_pedal_position > 0.0 ||
		    ctrl->acc_pedal_control > ACC_OFF_VOLTAGE ) {
			printf("both acc and trans retarder active\n");
			ctrl->trans_retarder_command_mode = new_mode =
				TSC_OVERRIDE_DISABLED;
		}
	}
#endif
	return  // no heartbeat message for transmission retarder
		// send message to disable control
		((new_mode == TSC_OVERRIDE_DISABLED
			 && old_mode != TSC_OVERRIDE_DISABLED)
		||
		// send active message every 50 milliseconds
        	((new_mode != TSC_OVERRIDE_DISABLED)
		&& (TIMEB_SUBTRACT(last_sent, &current) >= cmd->interval)))
		;

}
