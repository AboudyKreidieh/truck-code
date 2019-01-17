///**\file
// *
// * jbus_send.cpp
// *
// * @author Abdul Rahman Kreidieh
// * @version 1.0.0
// * @date July 14, 2018
// */
//
//#include "jbus_send.h"
//
//extern void print_long_output_typ (long_output_typ *ctrl);
//static struct avcs_timing tmg;
//
///* these are values for trucks, read realtime.ini for buses */
//static float max_engine_torque = MAX_ENGINE_TORQUE;
//static float max_retarder_torque = MAX_RETARDER_TORQUE;
//
//int debug = 0;
//
//static int sig_list[]=
//{
//	SIGINT,
//	SIGQUIT,
//	SIGTERM,
//	SIGALRM,
//	ERROR
//};
//
//static jmp_buf exit_env;
//
//static void sig_hand(int code)
//{
//	if (code == SIGALRM)
//		return;
//	else
//		longjmp(exit_env, code);
//}
//
//#define MAX_LOOP_NUMBER 8
//
///**
// * main calls active_loop every 5 milliseconds, loop numbers range from 0 to 8
// * Engine message can be sent every 10 milliseconds, retarders and brake
// * every 40 milliseconds, with 10 millisecond start-up latency for engine,
// * 20 ms for brake and 40 ms for retarders..
// */
//int active_loop(int i, int loop_number)
//
//{
//	switch(i) {
//	case JBUS_SEND_ENGINE:
//		 return (loop_number % 2 == 0);
//	case JBUS_SEND_ENGINE_RETARDER:
//		 return (loop_number == 1);
//	case JBUS_SEND_EBS:
//		return (loop_number == 3) || (loop_number == 7);
//	case JBUS_SEND_TRANS_RETARDER:
//		 return (loop_number == 5);
//	}
//	return 0;
//}
//static int update_invalid_read = 0;
//
///**
// * update_info_for_check
// *
// * If database is created and control is active, read the database
// * variables needed to monitor engine behavior.
// */
//void update_info_for_check (db_clt_typ *pclt, info_check_type *pinfo,
//	int loop_number)
//
//{
//	db_data_typ db_data;
//	j1939_eec1_typ eec1;
//	j1939_eec2_typ eec2;
//	int valid_read;
//
//
//	/* if no database, just return */
//	if (pclt == NULL)
//		return;
//
//	/* Only read every 20 milliseconds, since messages are not
//	 * updated faster than that */
//	if (!(loop_number == 0 || loop_number == 4))
//		return;
//
//        valid_read = clt_read(pclt, DB_J1939_EEC1_VAR,
//				 DB_J1939_EEC1_TYPE, &db_data);
//	if (valid_read) {
//		eec1 = *((j1939_eec1_typ *) &db_data.value.user);
//		pinfo->percent_engine_torque =
//				eec1.actual_engine_percent_torque;
//	} else
//		update_invalid_read++;
//
//        valid_read =clt_read(pclt, DB_J1939_EEC2_VAR,
//				DB_J1939_EEC2_TYPE, &db_data);
//	if (valid_read) {
//		eec2 = *((j1939_eec2_typ *) &db_data.value.user);
//		pinfo->accelerator_pedal_position =
//				 eec2.accelerator_pedal_position;
//	} else
//		update_invalid_read++;
//
//}
//
//
///**
// * Initializes send_jbus_type structures for each active message type.
// * Message type is active if corresponding bit in active_mask is set to 1.
// * Complications are due to different possible system configurations:
// * one, two or three different device files may be used for the engine,
// * brake and transmission J1939 network communications.
// *
// * Returns 0 if no device file can be opened; uses engine device for
// * brakes and transmission if transmission or brake files are NULL and
// * transmission retarder or brakes are active.
// *
// * Also returns 0 if no active message types, otherwise returns the number
// * of active message types.
// */
//
//int send_jbus_init (
//		jbus_func_t *pjbf,
//		send_jbus_type *msg,
//		info_check_type *pinfo,
//		int active_mask,
//		char *engine_file,
//		char *brake_file,
//		char *trans_file)
//{
//	int i;
//	int active_message_types = 0;
//	int engine_fd = -1;
//	int brake_fd = -1;
//	int trans_fd = -1;
//	struct timeb current;
//
//	ftime(&current);	/* use to initialize "last_time" */
//	for (i = 0; i < NUM_JBUS_SEND; i++) {
//		send_jbus_type *pm = &msg[i];
//		jbus_cmd_type *cmd = &msg[i].cmd;
//		j1939_tsc1_typ *tsc = &cmd->cmd.tsc1;
//		j1939_exac_typ *exac = &cmd->cmd.exac;
//
//		pm->total_sent = 0;
//		if (!(active_mask & (1 << i))){
//			pm->active = 0;
//			if (debug)
//				printf("Message type %d inactive\n", i);
//			continue;
//		} else if (debug) {
//			printf("Message type %d active\n", i);
//		}
//
//		active_message_types++;
//
//		/* same initializations for all types */
//		pm->active = 1;
//		cmd->last_time = current;
//		cmd->pinfo = pinfo;
//
//		/* set up default values for inactive message;
//		 * we pretend to be the source address that the ECM
//		 * we are commanding is willing to listen to
//		 */
//		switch (i) {
//			case JBUS_SEND_ENGINE:
//				pm->slot_number = 11;
//				pm->is_ready_to_send = ready_to_send_engine;
//				pm->update_command = update_engine_tsc;
//				pm->cmd_to_pdu = (void *) tsc1_to_pdu;
//				cmd->dbv = DB_J1939_TSC1_VAR;
//				cmd->interval = 10;
//				cmd->heartbeat = 200;
//				cmd->override_limit = 5000;
//				tsc->override_control_mode_priority = TSC_HIGHEST;
//				/* Cummins only supports speed control condition 01 */
//				tsc->requested_speed_control_conditions = 1;
//				tsc->override_control_modes = TSC_OVERRIDE_DISABLED;
//				tsc->requested_speed_or_limit = 0.0;
//				tsc->requested_torque_or_limit = 0.0;
//				tsc->destination_address = J1939_ADDR_ENGINE;
//				tsc->src_address = J1939_ADDR_TRANS;
//				break;
//			case JBUS_SEND_ENGINE_RETARDER:
//				pm->slot_number = 12;
//				pm->is_ready_to_send = ready_to_send_engine_retarder;
//				pm->update_command = update_engine_retarder_tsc;
//				pm->cmd_to_pdu = (void *) tsc1_to_pdu;
//				cmd->dbv = DB_J1939_TSC1_RTDR_VAR;
//				cmd->interval = 40;
//				cmd->heartbeat = 0;	/* not used */
//				cmd->override_limit = 0;	/* not used */
//				tsc->override_control_mode_priority = TSC_HIGHEST;
//				/* Cummins only supports speed control condition 01 */
//				tsc->requested_speed_control_conditions = 1;
//				tsc->override_control_modes = TSC_OVERRIDE_DISABLED;
//				tsc->requested_speed_or_limit = 0.0;
//				tsc->requested_torque_or_limit = 0.0;
//				tsc->destination_address = J1939_ADDR_ENG_RTDR;
//				tsc->src_address = J1939_ADDR_TRANS;
//				break;
//			case JBUS_SEND_EBS:
//				pm->slot_number = 13;
//				pm->is_ready_to_send = ready_to_send_ebs;
//				pm->update_command = update_brake_exac;
//				pm->cmd_to_pdu = (void *) exac_to_pdu;
//				cmd->dbv = DB_J1939_EXAC_VAR;
//				cmd->interval = 40;
//				cmd->heartbeat = 40;
//				cmd->override_limit = 0;	/* not used */
//				exac->ebs_override_control_mode_priority = TSC_HIGHEST;
//				exac->external_deceleration_control_mode = EXAC_NOT_ACTIVE;
//				exac->requested_deceleration_to_ebs = 0.0;
//				exac->edc_override_control_mode_priority = 3; /* lowest */
//				exac->override_control_modes = TSC_OVERRIDE_DISABLED;
//				exac->requested_torque_to_edc = 0.0;
//				exac->alive_signal = 0;
//				exac->acc_internal_status = J1939_BYTE_UNDEFINED;
//				exac->undefined = J1939_BYTE_UNDEFINED;
//				exac->src_address = J1939_ADDR_ACC;
//				break;
//			case JBUS_SEND_TRANS_RETARDER:
//				pm->slot_number = 10;
//				pm->is_ready_to_send = ready_to_send_trans_retarder;
//				pm->update_command = update_trans_retarder_tsc;
//				pm->cmd_to_pdu = (void *) tsc1_to_pdu;
//				cmd->dbv = DB_J1939_TSC1_RTDR_VAR;
//				cmd->interval = 40;
//				cmd->heartbeat = 0;	/* not used */
//				cmd->override_limit = 0;	/* not used */
//				tsc->override_control_mode_priority = TSC_LOW;
//				tsc->requested_speed_control_conditions = 0; /* not used */
//				tsc->override_control_modes = TSC_OVERRIDE_DISABLED;
//				tsc->requested_speed_or_limit = 0.0;
//				tsc->requested_torque_or_limit = 0.0;
//				tsc->destination_address = J1939_ADDR_TR_RTDR;
//				tsc->src_address = J1939_ADDR_CC;
//				break;
//		}
//	}
//
//	if (!active_message_types) return 0;
//
//	if (brake_file == NULL && engine_file != NULL)
//		brake_file = engine_file;
//
//	if (trans_file == NULL && engine_file != NULL)
//		trans_file = engine_file;
//
//	if (msg[JBUS_SEND_ENGINE].active ||
//		 	msg[JBUS_SEND_ENGINE_RETARDER].active) {
//		if (engine_file == NULL) {
//			printf("no engine device specified\n");
//			fflush(stdout);
//			return 0;	/* must be defined */
//		}
//		engine_fd = (pjbf->init)(engine_file, O_WRONLY, NULL);
//		if (engine_fd < 0) {
//			printf("open error, engine file");
//			printf(" %s fd %d\n", engine_file, engine_fd);
//			fflush(stdout);
//			return 0;
//		} else if (debug) {
//			printf("successfully open engine %s\n", engine_file);
//			fflush(stdout);
//		}
//		msg[JBUS_SEND_ENGINE].fd = engine_fd;
//		msg[JBUS_SEND_ENGINE_RETARDER].fd = engine_fd;
//		printf("engine_fd %d\n", engine_fd);
//	}
//	if (msg[JBUS_SEND_EBS].active && brake_file != NULL) {
//		if (strcmp(brake_file, engine_file) == 0 && engine_fd >= 0)
//			brake_fd = engine_fd;
//		else {
//			brake_fd = (pjbf->init)(brake_file, O_WRONLY, NULL);
//			if (brake_fd < 0) {
//				printf("open error, brake file");
//				printf(" %s fd %d\n", brake_file, brake_fd);
//				fflush(stdout);
//				return 0;
//			} else if (debug) {
//				printf("successfully open brake file %s\n",
//						brake_file);
//				fflush(stdout);
//			}
//		}
//		msg[JBUS_SEND_EBS].fd = brake_fd;
//	}
//	if (msg[JBUS_SEND_TRANS_RETARDER].active) {
//		if (strcmp(trans_file, engine_file) == 0 && engine_fd >= 0)
//			trans_fd = engine_fd;
//		else if (strcmp(trans_file, brake_file) == 0 && brake_fd >= 0)
//			trans_fd = brake_fd;
//		else {
//			trans_fd = (pjbf->init)(trans_file, O_WRONLY, NULL);
//			if (trans_fd < 0) {
//				printf("open error, transmission file");
//				printf(" %s fd %d\n", trans_file, trans_fd);
//				fflush(stdout);
//				return 0;
//			} else if (debug) {
//				printf("successfully open trans %s\n", trans_file);
//				fflush(stdout);
//			}
//		}
//		msg[JBUS_SEND_TRANS_RETARDER].fd = trans_fd;
//		printf("trans_fd: %d\n", trans_fd);
//	}
//	if (debug) {
//		printf("Initialized %d messages\n ", active_message_types);
//		fflush(stdout);
//	}
//	return active_message_types;
//}
//
//
///**
// * Send disable messages before exiting and close files; this should
// * happen only with an emergency shutdown.
// */
//void send_jbus_exit (jbus_func_t *pjbf, send_jbus_type *msg)
//{
//	struct j1939_pdu pdu;
//	send_jbus_type *pm;
//	jbus_cmd_type *cmd;
//	j1939_tsc1_typ *ptsc;
//	j1939_exac_typ *pexac;
//	int i;
//
//	for (i = 0; i < NUM_JBUS_SEND; i++) {
//		pm = &msg[i];
//		cmd = &pm->cmd;
//		if (pm->active) {
//			if (i == JBUS_SEND_EBS) {
//				pexac = &cmd->cmd.exac;
//				pexac->external_deceleration_control_mode = EXAC_NOT_ACTIVE;
//				exac_to_pdu(&pdu, pexac);
//			}
//			else {
//				ptsc = &cmd->cmd.tsc1;
//				ptsc->override_control_modes = TSC_OVERRIDE_DISABLED;
//				tsc1_to_pdu(&pdu, ptsc);
//			}
//			(pjbf->send)(pm->fd, &pdu, pm->slot_number);
//		}
//	}
//}
//
//
//static long_output_typ inactive_ctrl = {
//	600.0, 					// engine speed, truck idle, don't care since disabled
//	400.0,  				// engine torque, truck idle, don't care since disabled
//    0.0,  					// retarder torque, don't care since disabled
//	TSC_OVERRIDE_DISABLED,	// engine command mode
//	TSC_OVERRIDE_DISABLED,	// engine retarder command mode
//	0.0,					// accelerator pedal voltage -- not used by jbus_send
//	0.0,					// ebs deceleration
//	EXAC_NOT_ACTIVE,		// brake command mode
//	0.0,					// percent of maximum transmission retarder activation
//	TSC_OVERRIDE_DISABLED,	// transmission retarder command mode
//};
//
//
///* Under automatic control, reads longitudinal control variable from
// * database. If database not active, or variable not created, returns
// * inactive control values. Keeps trying to open database every time
// * it is called if initial failure.
// */
//long_output_typ get_ctrl_var (db_clt_typ **ppclt, char *hostname,
//		char *progname)
//{
//	db_clt_typ *pclt;
//	db_data_typ read_data_var;
//	int valid_read;
//	if (*ppclt == NULL) {
//		pclt = clt_login(progname, hostname, DEFAULT_SERVICE,
//			COMM_OS_XPORT);
//		*ppclt = pclt;
//		if (pclt == NULL) {
//			return (inactive_ctrl);
//		}
//		if (debug)
//			printf("%s logged in to database 0x%x\n", progname,
//				(unsigned int) pclt);
//	} else
//		pclt = *ppclt;
//
//	valid_read = clt_read(pclt, DB_LONG_OUTPUT_VAR,
//			 DB_LONG_OUTPUT_TYPE, &read_data_var);
//
//
//	if (valid_read) {
//		if (debug)
//			print_long_output_typ((long_output_typ *) &read_data_var.value.user);
//		return *((long_output_typ *) &read_data_var.value.user);
//	}
//
//	else {
//		clt_logout(pclt);	/* just in case db_slv still there */
//		*ppclt = NULL;	/* assume need to reconnect to database */
//		return inactive_ctrl;
//	}
//}
//
//
///* for debugging */
//void print_send_jbus_type(send_jbus_type *pm)
//
//{
//	jbus_cmd_type *cmd = &pm->cmd;
//	j1939_tsc1_typ *tsc = &cmd->cmd.tsc1;
//	j1939_exac_typ *exac = &cmd->cmd.exac;
//	int dbv = cmd->dbv;
//
//	printf("Message to %s\n",
//		(dbv == DB_J1939_TSC1_VAR) ? "engine":
//		((dbv == DB_J1939_TSC1_RTDR_VAR)? "engine retarder":
//		((dbv == DB_J1939_EXAC_VAR)? "brake":
//		((dbv == DB_J1939_TSC1_TRTDR_VAR)? "trans retarder":
//				"unknown"))));
//	printf("\tactive %d, slot %d, total %d, fd %d\n",
//		 pm->active, pm->slot_number, pm->total_sent, pm->fd);
//
//	printf("\tready_to_send ptr 0x%x, update ptr 0x%x, to_pdu ptr 0x%x\n",
//		(unsigned int) pm->is_ready_to_send,
//		(unsigned int) pm->update_command,
//		(unsigned int) pm->cmd_to_pdu);
//
//	printf("\tinterval %d, heartbeat %d, override_limit %d\n",
//			cmd->interval, cmd->heartbeat, cmd->override_limit);
//
//	printf("\tlast_sent: sec %d millisec %d\n", cmd->last_time.time,
//			cmd->last_time.millitm);
//
//	switch (dbv) {
//	case DB_J1939_TSC1_VAR:
//		printf("\tmode %d, engine speed %.3f, torque %.3f\n",
//			tsc->override_control_modes,
//			tsc->requested_speed_or_limit,
//			tsc->requested_torque_or_limit);
//		printf("\tdestination %d, source %d\n", tsc->destination_address,
//			tsc->src_address);
//		break;
//	case DB_J1939_TSC1_RTDR_VAR:
//	case DB_J1939_TSC1_TRTDR_VAR:
//		printf("\tmode %d, torque %.3f\n",
//			tsc->override_control_modes,
//			tsc->requested_torque_or_limit);
//		printf("\tdestination %d, source %d\n", tsc->destination_address,
//			tsc->src_address);
//		break;
//	case DB_J1939_EXAC_VAR:
//		printf("\tebs mode %d, deceleration %.3f\n",
//			exac->external_deceleration_control_mode,
//			exac->requested_deceleration_to_ebs);
// 		printf("\tedc mode %d, edc torque %.3f, alive_signal %d\n",
//			exac->override_control_modes,
//			exac->requested_torque_to_edc,
//			exac->alive_signal);
//		break;
//	}
//	fflush(stdout);
//}
//
///* for debugging */
//void print_long_output_typ (long_output_typ *ctrl)
//{
//	printf("Ctrl engine speed %.3f\n",  ctrl->engine_speed);
//	printf("Ctrl engine torque %.3f\n",  ctrl->engine_torque);
//	printf("Ctrl engine retarder torque %.3f\n",  ctrl->engine_retarder_torque);
//	printf("Ctrl engine_command_mode %d\n", ctrl->engine_command_mode);
//	printf("Ctrl engine_retarder_command_mode %d\n", ctrl->engine_retarder_command_mode);
//	printf("Ctrl EBS deceleration %.3f\n",  ctrl->ebs_deceleration);
//	printf("Ctrl brake_command_mode %d\n", ctrl->brake_command_mode);
//	fflush(stdout);
//}
//
//
//
//
//int main( int argc, char *argv[] )
//{
//	int ch;
//	send_jbus_type msg[NUM_JBUS_SEND];
//	struct j1939_pdu pdu;				/* Protocol Data Unit */
//        db_clt_typ *pclt = NULL;  		/* Database pointer */
//	long_output_typ ctrl;				/* Variable containing control values */
//	posix_timer_typ *ptimer;      		/* Timing proxy */
//	int interval = JBUS_INTERVAL_MSECS; /* Main loop execution interval */
//	int loop_number = 0;				/* Used to space out messages */
//	char *engine_file = NULL;
//	char *brake_file = NULL;
//	char *trans_file = NULL;
//	jbus_func_t jfunc;
//	int active_mask = (1 << NUM_JBUS_SEND) - 1;	/* all active */
//	int rtn_jmp = -1;					/* value returned from setjmp */
//	char hostname[MAXHOSTNAMELEN+1]; 	/* used in opening database */
//	char *progname = argv[0];			/* used in opening database */
//	char *vehicle_str = "Blue";			/* vehicle identifier */
//	info_check_type current_info;		/* info for monitoring safety */
//	FILE *fpin;
//
//	get_local_name(hostname, MAXHOSTNAMELEN);
//
//	/* default, use STB serial port converter */
//	jfunc.send = send_stb;
//	jfunc.receive = receive_stb;
//
//	while ((ch = getopt(argc, argv, "a:b:cde:t:v:")) != EOF) {
//		switch (ch) {
//			case 'a':
//				active_mask = atoi(optarg);
//				break;
//			case 'b':
//				brake_file = strdup(optarg);
//				break;
//			case 'c':
//				jfunc.send = send_can;
//				jfunc.receive = receive_can;
//				jfunc.init = init_can;
//				break;
//			case 'd':
//				debug = 1;
//				break;
//			case 'e':
//				engine_file = strdup(optarg);
//				break;
//			case 't':
//				trans_file = strdup(optarg);
//				break;
//			case 'v':
//				vehicle_str = strdup(optarg);
//				break;
//			default:
//				printf( "Usage: %s ", argv[0]);
//				printf("-b brake device -e engine device\n");
//				printf("-c(use CAN) -d(debug) \n");
//				exit(EXIT_FAILURE);
//				break;
//		}
//	}
//
//	if (debug) {
//		printf("jbus_send: active mask 0x%x\n", active_mask);
//		printf("Eng %s Brake %s Trans %s", engine_file,
//			brake_file ? brake_file : engine_file,
//			trans_file ? trans_file : engine_file);
//		fflush(stdout);
//	}
//
//	if ((fpin = get_ini_section("/home/truckcontrol/test/jparam.ini",
//			vehicle_str)) == NULL) {
//		printf("Cannot get ini file realtime.ini, section %s\n", vehicle_str);
//		fflush(stdout);
//	}
//	else
//		printf("Initialization file %s\n", "realtime.ini");
//
//	max_engine_torque = get_ini_double(fpin, "EngineReferenceTorque", MAX_ENGINE_TORQUE);    /* RPM */
//	max_retarder_torque = get_ini_double(fpin, "RetarderReferenceTorque", MAX_RETARDER_TORQUE);    /* RPM */
//
//	if (!send_jbus_init(&jfunc, msg, &current_info, active_mask,
//		 engine_file, brake_file, trans_file)) {
//		printf("Send structure initialization failed\n");
//		fflush(stdout);
//		exit (EXIT_FAILURE);
//	}
//
//	if (debug) {
//		int i;
//		for (i = 0; i < NUM_JBUS_SEND; i++)
//			print_send_jbus_type(&msg[i]);
//	}
//
//	if ((ptimer = timer_init( interval, 0 )) == NULL) {
//		printf("Unable to initialize jbussend timer\n");
//		exit (EXIT_FAILURE);
//	}
//	avcs_start_timing(&tmg);
//
//	if( (rtn_jmp = setjmp( exit_env )) != 0 ) {
//		avcs_end_timing(&tmg);
//		send_jbus_exit(&jfunc, msg);
//                close_local_database(pclt);
//
//		avcs_print_timing(stdout, &tmg);
//		fprintf(stderr, "jbussend exit %d\n", rtn_jmp);
//		fflush(stderr);
//		printf("jbussend, %d engine, %d engine retarder, %d brake, ",
//			msg[JBUS_SEND_ENGINE].total_sent,
//			msg[JBUS_SEND_ENGINE_RETARDER].total_sent,
//			msg[JBUS_SEND_EBS].total_sent);
//		printf("%d trans retarder -- exit %d\n",
//			msg[JBUS_SEND_TRANS_RETARDER].total_sent,
//			rtn_jmp);
//		printf("%d invalid data server reads\n", update_invalid_read);
//
//		exit( EXIT_SUCCESS );
//	}
//	else
//		sig_ign( sig_list, sig_hand );
//
//	if (debug) {
//		printf("Begin loop, interval %d\n", interval);
//		fflush(stdout);
//	}
//	update_info_for_check(pclt, &current_info, loop_number);
//	for ( ; ; ) {
//		int i;	/* counter for message types */
//		tmg.exec_num++;
//
//		ctrl = get_ctrl_var(&pclt, hostname, progname);
//
//		for (i = 0; i < NUM_JBUS_SEND; i++) {
//			send_jbus_type *pm = &msg[i];
//			if (pm->active) {
//				int can_send;
//				jbus_cmd_type *cmd = &pm->cmd;
//				can_send = active_loop(i, loop_number)
//					&& (pm->is_ready_to_send)(&ctrl, cmd);
//				pm->update_command(&ctrl, cmd, can_send);
//				if (can_send) {
//					if (i == JBUS_SEND_EBS)
//						(pm->cmd_to_pdu)(&pdu, &cmd->cmd.exac);
//					else
//						(pm->cmd_to_pdu)(&pdu, &cmd->cmd.tsc1);
//
//					jfunc.send(pm->fd, &pdu, pm->slot_number);
//					if (debug) {
//						if (i == 3) {
//							j1939_pdu_typ pdu_out;
//							pdu_to_pdu(&pdu, &pdu_out);
//							print_pdu(&pdu_out, stdout, 1);
//						}
//					}
//					pm->total_sent++;
//				}
//			}
//		}
//		loop_number++;
//		if (loop_number == MAX_LOOP_NUMBER) loop_number = 0;
//
//		/* update info before waiting */
//		update_info_for_check(pclt, &current_info, loop_number);
//
//		/* Now wait for proxy from timer */
//		TIMER_WAIT( ptimer );
//	}
//}
