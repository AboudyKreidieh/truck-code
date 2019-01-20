///**\file
// *
// * long_out.cpp
// *
// * Writes the DB_LONG_OUTPUT variable with values for a particular command.
// * Loops, waking up periodically, until killed by signal. Writes override
// * disable values for engine and retarder to database variable, then exits.
// *
// * Usage:
// * 		long_out
// *  	-v speed (RPM)
// *  	-t torque (N-m)
// *  	-b deceleration (m/s^2)
// *  	-n slot
// *  	-f serial port
// *  	-c state code
// *  	-d destination
// *  	-i interval
// *
// * @author Abdul Rahman Kreidieh
// * @version 1.0.0
// * @date January 10, 2019
// */
//
//#include "long_controller.h"
//#include "long_utils.h"
//
//#define INTERVAL_MSECS	100  /**< wake-up interval set at 100 milliseconds */
//
//#include "sys_os.h"
//#include "local.h"
//#include "sys_rt.h"
//#include "db_clt.h"
//#include "db_utils.h"
//#include "clt_vars.h"
//#include "vehicle.h"
//#include "timestamp.h"
//#include "jbus_vars.h"
//#include "jbus.h"
//#include "j1939.h"
//#include "j1939pdu.h"
//#include "j1939db.h"
//#include <timing.h>
//
//static struct avcs_timing tmg;
//
//static int sig_list[]=
//{
//	SIGINT,
//	SIGQUIT,
//	SIGTERM,
//	ERROR
//};
//
//
//static jmp_buf exit_env;
//
//
//static void sig_hand( int sig)
//{
//	longjmp( exit_env, 1);
//}
//
//int main( int argc, char *argv[] )
//{
//	int ch;
//	db_clt_typ *pclt;  						/* Database pointer */
//	posix_timer_typ *ptimer;      	 		/* Timing proxy */
//	int interval = INTERVAL_MSECS;			/* */
//	int engine_mode = TSC_SPEED_CONTROL;	/* */
//	float rqst_speed = 800.0;				/* RPMs */
//	float rqst_torque = 0.0;				/* */
//	float rqst_brake = 0.0;					/* */
//	int destination_address = 0;			/* engine; retarder is 15 */
//
//	long_output_typ cmd_var;
//
//	pclt = j1939_database_init(argv);
//
//	while ((ch = getopt(argc, argv, "b:d:i:m:t:v:")) != EOF) {
//			switch (ch) {
//	case 'b': rqst_brake = atof(optarg);
//		  destination_address = J1939_ADDR_BRAKE;
//		  printf("deceleration requested %6.4f\n", rqst_brake);
//		  break;
//	case 'd': destination_address = atoi(optarg);
//		  break;
//	case 'i': interval = atoi(optarg);
//		  break;
//	case 'm': engine_mode = atoi(optarg);
//		  printf("mode is %d\n", engine_mode);
//		  break;
//	case 't': rqst_torque = atof(optarg);
//		  printf("torque requested %.3f N-m\n", rqst_torque);
//		  break;
//	case 'v': rqst_speed = atof(optarg);
//		  printf("speed requested %.3f RPM\n", rqst_speed);
//		  break;
//	default:
//		printf("Usage: %s [-v engine speed(RPM) -t torque(N-m)", argv[0]);
//		printf("-b deceleration (m/s^2) \n");
//		printf("-d destination -i interval]\n");
//		exit(1);
//		break;}
//	}
//	fflush(stdout);
//
//	if( setjmp( exit_env ) != 0 ) {
//		char tmg_filename[80];
//		FILE *tmg_stream;
//		cmd_var.engine_command_mode = TSC_OVERRIDE_DISABLED;
//		cmd_var.engine_retarder_command_mode = TSC_OVERRIDE_DISABLED;
//		cmd_var.brake_command_mode = EXAC_NOT_ACTIVE;
//		if( clt_update( pclt, DB_LONG_OUTPUT_VAR, DB_LONG_OUTPUT_TYPE,
//		    sizeof( long_output_typ ), (void *) &cmd_var ) == FALSE ) {
//		    fprintf(stderr, "clt_update( DB_LONG_OUTPUT_VAR )\n" );
//		}
//		avcs_end_timing(&tmg);
//                close_local_database(pclt);
//		sprintf(tmg_filename, "tsc_spd_%.3f.tmg", rqst_speed);
//		tmg_stream = fopen(tmg_filename, "w");
//		avcs_print_timing(tmg_stream, &tmg);
//		fclose(tmg_stream);
//		exit( EXIT_SUCCESS );
//	}
//	else
//		sig_ign( sig_list, sig_hand );
//
//	/** Update database **/
//	if (destination_address == J1939_ADDR_ENGINE) {
//		cmd_var.engine_retarder_command_mode = 0;
//
//		cmd_var.engine_command_mode = engine_mode;
//		cmd_var.brake_command_mode = EXAC_NOT_ACTIVE;
//		cmd_var.ebs_deceleration = 0.0;
//		switch(engine_mode) {
//			case 0: 	/* disable */
//				break;
//			case 1: 	/* speed */
//				cmd_var.engine_speed = rqst_speed;
//				break;
//			case 2: 	/* torque */
//				cmd_var.engine_torque = rqst_torque;
//				break;
//			case 3: 	/* speed/torque limit */
//				cmd_var.engine_speed = rqst_speed;
//				cmd_var.engine_torque = rqst_torque;
//				break;
//			default:
//				fprintf(stderr,
//						"Override mode not recognized for engine in "
//						"tscj1939\n");
//				break;
//		}
//	}
//	else if (destination_address == J1939_ADDR_ENG_RTDR) {
//		cmd_var.engine_command_mode = 0;
//
//		cmd_var.engine_retarder_command_mode = engine_mode;
//		cmd_var.brake_command_mode = EXAC_NOT_ACTIVE;
//		cmd_var.ebs_deceleration = 0.0;
//		switch(engine_mode){
//			case 0: 	/* disable */
//				break;
//			case 2: 	/* torque */
//			case 3:		/* torque limit */
//				cmd_var.engine_retarder_torque = rqst_torque;
//				break;
//			default:
//				fprintf(stderr,
//						"Override mode not recognized for retarder in "
//						"tscj1939\n");
//				break;
//		}
//	}
//	else if (destination_address == J1939_ADDR_BRAKE) {
//		/* disable acceleration, don't use engine retarder */
//		cmd_var.engine_command_mode = TSC_TORQUE_CONTROL;
//		cmd_var.engine_torque = 0.0;
//		cmd_var.engine_retarder_command_mode = TSC_OVERRIDE_DISABLED;
//		cmd_var.brake_command_mode = EXAC_ACTIVE;
//		cmd_var.ebs_deceleration = rqst_brake;
//	}
//	else {
//		fprintf(stderr, "Destination address not recognized\n");
//	}
//
//	if( clt_update( pclt, DB_LONG_OUTPUT_VAR, DB_LONG_OUTPUT_TYPE,
//	    sizeof( long_output_typ ), (void *) &cmd_var ) == FALSE ) {
//		    fprintf(stderr, "clt_update( DB_LONG_OUTPUT_VAR )\n" );
//	}
//
//	if ((ptimer = timer_init( interval, 0 )) == NULL) {
//		printf("Unable to initialize long_out timer\n");
//	}
//
//	avcs_start_timing(&tmg);
//
//	for ( ; ; ) {
//		tmg.exec_num++;
//
//		/* Loop, on signal will turn off all commands */
//		TIMER_WAIT( ptimer );
//
//	}
//	longjmp(exit_env,1);	/* go to exit code when loop terminates */
//}
//
//
////j1939_pdu_typ *EngineSender::cmd_to_pdu() {
////	// collect the TSC1 command from the control message
////	timeb current;
////	j1939_tsc1_typ *tsc = &cmd->cmd.tsc1;
////	static int engine_speed_mode = 0;
////	static timeb last_mode_change;
////
////	/* There is an override time limit only for engine speed control */
////	if (ctrl->engine_command_mode == TSC_SPEED_CONTROL) {
////		if (engine_speed_mode &&
////			TIMEB_SUBTRACT(&last_mode_change, &current) >
////				cmd->override_limit) {
////			engine_speed_mode = 0;
////			tsc->override_control_modes = TSC_OVERRIDE_DISABLED;
////		} else {
////			engine_speed_mode = 1;
////			tsc->override_control_modes = TSC_SPEED_CONTROL;;
////		}
////	} else {
////		engine_speed_mode = 0;
////		tsc->override_control_modes = ctrl->engine_command_mode;
////	}
////	tsc->requested_speed_or_limit = ctrl->engine_speed;
////
////	/* convert torque to percent */
////	tsc->requested_torque_or_limit =
////		(100.0 * ctrl->engine_torque/max_engine_torque);
////
////	if (dosend) {																// TODO(ak): figure out the point of this
////		ftime(&current);
////		cmd->last_time = current;
////		if (engine_speed_mode == 0)
////			last_mode_change = current;
////	}
////
////	// convert the TSC1 message to a PDU message
////	j1939_pdu_typ *pdu = new j1939_pdu_typ();									// TODO(ak): do this
////
////	return pdu;
////}
////
////
////bool EngineSender::ready_to_send() {
////	timeb current;
////	timeb *last_sent = &cmd->last_time;
////	j1939_tsc1_typ *tsc = &cmd->cmd.tsc1;
////	int new_mode = ctrl->engine_command_mode;
////	int old_mode = tsc->override_control_modes;
////
////	ftime(&current);
////
////	return  // send heartbeat message
////		(((TIMEB_SUBTRACT(last_sent, &current) >= cmd->heartbeat)
////				&& cmd->heartbeat))
////		||
////		// send message to disable control
////		(new_mode == TSC_OVERRIDE_DISABLED
////			 && old_mode != TSC_OVERRIDE_DISABLED)
////		||
////		// called at 10 ms intervals, always send active engine msg
////        	(new_mode != TSC_OVERRIDE_DISABLED)
////		;
////}
////
////
////j1939_pdu_typ *EngineRetarderSender::cmd_to_pdu() {
////	// collect the TSC1 command from the control message
////	timeb current;
////	j1939_tsc1_typ *tsc1 = &cmd->cmd.tsc1;
////
////	tsc1->override_control_modes = ctrl->engine_retarder_command_mode;
////	tsc1->requested_torque_or_limit =
////			(100.0 * ctrl->engine_retarder_torque/max_retarder_torque);
////
////	if (dosend) {																// TODO(ak): figure out the point of this
////		ftime(&current);
////		cmd->last_time = current;
////	}
////
////	// convert the TSC1 message to a PDU message
////	j1939_pdu_typ *pdu = new j1939_pdu_typ();									// TODO(ak): do this
////
////	return pdu;
////}
////
////
////bool EngineRetarderSender::ready_to_send() {
////	timeb current;
////	timeb *last_sent = &cmd->last_time;
////	j1939_tsc1_typ *tsc = &cmd->cmd.tsc1;
////	int new_mode = ctrl->engine_retarder_command_mode;
////	int old_mode = tsc->override_control_modes;
////
////	ftime(&current);
////
////	return  // no heartbeat message for engine retarder
////		// send message to disable control
////		((new_mode == TSC_OVERRIDE_DISABLED
////			 && old_mode != TSC_OVERRIDE_DISABLED)
////		||
////		// send active message every 50 milliseconds
////        	((new_mode != TSC_OVERRIDE_DISABLED)
////		&& (TIMEB_SUBTRACT(last_sent, &current) >= cmd->interval)))
////		;
////}
////
////
////j1939_pdu_typ *EBSSender::cmd_to_pdu() {
////	// collect the EXAC command from the control message
////	timeb current;
////	j1939_exac_typ *exac = &cmd->cmd.exac;
////
////	exac->external_deceleration_control_mode = ctrl->brake_command_mode;
////#ifdef DEBUG_BRAKE
////	printf("long_output send %d ebs deceleration %.3f\n", dosend,
////				ctrl->ebs_deceleration);
////#endif
////	exac->requested_deceleration_to_ebs = ctrl->ebs_deceleration;
////
////	if (dosend) {																// TODO(ak): figure out the point of this
////		exac->alive_signal++;
////		ftime(&current);
////		cmd->last_time = current;
////	}
////
////	// convert the EXAC message to a PDU message
////	j1939_pdu_typ *pdu = new j1939_pdu_typ();									// TODO(ak): do this
////
////	return pdu;
////}
////
////
////bool EBSSender::ready_to_send() {
////	timeb current;
////	timeb *last_sent = &cmd->last_time;
////
////	ftime(&current);
////
////	/* message sent every 40 ms whether active or inactive */
////	return (TIMEB_SUBTRACT(last_sent, &current) >= cmd->interval);
////}
////
////
////j1939_pdu_typ *TransRetarderSender::cmd_to_pdu() {
////	// collect the TSC1 command from the control message
////	timeb current;
////	j1939_tsc1_typ *tsc1 = &cmd->cmd.tsc1;
////
////	tsc1->override_control_modes = ctrl->trans_retarder_command_mode;
////
////	// since we do not have a maximum reference torque available, let the
////	// controller decide how to set this as a percent
////	tsc1->requested_torque_or_limit =ctrl->trans_retarder_value;
////
////	if (dosend) {																// TODO(ak): figure out the point of this
////		ftime(&current);
////		cmd->last_time = current;
////	}
////
////	// convert the TSC1 message to a PDU message
////	j1939_pdu_typ *pdu = new j1939_pdu_typ();									// TODO(ak): do this
////
////	return pdu;
////}
////
////
/////** Maximum idle percent torque value; greater value indicates transmission
//// * retarder should not be enabled. */
////#define IDLE_PERCENT_TORQUE	20.0
////
////
/////** Positive value for long_output_typ acc_pedal_control indicates transmission
//// * retarder should not be enabled. */
////#define ACC_OFF_VOLTAGE	0.0
////
////
////bool TransRetarderSender::ready_to_send() {
////	timeb current;
////	timeb *last_sent = &cmd->last_time;
////	j1939_tsc1_typ *tsc = &cmd->cmd.tsc1;
////	int new_mode = ctrl->trans_retarder_command_mode;
////	int old_mode = tsc->override_control_modes;
////
////	ftime(&current);
////
////#if 0
////	// make sure not to turn on during acceleration, and to turn off if active
////	// during acceleration.
////	if (new_mode == TSC_TORQUE_CONTROL) {
////		if (pinfo->percent_engine_torque > IDLE_PERCENT_TORQUE ||
////		    pinfo->accelerator_pedal_position > 0.0 ||
////		    ctrl->acc_pedal_control > ACC_OFF_VOLTAGE ) {
////			printf("both acc and trans retarder active\n");
////			ctrl->trans_retarder_command_mode = new_mode =
////				TSC_OVERRIDE_DISABLED;
////		}
////	}
////#endif
////	return  // no heartbeat message for transmission retarder
////		// send message to disable control
////		((new_mode == TSC_OVERRIDE_DISABLED
////			 && old_mode != TSC_OVERRIDE_DISABLED)
////		||
////		// send active message every 50 milliseconds
////        	((new_mode != TSC_OVERRIDE_DISABLED)
////		&& (TIMEB_SUBTRACT(last_sent, &current) >= cmd->interval)))
////		;
////
////}
//
