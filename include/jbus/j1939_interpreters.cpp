/**\file
 *
 * j1939_interpreters.cpp
 *
 * Function definitions for j1939_interpreter.h
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date May 31, 2018
 */

#include "j1939_interpreters.h"
#include "j1939_utils.h"
#include "utils/timestamp.h"
#include "utils/common.h"		/* BYTE */
#include <vector>
#include <string>
#include <map>

using namespace std;


int J1939Interpreter::pgn() {
	return 0;
}

bool J1939Interpreter::is_type(j1939_pdu_typ *pdu) {
	int target_pgn = TWOBYTES(pdu->pdu_format, pdu->pdu_specific);
    return (target_pgn == this->pgn());
}

J1939Interpreter::~J1939Interpreter() {}


/* -------------------------------------------------------------------------- */
/* ----------------------- In case not interpretable ------------------------ */
/* -------------------------------------------------------------------------- */


int PDUInterpreter::pgn() {
	return PDU;
}


void *PDUInterpreter::convert(j1939_pdu_typ *pdu) {
	return (void*) pdu;  // no changes are made
}


void PDUInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_pdu_typ *pdu = (j1939_pdu_typ*) pdv;

	fprintf(fp, "PDU");
	print_timestamp(fp, &pdu->timestamp);
	if (numeric) {
		fprintf(fp, " %d", pdu->priority);
		fprintf(fp, " %d", pdu->pdu_format);
		fprintf(fp, " %d", pdu->pdu_specific);
		fprintf(fp, " %d", pdu->src_address);
		fprintf(fp, " %d", pdu->num_bytes);
		for (int i=0; i<pdu->num_bytes; ++i)
			fprintf(fp, " %d", pdu->data_field[i]);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Priority %d\n", pdu->priority);
		fprintf(fp, " Protocol Data Unit Format (PF) %d\n", pdu->pdu_format);
		fprintf(fp, " PDU Specific (PS) %d\n", pdu->pdu_specific);
		fprintf(fp, " Source Address %d\n", pdu->src_address);
		fprintf(fp, " Number of bytes %d\n", pdu->num_bytes);
		fprintf(fp, " Data Field");
		for (int i=0; i<pdu->num_bytes; ++i)
			fprintf(fp, " %d", pdu->data_field[i]);
		fprintf(fp, "\n");
	}
}


void *PDUInterpreter::import(vector<string> &tokens) {
	j1939_pdu_typ *pdu = new j1939_pdu_typ();

	import_timestamp(&pdu->timestamp, tokens[1]);
    pdu->priority = stoi(tokens[2]);
	pdu->pdu_format = stoi(tokens[3]);
	pdu->pdu_specific = stoi(tokens[4]);
	pdu->src_address = stoi(tokens[5]);
	pdu->num_bytes = stoi(tokens[6]);
	for (int i=0; i<pdu->num_bytes; i++)
        pdu->data_field[i] = stoi(tokens[7+i]);

    return (void*) pdu;
}


/* -------------------------------------------------------------------------- */
/* ------------------------ Received from the brake ------------------------- */
/* -------------------------------------------------------------------------- */


int TSC1Interpreter::pgn() {
	return TSC1;
}


void *TSC1Interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_tsc1_typ *tsc1 = new j1939_tsc1_typ();
	tsc1->timestamp = pdu->timestamp;

	tsc1->src_address = pdu->src_address;
	tsc1->destination_address = pdu->pdu_specific;

    tsc1->ovrd_ctrl_m = BITS21(pdu->data_field[0]);
	tsc1->req_spd_ctrl = BITS43(pdu->data_field[0]);
	tsc1->ovrd_ctrl_m_pr = BITS65(pdu->data_field[0]);

	int data = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	tsc1->req_spd_lim = speed_in_rpm_2byte(data);

	tsc1->req_trq_lim = percent_m125_to_p125(pdu->data_field[3]);

	return (void*) tsc1;
}


void TSC1Interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_tsc1_typ *tsc1 = (j1939_tsc1_typ*) pdv;

	fprintf(fp, "TSC1");
	print_timestamp(fp, &tsc1->timestamp);
	if (numeric) {
		fprintf(fp, " %d", tsc1->destination_address);
		fprintf(fp, " %d", tsc1->src_address);
		fprintf(fp, " %d", tsc1->ovrd_ctrl_m_pr);
		fprintf(fp, " %d", tsc1->req_spd_ctrl);
		fprintf(fp, " %d", tsc1->ovrd_ctrl_m);
		fprintf(fp, " %.3f", tsc1->req_spd_lim);
		fprintf(fp, " %.3f", tsc1->req_trq_lim);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Destination %d\n", tsc1->destination_address);
		fprintf(fp, " Source address %d\n", tsc1->src_address);
		fprintf(fp, " Override Control Mode priority %d\n",
				tsc1->ovrd_ctrl_m_pr);
		fprintf(fp, " Requested speed control conditions %d\n",
				tsc1->req_spd_ctrl);
		fprintf(fp, " Override control mode %d\n", tsc1->ovrd_ctrl_m);
		fprintf(fp, " Requested speed/speed limit %.3f\n", tsc1->req_spd_lim);
		fprintf(fp, " Requested torque/torque limit %.3f\n", tsc1->req_trq_lim);
	}
}


void *TSC1Interpreter::import(vector<string> &tokens) {
	j1939_tsc1_typ *tsc1 = new j1939_tsc1_typ();

	import_timestamp(&tsc1->timestamp, tokens[1]);
	tsc1->destination_address = stoi(tokens[2]);
	tsc1->src_address = stoi(tokens[3]);
	tsc1->ovrd_ctrl_m_pr = stoi(tokens[4]);
	tsc1->req_spd_ctrl = stoi(tokens[5]);
	tsc1->ovrd_ctrl_m = stoi(tokens[6]);
	tsc1->req_spd_lim = stof(tokens[7]);
	tsc1->req_trq_lim = stof(tokens[8]);

	return (void*) tsc1;
}


int EBC1Interpreter::pgn() {
	return EBC1;
}


void *EBC1Interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_ebc1_typ *ebc1 = new j1939_ebc1_typ();
	ebc1->timestamp = pdu->timestamp;

	ebc1->ebs_brk_switch = BITS87(pdu->data_field[0]);
	ebc1->antilock_brk_active = BITS65(pdu->data_field[0]);
	ebc1->asr_brk_ctrl_active = BITS43(pdu->data_field[0]);
	ebc1->asr_engine_ctrl_active = BITS21(pdu->data_field[0]);

	ebc1->brk_pedal_pos = percent_0_to_100(pdu->data_field[1]);

	ebc1->trac_ctrl_override_switch = BITS87(pdu->data_field[2]);
	ebc1->asr_hillholder_switch = BITS65(pdu->data_field[2]);
	ebc1->asr_offroad_switch = BITS43(pdu->data_field[2]);
	ebc1->abs_offroad_switch = BITS21(pdu->data_field[2]);

	ebc1->accel_enable_switch = BITS87(pdu->data_field[3]);
	ebc1->aux_eng_shutdown_switch = BITS65(pdu->data_field[3]);
	ebc1->eng_derate_switch = BITS43(pdu->data_field[3]);
	ebc1->accel_interlock_switch = BITS21(pdu->data_field[3]);

	ebc1->eng_retarder_selection = percent_0_to_100(pdu->data_field[4]);

	ebc1->abs_ebs_amber_warning = BITS65(pdu->data_field[5]);
	ebc1->ebs_red_warning = BITS43(pdu->data_field[5]);
	ebc1->abs_fully_operational = BITS21(pdu->data_field[5]);

	ebc1->src_address_ctrl = pdu->data_field[6];

	ebc1->total_brk_demand = brake_demand(pdu->data_field[7]);

	return (void*) ebc1;
}


void EBC1Interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_ebc1_typ *ebc1 = (j1939_ebc1_typ*) pdv;

	fprintf(fp, "EBC1");
	print_timestamp(fp, &ebc1->timestamp);
	if (numeric) {
		fprintf(fp, " %d", ebc1->ebs_brk_switch);
		fprintf(fp, " %d", ebc1->antilock_brk_active);
		fprintf(fp, " %d", ebc1->asr_brk_ctrl_active);
		fprintf(fp, " %d", ebc1->asr_engine_ctrl_active);
		fprintf(fp, " %.2f", ebc1->brk_pedal_pos);
		fprintf(fp, " %d", ebc1->trac_ctrl_override_switch);
		fprintf(fp, " %d", ebc1->asr_hillholder_switch);
		fprintf(fp, " %d", ebc1->abs_offroad_switch);
		fprintf(fp, " %d", ebc1->asr_offroad_switch);
		fprintf(fp, " %d", ebc1->accel_enable_switch);
		fprintf(fp, " %d", ebc1->aux_eng_shutdown_switch);
		fprintf(fp, " %d", ebc1->eng_derate_switch);
		fprintf(fp, " %d", ebc1->accel_interlock_switch);
		fprintf(fp, " %.2f", ebc1->eng_retarder_selection);
		fprintf(fp, " %d", ebc1->abs_ebs_amber_warning);
		fprintf(fp, " %d", ebc1->ebs_red_warning);
		fprintf(fp, " %d", ebc1->abs_fully_operational);
	 	fprintf(fp, " %d", ebc1->src_address_ctrl);
	 	fprintf(fp, " %.3f", ebc1->total_brk_demand);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " EBS brake switch status %d\n", ebc1->ebs_brk_switch);
		fprintf(fp, " ABS active status %d\n", ebc1->antilock_brk_active);
		fprintf(fp, " ASR brake control status %d\n",
				ebc1->asr_brk_ctrl_active);
		fprintf(fp, " ASR engine control active status %d\n",
				ebc1->asr_engine_ctrl_active);
		fprintf(fp, " Brake pedal position %.2f\n", ebc1->brk_pedal_pos);
		fprintf(fp, " Traction control override switch status %d\n",
				ebc1->trac_ctrl_override_switch);
		fprintf(fp, " Hill holder switch status %d\n",
				ebc1->asr_hillholder_switch);
		fprintf(fp, " ABS off road switch status %d\n",
				ebc1->abs_offroad_switch);
		fprintf(fp, " ASR off road switch status %d\n",
				ebc1->asr_offroad_switch);
		fprintf(fp, " Remote accelerator enable switch status %d\n",
				ebc1->accel_enable_switch);
		fprintf(fp, " Auxiliary engine shutdown switch status %d\n",
				ebc1->aux_eng_shutdown_switch);
		fprintf(fp, " Engine derate switch status %d\n",
				ebc1->eng_derate_switch);
		fprintf(fp, " Accelerator interlock switch status %d\n",
				ebc1->accel_interlock_switch);
		fprintf(fp, " Percent engine retarder torque selected %.2f\n",
				ebc1->eng_retarder_selection);
		fprintf(fp, " ABS/EBS amber warning state %d\n",
				ebc1->abs_ebs_amber_warning);
		fprintf(fp, " EBS red warning state %d\n", ebc1->ebs_red_warning);
		fprintf(fp, " ABS fully operational %d\n", ebc1->abs_fully_operational);
		fprintf(fp, " Source address %d (0x%0x)\n",
				ebc1->src_address_ctrl, ebc1->src_address_ctrl);
		fprintf(fp, " Total brake demand %.3f\n", ebc1->total_brk_demand);
	}
}


void *EBC1Interpreter::import(vector<string> &tokens) {
	j1939_ebc1_typ *ebc1 = new j1939_ebc1_typ();

	import_timestamp(&ebc1->timestamp, tokens[1]);
	ebc1->ebs_brk_switch = stoi(tokens[2]);
	ebc1->antilock_brk_active = stoi(tokens[3]);
	ebc1->asr_brk_ctrl_active = stoi(tokens[4]);
	ebc1->asr_engine_ctrl_active = stoi(tokens[5]);
	ebc1->brk_pedal_pos = stof(tokens[6]);
	ebc1->trac_ctrl_override_switch = stoi(tokens[7]);
	ebc1->asr_hillholder_switch = stoi(tokens[8]);
	ebc1->abs_offroad_switch = stoi(tokens[9]);
	ebc1->asr_offroad_switch = stoi(tokens[10]);
	ebc1->accel_enable_switch = stoi(tokens[11]);
	ebc1->aux_eng_shutdown_switch = stoi(tokens[12]);
	ebc1->eng_derate_switch = stoi(tokens[13]);
	ebc1->accel_interlock_switch = stoi(tokens[14]);
	ebc1->eng_retarder_selection = stof(tokens[15]);
	ebc1->abs_ebs_amber_warning = stoi(tokens[16]);
	ebc1->ebs_red_warning = stoi(tokens[17]);
	ebc1->abs_fully_operational = stoi(tokens[18]);
 	ebc1->src_address_ctrl = stoi(tokens[19]);
 	ebc1->total_brk_demand = stof(tokens[20]);

	return (void*) ebc1;
}


int EBC2Interpreter::pgn() {
	return EBC2;
}


void *EBC2Interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_ebc2_typ *ebc2 = new j1939_ebc2_typ();
	ebc2->timestamp = pdu->timestamp;

	int two_bytes = TWOBYTES(pdu->data_field[1], pdu->data_field[0]);
	ebc2->front_axle_spd = wheel_based_mps(two_bytes);
	ebc2->rel_spd_front_left = wheel_based_mps_relative(pdu->data_field[2]);
	ebc2->rel_spd_front_right = wheel_based_mps_relative(pdu->data_field[3]);
	ebc2->rel_spd_rear_left_1 = wheel_based_mps_relative(pdu->data_field[4]);
	ebc2->rel_spd_rear_right_1 = wheel_based_mps_relative(pdu->data_field[5]);
	ebc2->rel_spd_rear_left_2 = wheel_based_mps_relative(pdu->data_field[6]);
	ebc2->rel_spd_rear_right_2 = wheel_based_mps_relative(pdu->data_field[7]);

	return (void*) ebc2;
}


void EBC2Interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_ebc2_typ *ebc2 = (j1939_ebc2_typ*) pdv;

	fprintf(fp, "EBC2");
	print_timestamp(fp, &ebc2->timestamp);
	if (numeric) {
		fprintf(fp, " %.3f", ebc2->front_axle_spd);
		fprintf(fp, " %.3f", ebc2->rel_spd_front_left);
		fprintf(fp, " %.3f", ebc2->rel_spd_front_right);
		fprintf(fp, " %.3f", ebc2->rel_spd_rear_left_1);
		fprintf(fp, " %.3f", ebc2->rel_spd_rear_right_1);
		fprintf(fp, " %.3f", ebc2->rel_spd_rear_left_2);
		fprintf(fp, " %.3f", ebc2->rel_spd_rear_right_2);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Front axle speed %.3f\n", ebc2->front_axle_spd);
		fprintf(fp, " Front left wheel relative speed %.3f\n",
				ebc2->rel_spd_front_left);
		fprintf(fp, " Front right wheel relative speed %.3f\n",
				ebc2->rel_spd_front_right);
		fprintf(fp, " Rear 1 left wheel relative speed %.3f\n",
				ebc2->rel_spd_rear_left_1);
		fprintf(fp, " Rear 1 left wheel relative speed %.3f\n",
				ebc2->rel_spd_rear_right_1);
		fprintf(fp, " Rear 2 left wheel relative speed %.3f\n",
				ebc2->rel_spd_rear_left_2);
		fprintf(fp, " Rear 2 left wheel relative speed %.3f\n",
				ebc2->rel_spd_rear_right_2);
	}
}


void *EBC2Interpreter::import(vector<string> &tokens) {
	j1939_ebc2_typ *ebc2 = new j1939_ebc2_typ();

	import_timestamp(&ebc2->timestamp, tokens[1]);
	ebc2->front_axle_spd = stof(tokens[2]);
	ebc2->rel_spd_front_left = stof(tokens[3]);
	ebc2->rel_spd_front_right = stof(tokens[4]);
	ebc2->rel_spd_rear_left_1 = stof(tokens[5]);
	ebc2->rel_spd_rear_right_1 = stof(tokens[6]);
	ebc2->rel_spd_rear_left_2 = stof(tokens[7]);
	ebc2->rel_spd_rear_right_2 = stof(tokens[8]);

	return (void*) ebc2;
}


int EEC1Interpreter::pgn() {
	return EEC1;
}


void *EEC1Interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_eec1_typ *eec1 = new j1939_eec1_typ();
	eec1->timestamp = pdu->timestamp;

	eec1->eng_trq_mode = LONIBBLE(pdu->data_field[0]);
	eec1->drvr_demand_eng_trq = percent_m125_to_p125(pdu->data_field[1]);
	eec1->actual_eng_trq = percent_m125_to_p125(pdu->data_field[2]);
	int two_bytes = TWOBYTES(pdu->data_field[4], pdu->data_field[3]);
	eec1->eng_spd = speed_in_rpm_2byte(two_bytes);
	eec1->src_address = pdu->data_field[5];
	eec1->eng_demand_trq = percent_m125_to_p125(pdu->data_field[7]);

	return (void*) eec1;
}


void EEC1Interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_eec1_typ *eec1 = (j1939_eec1_typ*) pdv;

	fprintf(fp, "EEC1");
	print_timestamp(fp, &eec1->timestamp);
	if (numeric) {
		fprintf(fp, " %d", eec1->eng_trq_mode);
		fprintf(fp, " %.2f", eec1->drvr_demand_eng_trq);
		fprintf(fp, " %.2f", eec1->actual_eng_trq);
		fprintf(fp, " %.2f", eec1->eng_demand_trq);
		fprintf(fp, " %.3f", eec1->eng_spd);
		fprintf(fp, " %d", eec1->src_address);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Engine retarder torque mode %d\n", eec1->eng_trq_mode);
		fprintf(fp, " Driver's demand percent torque %.2f\n",
				eec1->drvr_demand_eng_trq);
		fprintf(fp, " Actual engine percent torque %.2f\n",
				eec1->actual_eng_trq);
		fprintf(fp, " Engine Demand - Percent Torque %.2f\n",
				eec1->eng_demand_trq);
		fprintf(fp, " Engine speed (rpm) %.3f\n", eec1->eng_spd);
		fprintf(fp, " Source address engine control device %d\n",
				eec1->src_address);
	}
}


void *EEC1Interpreter::import(vector<string> &tokens) {
	j1939_eec1_typ *eec1 = new j1939_eec1_typ();

	if (tokens.size() == 8) {
		import_timestamp(&eec1->timestamp, tokens[1]);
		eec1->eng_trq_mode = stoi(tokens[2]);
		eec1->drvr_demand_eng_trq = stof(tokens[3]);
		eec1->actual_eng_trq = stof(tokens[4]);
		eec1->eng_demand_trq = stof(tokens[5]);
		eec1->eng_spd = stof(tokens[6]);
		eec1->src_address = stoi(tokens[7]);
	}
	else if (tokens.size() == 7) {
		import_timestamp(&eec1->timestamp, tokens[1]);
		eec1->eng_trq_mode = stoi(tokens[2]);
		eec1->drvr_demand_eng_trq = stof(tokens[3]);
		eec1->actual_eng_trq = stof(tokens[4]);
		eec1->eng_spd = stof(tokens[5]);
		eec1->src_address = stoi(tokens[6]);
	}

	return (void*) eec1;
}


int EEC2Interpreter::pgn() {
	return EEC2;
}


void *EEC2Interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_eec2_typ *eec2 = new j1939_eec2_typ();
	eec2->timestamp = pdu->timestamp;

	eec2->accel_pedal2_idle = BITS87(pdu->data_field[0]);
	eec2->spd_limit_status = BITS65(pdu->data_field[0]);
	eec2->accel_pedal_kickdown = BITS43(pdu->data_field[0]);
	eec2->accel_pedal1_idle = BITS21(pdu->data_field[0]);

	eec2->accel_pedal1_pos = percent_0_to_100(pdu->data_field[1]);
	eec2->eng_prcnt_load_curr_spd = percent_0_to_250(pdu->data_field[2]);
	eec2->accel_pedal2_pos = percent_0_to_100(pdu->data_field[4]);
	eec2->act_max_avail_eng_trq = percent_0_to_100(pdu->data_field[6]);

	return (void*) eec2;
}


void EEC2Interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_eec2_typ *eec2 = (j1939_eec2_typ*) pdv;

	fprintf(fp, "EEC2");
	print_timestamp(fp, &eec2->timestamp);
	if (numeric) {
		fprintf(fp, " %d", eec2->spd_limit_status);
		fprintf(fp, " %d", eec2->accel_pedal_kickdown);
		fprintf(fp, " %d", eec2->accel_pedal1_idle);
		fprintf(fp, " %d", eec2->accel_pedal2_idle);
		fprintf(fp, " %.2f", eec2->accel_pedal1_pos);
		fprintf(fp, " %.2f", eec2->accel_pedal2_pos);
		fprintf(fp, " %.2f", eec2->eng_prcnt_load_curr_spd);
		fprintf(fp, " %.2f", eec2->act_max_avail_eng_trq);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Road speed limit %d\n", eec2->spd_limit_status);
		fprintf(fp, " Kickpedal active %d\n", eec2->accel_pedal_kickdown);
		fprintf(fp, " Low idle 1 %d\n", eec2->accel_pedal1_idle);
		fprintf(fp, " Low idle 2 %d\n", eec2->accel_pedal2_idle);
		fprintf(fp, " AP1 position %.2f\n", eec2->accel_pedal1_pos);
		fprintf(fp, " AP2 position %.2f\n", eec2->accel_pedal2_pos);
		fprintf(fp, " Percent load %.2f\n", eec2->eng_prcnt_load_curr_spd);
		fprintf(fp, " Percent torque %.2f\n", eec2->act_max_avail_eng_trq);
	}
}


void *EEC2Interpreter::import(vector<string> &tokens) {
	j1939_eec2_typ *eec2 = new j1939_eec2_typ();

	if (tokens.size() == 10) {
		import_timestamp(&eec2->timestamp, tokens[1]);
		eec2->spd_limit_status = stoi(tokens[2]);
		eec2->accel_pedal_kickdown = stoi(tokens[3]);
		eec2->accel_pedal1_idle = stoi(tokens[4]);
		eec2->accel_pedal2_idle = stoi(tokens[5]);
		eec2->accel_pedal1_pos = stof(tokens[6]);
		eec2->accel_pedal2_pos = stof(tokens[7]);
		eec2->eng_prcnt_load_curr_spd = stof(tokens[8]);
		eec2->act_max_avail_eng_trq = stof(tokens[9]);
	}
	else if (tokens.size() == 8) {
		import_timestamp(&eec2->timestamp, tokens[1]);
		eec2->spd_limit_status = stoi(tokens[2]);
		eec2->accel_pedal_kickdown = stoi(tokens[3]);
		eec2->accel_pedal1_idle = stoi(tokens[4]);
		eec2->accel_pedal1_pos = stof(tokens[5]);
		eec2->eng_prcnt_load_curr_spd = stof(tokens[6]);
		eec2->act_max_avail_eng_trq = stof(tokens[7]);
	}

	return (void*) eec2;
}


int EEC3Interpreter::pgn() {
	return EEC3;
}


void *EEC3Interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_eec3_typ *eec3 = new j1939_eec3_typ();
	eec3->timestamp = pdu->timestamp;

	eec3->nominal_friction = percent_m125_to_p125(pdu->data_field[0]);

	int two_bytes = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	eec3->desired_operating_spd = 0.125 * two_bytes;

	eec3->operating_spd_adjust = percent_0_to_250(pdu->data_field[3]);

	eec3->est_eng_prstic_loss = percent_m125_to_p125(pdu->data_field[4]);

	return (void*) eec3;
}


void EEC3Interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_eec3_typ *eec3 = (j1939_eec3_typ*) pdv;

	fprintf(fp, "EEC3");
	print_timestamp(fp, &eec3->timestamp);
	if (numeric) {
		fprintf(fp," %.2f", eec3->nominal_friction);
		fprintf(fp," %.2f", eec3->est_eng_prstic_loss);
		fprintf(fp," %d", eec3->operating_spd_adjust);
		fprintf(fp," %.2f", eec3->desired_operating_spd);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp," Nominal friction percent torque %.2f\n",
			 eec3->nominal_friction);
		fprintf(fp," Estimated engine power loss - percent torque %.2f\n",
			 eec3->est_eng_prstic_loss);
		fprintf(fp," Desired Operating Speed Asymmetry Adjustment %d\n",
			 eec3->operating_spd_adjust);
		fprintf(fp," Engine desired operating speed %.2f\n",
			 eec3->desired_operating_spd);
	}
	// TODO(ak): add asymmetry adjustment
}


void *EEC3Interpreter::import(vector<string> &tokens) {
	j1939_eec3_typ *eec3 = new j1939_eec3_typ();

	if (tokens.size() == 6) {
		import_timestamp(&eec3->timestamp, tokens[1]);
		eec3->nominal_friction = stof(tokens[2]);
		eec3->est_eng_prstic_loss = stof(tokens[3]);
		eec3->operating_spd_adjust = stoi(tokens[4]);
		eec3->desired_operating_spd = stof(tokens[5]);
	}
	else if (tokens.size() == 5) {
		// for backwards compatibility
		import_timestamp(&eec3->timestamp, tokens[1]);
		eec3->nominal_friction = stof(tokens[2]);
		eec3->desired_operating_spd = stoi(tokens[3]);
		eec3->operating_spd_adjust = stof(tokens[4]);
	}

	return (void*) eec3;
}


int ERC1Interpreter::pgn() {
	return ERC1;
}


void *ERC1Interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_erc1_typ *erc1 = new j1939_erc1_typ();
	erc1->timestamp = pdu->timestamp;

	erc1->enable_shift_assist = BITS87(pdu->data_field[0]);
	erc1->enable_brake_assist = BITS65(pdu->data_field[0]);
	erc1->trq_mode = LONIBBLE(pdu->data_field[0]);

	erc1->actual_ret_pcnt_trq = percent_m125_to_p125(pdu->data_field[1]);
	erc1->intended_ret_pcnt_trq = percent_m125_to_p125(pdu->data_field[2]);
	erc1->rq_brake_light = BITS43(pdu->data_field[3]);
	erc1->src_address_ctrl = pdu->data_field[4];
	erc1->drvrs_demand_prcnt_trq = percent_m125_to_p125(pdu->data_field[5]);
	erc1->selection_nonengine = percent_0_to_100(pdu->data_field[6]);
	erc1->max_available_prcnt_trq = percent_m125_to_p125(pdu->data_field[7]);

	return (void*) erc1;
}


void ERC1Interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_erc1_typ *erc1 = (j1939_erc1_typ*) pdv;

	fprintf(fp, "ERC1");
	print_timestamp(fp, &erc1->timestamp);
	if (numeric) {
		fprintf(fp, " %d", erc1->enable_shift_assist);
		fprintf(fp, " %d", erc1->enable_brake_assist);
		fprintf(fp, " %d", erc1->trq_mode);
		fprintf(fp, " %.2f", erc1->actual_ret_pcnt_trq);
		fprintf(fp, " %.2f", erc1->intended_ret_pcnt_trq);
		fprintf(fp, " %d", erc1->rq_brake_light);
	 	fprintf(fp, " %d", erc1->src_address_ctrl);
		fprintf(fp, " %hhd", erc1->drvrs_demand_prcnt_trq);
		fprintf(fp, " %.2f", erc1->selection_nonengine);
		fprintf(fp, " %hhd", erc1->max_available_prcnt_trq);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Enable shift assist status %d\n",
				erc1->enable_shift_assist);
		fprintf(fp, " Enable brake assist status %d\n",
				erc1->enable_brake_assist);
		fprintf(fp, " Engine retarder torque mode %d\n", erc1->trq_mode);
		fprintf(fp, " Actual retarder percent torque %.2f\n",
				erc1->actual_ret_pcnt_trq);
		fprintf(fp, " Intended retarder percent torque %.2f\n",
				erc1->intended_ret_pcnt_trq);
		fprintf(fp, " Retarder requesting brake light %d\n",
				erc1->rq_brake_light);
		fprintf(fp, " Source address %d (0x%0x)\n",
				erc1->src_address_ctrl, erc1->src_address_ctrl);
		fprintf(fp, " Drivers demand retarder percent torque %d\n",
				erc1->drvrs_demand_prcnt_trq);
		fprintf(fp, " Retarder selection Non-eng %.2f\n",
				erc1->selection_nonengine);
		fprintf(fp, " Actual maximum available retarder percent torque %d\n",
				erc1->max_available_prcnt_trq);
	}
}


void *ERC1Interpreter::import(vector<string> &tokens) {
	j1939_erc1_typ *erc1 = new j1939_erc1_typ();

	if (tokens.size() == 12) {
		import_timestamp(&erc1->timestamp, tokens[1]);
		erc1->enable_shift_assist = stoi(tokens[2]);
		erc1->enable_brake_assist = stoi(tokens[3]);
		erc1->trq_mode = stoi(tokens[4]);
		erc1->actual_ret_pcnt_trq = stof(tokens[5]);
		erc1->intended_ret_pcnt_trq = stof(tokens[6]);
		erc1->rq_brake_light = stoi(tokens[7]);
	 	erc1->src_address_ctrl = stoi(tokens[8]);
		erc1->drvrs_demand_prcnt_trq = stoi(tokens[9]);
		erc1->selection_nonengine = stof(tokens[10]);
		erc1->max_available_prcnt_trq = stoi(tokens[11]);
	}
	else if (tokens.size() == 9) {
		import_timestamp(&erc1->timestamp, tokens[1]);
		erc1->enable_shift_assist = stoi(tokens[2]);
		erc1->enable_brake_assist = stoi(tokens[3]);
		erc1->trq_mode = stoi(tokens[4]);
		erc1->actual_ret_pcnt_trq = stof(tokens[5]);
		erc1->intended_ret_pcnt_trq = stof(tokens[6]);
		erc1->rq_brake_light = stoi(tokens[7]);
	 	erc1->src_address_ctrl = stoi(tokens[8]);
	}

	return (void*) erc1;
}


int ETC1Interpreter::pgn() {
	return ETC1;
}


void* ETC1Interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_etc1_typ *etc1 = new j1939_etc1_typ();
	etc1->timestamp = pdu->timestamp;
	int two_bytes;

	etc1->trans_shift = BITS65(pdu->data_field[0]);
	etc1->trq_conv_lockup = BITS43(pdu->data_field[0]);
	etc1->trans_driveline = BITS21(pdu->data_field[0]);

	two_bytes = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	etc1->tran_output_shaft_spd = speed_in_rpm_2byte(two_bytes);

	etc1->prcnt_clutch_slip = percent_0_to_100(pdu->data_field[3]);

	etc1->prog_shift_disable = BITS43(pdu->data_field[4]);
	etc1->eng_overspd_enable = BITS21(pdu->data_field[4]);

	two_bytes = TWOBYTES(pdu->data_field[6], pdu->data_field[5]);
	etc1->trans_input_shaft_spd = speed_in_rpm_2byte(two_bytes);

	etc1->src_address_ctrl = pdu->data_field[7];

	return (void*) etc1;
}


void ETC1Interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_etc1_typ *etc1 = (j1939_etc1_typ*) pdv;

	fprintf(fp, "ETC1");
	print_timestamp(fp, &etc1->timestamp);
	if (numeric) {
		fprintf(fp, " %d", etc1->trans_shift);
		fprintf(fp, " %d", etc1->trq_conv_lockup);
		fprintf(fp, " %d", etc1->trans_driveline);
		fprintf(fp, " %.2f", etc1->tran_output_shaft_spd);
		fprintf(fp, " %.2f", etc1->prcnt_clutch_slip);
		fprintf(fp, " %d", etc1->prog_shift_disable);
		fprintf(fp, " %d", etc1->eng_overspd_enable);
		fprintf(fp, " %.2f", etc1->trans_input_shaft_spd);
		fprintf(fp, " %d", etc1->src_address_ctrl);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Shift in progress %d\n", etc1->trans_shift);
		fprintf(fp, " Torque converter lockup engaged %d\n",
			 etc1->trq_conv_lockup);
		fprintf(fp, " Driveline engaged %d\n", etc1->trans_driveline);
		fprintf(fp, " Output shaft speed %.2f\n", etc1->tran_output_shaft_spd);
		fprintf(fp, " Percent clutch slip %.2f\n", etc1->prcnt_clutch_slip);
		fprintf(fp, " Progressive shift disable %d\n", etc1->prog_shift_disable);
		fprintf(fp, " Momentary engine overspeed enable %d\n",
			 etc1->eng_overspd_enable);
		fprintf(fp, " Input shaft speed %.2f\n", etc1->trans_input_shaft_spd);
		fprintf(fp, " Source address %d (0x%0x)\n",
			etc1->src_address_ctrl, etc1->src_address_ctrl);
	}
}


void *ETC1Interpreter::import(vector<string> &tokens) {
	j1939_etc1_typ *etc1 = new j1939_etc1_typ();

	import_timestamp(&etc1->timestamp, tokens[1]);
	etc1->trans_shift = stoi(tokens[2]);
	etc1->trq_conv_lockup = stoi(tokens[3]);
	etc1->trans_driveline = stoi(tokens[4]);
	etc1->tran_output_shaft_spd = stof(tokens[5]);
	etc1->prcnt_clutch_slip = stof(tokens[6]);
	etc1->prog_shift_disable = stoi(tokens[7]);
	etc1->eng_overspd_enable = stoi(tokens[8]);
	etc1->trans_input_shaft_spd = stof(tokens[9]);
	etc1->src_address_ctrl = stoi(tokens[10]);

	return (void*) etc1;
}


int ETC2Interpreter::pgn() {
	return ETC2;
}


void *ETC2Interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_etc2_typ *etc2 = new j1939_etc2_typ();
	etc2->timestamp = pdu->timestamp;

	etc2->trans_selected_gear = gear_m125_to_p125(pdu->data_field[0]);
	int two_bytes = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	etc2->trans_act_gear_ratio = gear_ratio(two_bytes);
	etc2->trans_current_gear = gear_m125_to_p125(pdu->data_field[3]);
	etc2->range_selected = TWOBYTES(pdu->data_field[5], pdu->data_field[4]);
	etc2->range_attained = TWOBYTES(pdu->data_field[7], pdu->data_field[6]);

	return (void*) etc2;
}


void ETC2Interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_etc2_typ *etc2 = (j1939_etc2_typ*) pdv;

	fprintf(fp, "ETC2");
	print_timestamp(fp, &etc2->timestamp);
	if (numeric){
		fprintf(fp, " %d", etc2->trans_selected_gear);
		fprintf(fp, " %.2f", etc2->trans_act_gear_ratio);
		fprintf(fp, " %d", etc2->trans_current_gear);
		fprintf(fp, " %d", etc2->range_selected);
		fprintf(fp, " %d", etc2->range_attained);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Selected gear %d\n", etc2->trans_selected_gear);
		fprintf(fp, " Actual gear ratio %.2f\n", etc2->trans_act_gear_ratio);
		fprintf(fp, " Current gear %d\n", etc2->trans_current_gear);
		fprintf(fp, " Trans. requested range %d\n", etc2->range_selected);
		fprintf(fp, " Trans. current range %d\n", etc2->range_attained);
	}
}


void *ETC2Interpreter::import(vector<string> &tokens) {
	j1939_etc2_typ *etc2 = new j1939_etc2_typ();

	import_timestamp(&etc2->timestamp, tokens[1]);
	etc2->trans_selected_gear = stoi(tokens[2]);
	etc2->trans_act_gear_ratio = stof(tokens[3]);
	etc2->trans_current_gear = stoi(tokens[4]);
	etc2->range_selected = stoi(tokens[5]);
	etc2->range_attained = stoi(tokens[6]);

	return (void*) etc2;
}


int TURBOInterpreter::pgn() {
	return TURBO;
}


void *TURBOInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_turbo_typ *turbo = new j1939_turbo_typ();
	turbo->timestamp = pdu->timestamp;

	turbo->turbo_lube_oil_pressure = pressure_0_to_1000kpa(pdu->data_field[0]);
	int two_bytes = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	turbo->turbo_speed = rotor_speed_in_rpm(two_bytes);

	return (void*) turbo;
}


void TURBOInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_turbo_typ *turbo = (j1939_turbo_typ*) pdv;

	fprintf(fp, "TURBO");
	print_timestamp(fp, &turbo->timestamp);
	if (numeric) {
		fprintf(fp, " %.2f", turbo->turbo_lube_oil_pressure);
		fprintf(fp, " %.2f", turbo->turbo_speed);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp," Turbocharger lube oil pressure %.2f\n",
			turbo->turbo_lube_oil_pressure);
		fprintf(fp," Turbocharger speed %.2f\n", turbo->turbo_speed);
	}
}


void *TURBOInterpreter::import(vector<string> &tokens) {
	j1939_turbo_typ *turbo = new j1939_turbo_typ();

	import_timestamp(&turbo->timestamp, tokens[1]);
	turbo->turbo_lube_oil_pressure = stof(tokens[2]);
	turbo->turbo_speed = stof(tokens[3]);

	return (void*) turbo;
}


int VDInterpreter::pgn() {
	return VD;
}


void *VDInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_vd_typ *vd = new j1939_vd_typ();
	vd->timestamp = pdu->timestamp;
	unsigned int four_bytes;

	four_bytes = FOURBYTES(pdu->data_field[3], pdu->data_field[2],
			pdu->data_field[2], pdu->data_field[0]);

	vd->trip_dist = distance_in_km(four_bytes);

	four_bytes = FOURBYTES(pdu->data_field[7], pdu->data_field[6],
			pdu->data_field[5], pdu->data_field[4]);
	vd->tot_vehicle_dist = distance_in_km(four_bytes);

	return (void*) vd;
}


void VDInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_vd_typ *vd = (j1939_vd_typ*) pdv;

	fprintf(fp, "VD");
	print_timestamp(fp, &vd->timestamp);
	if (numeric) {
		fprintf(fp," %.2f", vd->trip_dist);
		fprintf(fp," %.2f", vd->tot_vehicle_dist);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp," Trip distance (km) %.2f\n", vd->trip_dist);
		fprintf(fp," Total vehicle distance (km) %.2f\n", vd->tot_vehicle_dist);
	}
}


void *VDInterpreter::import(vector<string> &tokens) {
	j1939_vd_typ *vd = new j1939_vd_typ();

	import_timestamp(&vd->timestamp, tokens[1]);
	vd->trip_dist = stof(tokens[2]);
	vd->tot_vehicle_dist = stof(tokens[3]);

	return (void*) vd;
}


int RCFGInterpreter::pgn() {
	return RCFG;
}


void *RCFGInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_rcfg_typ *rcfg = new j1939_rcfg_typ();
	rcfg->timestamp = pdu->timestamp;
	unsigned short two_bytes;
	BYTE data[21];	 // to hold data bytes from 3 packets
	int i, j;

	for (i = 0; i < 3; i++) {
		int *p = &pdu[i].data_field[0];
		for (j = 0; j < 7; j++)
			data[i*7+j] = p[j+1];  // first byte is sequence no.
	}

	rcfg->retarder_loc = HINIBBLE(data[0]);
	rcfg->retarder_type = LONIBBLE(data[0]);
	rcfg->retarder_ctrl_steps = data[1];

	for (i = 0; i < 4; i++) {
		two_bytes = TWOBYTES(data[3*i+3], data[3*i+2]);
		rcfg->retarder_speed[i] = speed_in_rpm_2byte(two_bytes);
		rcfg->percent_torque[i] = percent_m125_to_p125(data[3*i + 1]);
	}

	two_bytes = TWOBYTES(data[15], data[14]);
	rcfg->retarder_speed[4] = speed_in_rpm_2byte(two_bytes);
	two_bytes = TWOBYTES(data[17], data[16]);
	rcfg->reference_retarder_trq = torque_in_nm(two_bytes);
	rcfg->percent_torque[4] = percent_m125_to_p125(data[18]);

	return (void*) rcfg;
}


void RCFGInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_rcfg_typ *rcfg = (j1939_rcfg_typ*) pdv;
	int i;

	fprintf(fp, "RCFG");
	print_timestamp(fp, &rcfg->timestamp);
	if (numeric) {
		fprintf(fp, " %d", rcfg->retarder_loc);
		fprintf(fp, " %d", rcfg->retarder_type);
		fprintf(fp, " %d", rcfg->retarder_ctrl_steps);
		for (i = 0; i < 5; i++)
			fprintf(fp, " %.2f", rcfg->retarder_speed[i]);
		for (i = 0; i < 5; i++)
			fprintf(fp, " %.2f", rcfg->percent_torque[i]);
		fprintf(fp, " %.2f", rcfg->reference_retarder_trq);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Retarder location 0x%x, type 0x%x, control %d\n",
				rcfg->retarder_loc, rcfg->retarder_type,
				rcfg->retarder_ctrl_steps);

		fprintf(fp, " Retarder speed");
		for (i = 0; i < 5; i++)
			fprintf(fp, " %.2f", rcfg->retarder_speed[i]);
		fprintf(fp, "\n");

		fprintf(fp, " Percent torque");
		for (i = 0; i < 5; i++)
			fprintf(fp, " %.2f", rcfg->percent_torque[i]);
		fprintf(fp, "\n");

		fprintf(fp, " Reference retarder torque %.2f\n",
				rcfg->reference_retarder_trq);
	}
}


void *RCFGInterpreter::import(vector<string> &tokens) {
	j1939_rcfg_typ *rcfg = new j1939_rcfg_typ();
	int i;

	if (tokens.size() == 16) {
		import_timestamp(&rcfg->timestamp, tokens[1]);
		rcfg->retarder_loc = stoi(tokens[2]);
		rcfg->retarder_type = stoi(tokens[3]);
		rcfg->retarder_ctrl_steps = stoi(tokens[4]);
		for (i = 0; i < 5; i++)
			rcfg->retarder_speed[i] = stof(tokens[5+i]);
		for (i = 0; i < 5; i++)
			rcfg->percent_torque[i] = stof(tokens[10+i]);
		rcfg->reference_retarder_trq = stof(tokens[15]);
	}
	else if (tokens.size() == 17) {
//		import_timestamp(&rcfg->timestamp, tokens[1]);
		rcfg->retarder_loc = stoi(tokens[3]);
		rcfg->retarder_type = stoi(tokens[4]);
		rcfg->retarder_ctrl_steps = stoi(tokens[5]);
		for (i = 0; i < 5; i++)
			rcfg->retarder_speed[i] = stof(tokens[6+i]);
		for (i = 0; i < 5; i++)
			rcfg->percent_torque[i] = stof(tokens[11+i]);
		rcfg->reference_retarder_trq = stof(tokens[16]);
	}

	return (void*) rcfg;
}


//void *TCFG_interpreter::convert(j1939_pdu_typ *pdu) {
//
//}
//
//
//void TCFG_interpreter::print(void *pdv, FILE *fp, bool numeric) {
//
//}
//
//
//void TCFG_interpreter::publish(void *pdv) {
//
//}


int ECFGInterpreter::pgn() {
	return ECFG;
}


void *ECFGInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_ecfg_typ *ecfg = new j1939_ecfg_typ();
	ecfg->timestamp = pdu->timestamp;
	int two_bytes;
	int data[28];	 // to hold data bytes from 4 packets
	int i, j;

	for (i = 0; i < 4; i++) {
		int *p = &pdu[i].data_field[0];
		for (j = 0; j < 7; j++)
			data[i*7+j] = p[j+1];  // first byte is sequence no.
	}

	for (i = 0; i < 5; i++){
		two_bytes = TWOBYTES(data[3*i+1], data[3*i]);
		ecfg->engine_spd[i] = speed_in_rpm_2byte(two_bytes);
		ecfg->percent_trq[i] = percent_m125_to_p125(data[3*i + 2]);
	}

	two_bytes = TWOBYTES(data[16], data[15]);
	ecfg->engine_spd[5] = speed_in_rpm_2byte(two_bytes);

	two_bytes = TWOBYTES(data[18], data[17]);
	ecfg->gain_endspeed_governor = gain_in_kp(two_bytes);

	two_bytes = TWOBYTES(data[20], data[19]);
	ecfg->reference_eng_trq = torque_in_nm(two_bytes);

	two_bytes = TWOBYTES(data[22], data[21]);
	ecfg->engine_spd[6] = speed_in_rpm_2byte(two_bytes);

	ecfg->max_momentary_overide_time = time_0_to_25sec(data[23]);
	ecfg->spd_ctrl_lower_lim = speed_in_rpm_1byte(data[24]);
	ecfg->spd_ctrl_upper_lim = speed_in_rpm_1byte(data[25]);
	ecfg->trq_ctrl_lower_lim = percent_m125_to_p125(data[26]);
	ecfg->trq_ctrl_upper_lim = percent_m125_to_p125(data[27]);

	return (void*) ecfg;
}


void ECFGInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_ecfg_typ *ecfg = (j1939_ecfg_typ*) pdv;
	int i;

	fprintf(fp, "ECFG");
	print_timestamp(fp, &ecfg->timestamp);
	if (numeric) {
		fprintf(fp, " 0x%x", ecfg->receive_status);
		for (i = 0; i < 7; i++)
			fprintf(fp, " %.2f", ecfg->engine_spd[i]);
		for (i = 0; i < 5; i++)
			fprintf(fp, " %.2f", ecfg->percent_trq[i]);
		fprintf(fp, " %.2f", ecfg->gain_endspeed_governor);
		fprintf(fp, " %.2f", ecfg->reference_eng_trq);
		fprintf(fp, " %.2f", ecfg->max_momentary_overide_time);
		fprintf(fp, " %.2f", ecfg->spd_ctrl_lower_lim);
		fprintf(fp, " %.2f", ecfg->spd_ctrl_upper_lim);
		fprintf(fp, " %.2f", ecfg->trq_ctrl_lower_lim);
		fprintf(fp, " %.2f", ecfg->trq_ctrl_upper_lim);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Engine configuration received mask 0x%x\n",
				ecfg->receive_status);
		fprintf(fp, " Engine speed");
		for (i = 0; i < 7; i++)
			fprintf(fp, " %.2f", ecfg->engine_spd[i]);
		fprintf(fp, "\n Percent torque");
		for (i = 0; i < 5; i++)
			fprintf(fp, " %.2f", ecfg->percent_trq[i]);
		fprintf(fp, "\n Gain endspeed governor %.2f\n",
				ecfg->gain_endspeed_governor);
		fprintf(fp, " Reference engine torque %.2f\n",
				ecfg->reference_eng_trq);
		fprintf(fp, " Max Momentary Override Time %.2f\n",
				ecfg->max_momentary_overide_time);
		fprintf(fp, " Speed Control Lower Limit %.2f\n",
				ecfg->spd_ctrl_lower_lim);
		fprintf(fp, " Speed Control Upper Limit %.2f\n",
				ecfg->spd_ctrl_upper_lim);
		fprintf(fp, " Torque Control Lower Limit %.2f\n",
				ecfg->trq_ctrl_lower_lim);
		fprintf(fp, " Torque Control Upper Limit %.2f\n",
				ecfg->trq_ctrl_upper_lim);
	}
}


void *ECFGInterpreter::import(vector<string> &tokens) {
	j1939_ecfg_typ *ecfg = new j1939_ecfg_typ();
	int i;

	import_timestamp(&ecfg->timestamp, tokens[1]);
	ecfg->receive_status = stoi(tokens[2]);
	for (i = 0; i < 7; i++)
		ecfg->engine_spd[i] = stof(tokens[3+i]);
	for (i = 0; i < 5; i++)
		ecfg->percent_trq[i] = stof(tokens[10+i]);
	ecfg->gain_endspeed_governor = stof(tokens[15]);
	ecfg->reference_eng_trq = stof(tokens[16]);
	ecfg->max_momentary_overide_time = stof(tokens[17]);
	ecfg->spd_ctrl_lower_lim = stof(tokens[18]);
	ecfg->spd_ctrl_upper_lim = stof(tokens[19]);
	ecfg->trq_ctrl_lower_lim = stof(tokens[20]);
	ecfg->trq_ctrl_upper_lim = stof(tokens[21]);

//	import_timestamp(&ecfg->timestamp, tokens[1]);
//	for (i = 0; i < 7; i++)
//		ecfg->engine_spd[i] = stof(tokens[3+i]);
//	for (i = 0; i < 5; i++)
//		ecfg->percent_trq[i] = stof(tokens[10+i]);
//	ecfg->gain_endspeed_governor = stof(tokens[15]);
//	ecfg->reference_eng_trq = stof(tokens[16]);
//	ecfg->max_momentary_overide_time = stof(tokens[17]);
//	ecfg->spd_ctrl_lower_lim = stof(tokens[18]);
//	ecfg->spd_ctrl_upper_lim = stof(tokens[19]);
//	ecfg->trq_ctrl_lower_lim = stof(tokens[20]);
//	ecfg->trq_ctrl_upper_lim = stof(tokens[21]);

	return (void*) ecfg;
}


int ETEMPInterpreter::pgn() {
	return ETEMP;
}


void *ETEMPInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_etemp_typ *etemp = new j1939_etemp_typ();
	etemp->timestamp = pdu->timestamp;
	unsigned short two_bytes;

	etemp->eng_coolant_temp = temp_m40_to_p210(pdu->data_field[0]);

	etemp->fuel_temp = temp_m40_to_p210(pdu->data_field[1]);

	two_bytes = TWOBYTES(pdu->data_field[3], pdu->data_field[2]);
	etemp->eng_oil_temp = temp_m273_to_p1735(two_bytes);

	two_bytes = TWOBYTES(pdu->data_field[5], pdu->data_field[4]);
	etemp->turbo_oil_temp = temp_m273_to_p1735(two_bytes);

	etemp->eng_intercooler_temp = temp_m40_to_p210(pdu->data_field[6]);

	etemp->eng_intercooler_thermostat_opening =
		percent_0_to_100(pdu->data_field[7]);

	return (void*) etemp;
}


void ETEMPInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_etemp_typ *etemp = (j1939_etemp_typ*) pdv;

	fprintf(fp, "ETEMP");
	print_timestamp(fp, &etemp->timestamp);
	if (numeric){
		fprintf(fp, " %.3f", etemp->eng_coolant_temp);
		fprintf(fp, " %.3f", etemp->fuel_temp);
		fprintf(fp, " %.3f", etemp->eng_oil_temp);
		fprintf(fp, " %.3f", etemp->turbo_oil_temp);
		fprintf(fp, " %.3f", etemp->eng_intercooler_temp);
		fprintf(fp, " %.3f", etemp->eng_intercooler_thermostat_opening);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Engine coolant temperature %.3f\n",
				etemp->eng_coolant_temp);
		fprintf(fp, " Fuel temperature %.3f\n", etemp->fuel_temp);
		fprintf(fp, " Engine oil temperature %.3f\n", etemp->eng_oil_temp);
		fprintf(fp, " Turbo oil temperature %.3f\n", etemp->turbo_oil_temp);
		fprintf(fp, " Engine intercooler temperature %.3f\n",
				etemp->eng_intercooler_temp);
		fprintf(fp, " Engine intercooler thermostat opening %.3f\n",
				etemp->eng_intercooler_thermostat_opening);
	}
}



void *ETEMPInterpreter::import(vector<string> &tokens) {
	j1939_etemp_typ *etemp = new j1939_etemp_typ();

	import_timestamp(&etemp->timestamp, tokens[1]);
	etemp->eng_coolant_temp = stof(tokens[2]);
	etemp->fuel_temp = stof(tokens[3]);
	etemp->eng_oil_temp = stof(tokens[4]);
	etemp->turbo_oil_temp = stof(tokens[5]);
	etemp->eng_intercooler_temp = stof(tokens[6]);
	etemp->eng_intercooler_thermostat_opening = stof(tokens[7]);

	return (void*) etemp;
}


int PTOInterpreter::pgn() {
	return PTO;
}


void *PTOInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_pto_typ *pto = new j1939_pto_typ();
	pto->timestamp = pdu->timestamp;
	int two_bytes;

	pto->oil_temp = temp_m40_to_p210(pdu->data_field[0]);

	two_bytes = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	pto->speed = speed_in_rpm_2byte(two_bytes);

	two_bytes = TWOBYTES(pdu->data_field[4], pdu->data_field[3]);
	pto->set_speed = speed_in_rpm_2byte(two_bytes);

	pto->remote_variable_spd_status = BITS65(pdu->data_field[5]);
	pto->remote_preprogramm_status = BITS43(pdu->data_field[5]);
	pto->enable_switch = BITS21(pdu->data_field[5]);

	pto->accel_switch = BITS87(pdu->data_field[6]);
	pto->resume_switch = BITS65(pdu->data_field[6]);
	pto->coast_decel_switch = BITS43(pdu->data_field[6]);
	pto->set_switch = BITS21(pdu->data_field[6]);

	return (void*) pto;
}


void PTOInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_pto_typ *pto = (j1939_pto_typ*) pdv;

	fprintf(fp, "PTO");
	print_timestamp(fp, &pto->timestamp);
	if (numeric) {
		fprintf(fp, " %.3f", pto->oil_temp);
		fprintf(fp, " %.3f", pto->speed);
		fprintf(fp, " %.3f", pto->set_speed);
		fprintf(fp, " %d", pto->remote_variable_spd_status);
		fprintf(fp, " %d", pto->remote_preprogramm_status);
		fprintf(fp, " %d", pto->enable_switch);
		fprintf(fp, " %d", pto->accel_switch);
		fprintf(fp, " %d", pto->resume_switch);
		fprintf(fp, " %d", pto->coast_decel_switch);
		fprintf(fp, " %d", pto->set_switch);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " PTO oil temperature %.3f\n", pto->oil_temp);
		fprintf(fp, " PTO speed %.3f\n", pto->speed);
		fprintf(fp, " PTO set speed %.3f\n", pto->set_speed);
		fprintf(fp, " Remote PTO variable speed control switch %d\n",
				pto->remote_variable_spd_status);
		fprintf(fp, " Remote PTO preprogrammed speed control switch %d\n",
				pto->remote_preprogramm_status);
		fprintf(fp, " PTO enable switch %d\n", pto->enable_switch);
		fprintf(fp, " PTO accelerate switch %d\n", pto->accel_switch);
		fprintf(fp, " PTO resume switch %d\n", pto->resume_switch);
		fprintf(fp, " PTO coast decelerate switch %d\n",
				pto->coast_decel_switch);
		fprintf(fp, " PTO set switch %d\n", pto->set_switch);
	}
}


void *PTOInterpreter::import(vector<string> &tokens) {
	j1939_pto_typ *pto = new j1939_pto_typ();

	import_timestamp(&pto->timestamp, tokens[1]);
	pto->oil_temp = stof(tokens[2]);
	pto->speed = stof(tokens[3]);
	pto->set_speed = stof(tokens[4]);
	pto->remote_variable_spd_status = stoi(tokens[5]);
	pto->remote_preprogramm_status = stoi(tokens[6]);
	pto->enable_switch = stoi(tokens[7]);
	pto->accel_switch = stoi(tokens[8]);
	pto->resume_switch = stoi(tokens[9]);
	pto->coast_decel_switch = stoi(tokens[10]);
	pto->set_switch = stoi(tokens[11]);

	return (void*) pto;
}


int CCVSInterpreter::pgn() {
	return CCVS;
}


void *CCVSInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_ccvs_typ *ccvs = new j1939_ccvs_typ();
	ccvs->timestamp = pdu->timestamp;
	int byte;
	int two_bytes;

	byte =  pdu->data_field[0];
	ccvs->park_brk_release = BITS87(byte);
	ccvs->cc_pause_switch = BITS65(byte);
	ccvs->parking_brk_switch = BITS43(byte);
	ccvs->two_spd_axle_switch = BITS21(byte);

	two_bytes = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	ccvs->vehicle_spd = wheel_based_mps(two_bytes);

	byte = pdu->data_field[3];
	ccvs->clutch_switch = BITS87(byte);
	ccvs->brk_switch = BITS65(byte);
	ccvs->cc_enable_switch = BITS43(byte);
	ccvs->cc_active = BITS21(byte);

	byte = pdu->data_field[4];
	ccvs->cc_accel_switch = BITS87(byte);
	ccvs->cc_resume_switch = BITS65(byte);
	ccvs->cc_coast_switch = BITS43(byte);
	ccvs->cc_set_switch = BITS21(byte);

	ccvs->cc_set_speed = cruise_control_set_meters_per_sec(pdu->data_field[5]);

	byte = pdu->data_field[6];
	ccvs->cc_state = HINIBBLE(byte) >> 1;
	ccvs->pto_state = byte & 0x1f;

	byte = pdu->data_field[7];
	ccvs->eng_shutdown_override = BITS87(byte);
	ccvs->eng_test_mode_switch = BITS65(byte);
	ccvs->eng_idle_decr_switch = BITS43(byte);
	ccvs->eng_idle_incr_switch = BITS21(byte);

	return (void*) ccvs;
}


void CCVSInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_ccvs_typ *ccvs = (j1939_ccvs_typ*) pdv;

	fprintf(fp, "CCVS");
	print_timestamp(fp, &ccvs->timestamp);
	if (numeric){
		fprintf(fp, " %d", ccvs->parking_brk_switch);
		fprintf(fp, " %d", ccvs->park_brk_release);
		fprintf(fp, " %d", ccvs->two_spd_axle_switch);
		fprintf(fp, " %.3f", ccvs->vehicle_spd);
		fprintf(fp, " %d", ccvs->clutch_switch);
		fprintf(fp, " %d", ccvs->brk_switch);
		fprintf(fp, " %d", ccvs->cc_pause_switch);
		fprintf(fp, " %d", ccvs->cc_enable_switch);
		fprintf(fp, " %d", ccvs->cc_active);
		fprintf(fp, " %d", ccvs->cc_accel_switch);
		fprintf(fp, " %d", ccvs->cc_resume_switch);
		fprintf(fp, " %d", ccvs->cc_coast_switch);
		fprintf(fp, " %d", ccvs->cc_set_switch);
		fprintf(fp, " %.3f", ccvs->cc_set_speed);
		fprintf(fp, " %d", ccvs->cc_state);
		fprintf(fp, " %d", ccvs->pto_state);
		fprintf(fp, " %d", ccvs->eng_shutdown_override);
		fprintf(fp, " %d", ccvs->eng_test_mode_switch);
		fprintf(fp, " %d", ccvs->eng_idle_decr_switch);
		fprintf(fp, " %d", ccvs->eng_idle_incr_switch);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Parking brake %d\n", ccvs->parking_brk_switch);
		fprintf(fp, " Parking brake inhibit %d\n", ccvs->park_brk_release);
		fprintf(fp, " Two speed axle switch %d\n", ccvs->two_spd_axle_switch);
		fprintf(fp, " Vehicle speed (meters/sec) %.3f\n", ccvs->vehicle_spd);
		fprintf(fp, " Clutch switch %d\n", ccvs->clutch_switch);
		fprintf(fp, " Brake switch %d\n", ccvs->brk_switch);
		fprintf(fp, " Cruise control pause %d\n", ccvs->cc_pause_switch);
		fprintf(fp, " Cruise control enable %d\n", ccvs->cc_enable_switch);
		fprintf(fp, " Cruise control active %d\n", ccvs->cc_active);
		fprintf(fp, " Cruise control accelerate %d\n", ccvs->cc_accel_switch);
		fprintf(fp, " Cruise control resume %d\n", ccvs->cc_resume_switch);
		fprintf(fp, " Cruise control coast %d\n", ccvs->cc_coast_switch);
		fprintf(fp, " Cruise control set %d\n", ccvs->cc_set_switch);
		fprintf(fp, " Cruise control set speed %.3f\n", ccvs->cc_set_speed);
		fprintf(fp, " Cruise control state %d\n", ccvs->cc_state);
		fprintf(fp, " PTO state %d\n", ccvs->pto_state);
		fprintf(fp, " Engine shutdown override %d\n",
				ccvs->eng_shutdown_override);
		fprintf(fp, " Engine test mode %d\n", ccvs->eng_test_mode_switch);
		fprintf(fp, " Idle decrement %d\n", ccvs->eng_idle_decr_switch);
		fprintf(fp, " Idle increment %d\n", ccvs->eng_idle_incr_switch);
	}
}


void *CCVSInterpreter::import(vector<string> &tokens) {
	j1939_ccvs_typ *ccvs = new j1939_ccvs_typ();

	if (tokens.size() == 22) {
		import_timestamp(&ccvs->timestamp, tokens[1]);
		ccvs->parking_brk_switch = stoi(tokens[2]);
		ccvs->park_brk_release = stoi(tokens[3]);
		ccvs->two_spd_axle_switch = stoi(tokens[4]);
		ccvs->vehicle_spd = stof(tokens[5]);
		ccvs->clutch_switch = stoi(tokens[6]);
		ccvs->brk_switch = stoi(tokens[7]);
		ccvs->cc_pause_switch = stoi(tokens[8]);
		ccvs->cc_enable_switch = stoi(tokens[9]);
		ccvs->cc_active = stoi(tokens[10]);
		ccvs->cc_accel_switch = stoi(tokens[11]);
		ccvs->cc_resume_switch = stoi(tokens[12]);
		ccvs->cc_coast_switch = stoi(tokens[13]);
		ccvs->cc_set_switch = stoi(tokens[14]);
		ccvs->cc_set_speed = stof(tokens[15]);
		ccvs->cc_state = stoi(tokens[16]);
		ccvs->pto_state = stoi(tokens[17]);
		ccvs->eng_shutdown_override = stoi(tokens[18]);
		ccvs->eng_test_mode_switch = stoi(tokens[19]);
		ccvs->eng_idle_decr_switch = stoi(tokens[20]);
		ccvs->eng_idle_incr_switch = stoi(tokens[21]);
	}
	else if (tokens.size() == 20) {
		import_timestamp(&ccvs->timestamp, tokens[1]);
		ccvs->park_brk_release = stoi(tokens[2]);
		ccvs->two_spd_axle_switch = stoi(tokens[3]);
		ccvs->vehicle_spd = stof(tokens[4]);
		ccvs->clutch_switch = stoi(tokens[5]);
		ccvs->brk_switch = stoi(tokens[6]);
		ccvs->cc_enable_switch = stoi(tokens[7]);
		ccvs->cc_active = stoi(tokens[8]);
		ccvs->cc_accel_switch = stoi(tokens[9]);
		ccvs->cc_resume_switch = stoi(tokens[10]);
		ccvs->cc_coast_switch = stoi(tokens[11]);
		ccvs->cc_set_switch = stoi(tokens[12]);
		ccvs->cc_set_speed = stof(tokens[13]);
		ccvs->cc_state = stoi(tokens[14]);
		ccvs->pto_state = stoi(tokens[15]);
		ccvs->eng_shutdown_override = stoi(tokens[16]);
		ccvs->eng_test_mode_switch = stoi(tokens[17]);
		ccvs->eng_idle_decr_switch = stoi(tokens[18]);
		ccvs->eng_idle_incr_switch = stoi(tokens[19]);
	}

	return (void*) ccvs;
}


int LFEInterpreter::pgn() {
	return LFE;
}


void *LFEInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_lfe_typ *lfe = new j1939_lfe_typ();
	lfe->timestamp = pdu->timestamp;
	int two_bytes;

	two_bytes = TWOBYTES(pdu->data_field[1], pdu->data_field[0]);
	lfe->eng_fuel_rate = fuel_rate_cm3_per_sec(two_bytes);

	two_bytes = TWOBYTES(pdu->data_field[3], pdu->data_field[2]);
	lfe->eng_inst_fuel_economy = fuel_economy_meters_per_cm3(two_bytes);

	two_bytes = TWOBYTES(pdu->data_field[5], pdu->data_field[4]);
	lfe->eng_avg_fuel_economy = fuel_economy_meters_per_cm3(two_bytes);

	lfe->eng_throttle1_pos = percent_0_to_100(pdu->data_field[6]);
	lfe->eng_throttle2_pos = percent_0_to_100(pdu->data_field[7]);

	return (void*) lfe;
}


void LFEInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_lfe_typ *lfe = (j1939_lfe_typ*) pdv;

	fprintf(fp, "LFE");
	print_timestamp(fp, &lfe->timestamp);
	if (numeric) {
		fprintf(fp, " %.3f", lfe->eng_fuel_rate);
		fprintf(fp, " %.3f", lfe->eng_inst_fuel_economy);
		fprintf(fp, " %.3f", lfe->eng_avg_fuel_economy);
		fprintf(fp, " %.3f", lfe->eng_throttle1_pos);
		fprintf(fp, " %.3f", lfe->eng_throttle2_pos);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Fuel rate (cm3/sec) %.3f\n", lfe->eng_fuel_rate);
		fprintf(fp, " Instantaneous fuel economy (m/cm3) %.3f\n",
				lfe->eng_inst_fuel_economy);
		fprintf(fp, " Average fuel economy (m/cm3) %.3f\n",
				lfe->eng_avg_fuel_economy);
		fprintf(fp, " Throttle 1 position (percent) %.3f\n",
				lfe->eng_throttle1_pos);
		fprintf(fp, " Throttle 2 position (percent) %.3f\n",
				lfe->eng_throttle2_pos);
	}
}


void *LFEInterpreter::import(vector<string> &tokens) {
	j1939_lfe_typ *lfe = new j1939_lfe_typ();

	if (tokens.size() == 7) {
		import_timestamp(&lfe->timestamp, tokens[1]);
		lfe->eng_fuel_rate = stof(tokens[2]);
		lfe->eng_inst_fuel_economy = stof(tokens[3]);
		lfe->eng_avg_fuel_economy = stof(tokens[4]);
		lfe->eng_throttle1_pos = stof(tokens[5]);
		lfe->eng_throttle2_pos = stof(tokens[6]);
	}
	else if (tokens.size() == 6) {
		import_timestamp(&lfe->timestamp, tokens[1]);
		lfe->eng_fuel_rate = stof(tokens[2]);
		lfe->eng_inst_fuel_economy = stof(tokens[3]);
		lfe->eng_avg_fuel_economy = stof(tokens[4]);
		lfe->eng_throttle1_pos = stof(tokens[5]);
	}

	return (void*) lfe;
}


int AMBCInterpreter::pgn() {
	return AMBC;
}

void *AMBCInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_ambc_typ *ambc = new j1939_ambc_typ();
	ambc->timestamp = pdu->timestamp;
	int two_bytes;

	ambc->barometric_pressure = pressure_0_to_125kpa(pdu->data_field[0]);

	two_bytes = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	ambc->cab_interior_temp = temp_m273_to_p1735(two_bytes);

	two_bytes = TWOBYTES(pdu->data_field[4], pdu->data_field[3]);
	ambc->ambient_air_temp = temp_m273_to_p1735(two_bytes);

	ambc->air_inlet_temp = temp_m40_to_p210(pdu->data_field[5]);

	two_bytes = TWOBYTES(pdu->data_field[7], pdu->data_field[6]);
	ambc->road_surface_temp = temp_m273_to_p1735(two_bytes);

	return (void*) ambc;
}


void AMBCInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_ambc_typ *ambc = (j1939_ambc_typ*) pdv;

	fprintf(fp, "AMBC");
	print_timestamp(fp, &ambc->timestamp);
	if (numeric) {
		fprintf(fp, " %.3f", ambc->barometric_pressure);
		fprintf(fp, " %.3f", ambc->cab_interior_temp);
		fprintf(fp, " %.3f", ambc->ambient_air_temp);
		fprintf(fp, " %.3f", ambc->air_inlet_temp);
		fprintf(fp, " %.3f", ambc->road_surface_temp);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Barometric pressure %.3f\n", ambc->barometric_pressure);
		fprintf(fp, " Cab interior temperature %.3f\n", ambc->cab_interior_temp);
		fprintf(fp, " Ambient air temperature %.3f\n", ambc->ambient_air_temp);
		fprintf(fp, " Air inlet temperature %.3f\n", ambc->air_inlet_temp);
		fprintf(fp, " Road surface temperature %.3f\n", ambc->road_surface_temp);
	}
}


void *AMBCInterpreter::import(vector<string> &tokens) {
	j1939_ambc_typ *ambc = new j1939_ambc_typ();

	import_timestamp(&ambc->timestamp, tokens[1]);
	ambc->barometric_pressure = stof(tokens[2]);
	ambc->cab_interior_temp = stof(tokens[3]);
	ambc->ambient_air_temp = stof(tokens[4]);
	ambc->air_inlet_temp = stof(tokens[5]);
	ambc->road_surface_temp = stof(tokens[6]);

	return (void*) ambc;
}


int IECInterpreter::pgn() {
	return IEC;
}


void *IECInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_iec_typ *iec = new j1939_iec_typ();
	iec->timestamp = pdu->timestamp;
	unsigned short two_bytes;

	iec->particulate_inlet_pressure = pressure_0_to_125kpa(pdu->data_field[0]);
	iec->boost_pressure = pressure_0_to_500kpa(pdu->data_field[1]);
	iec->intake_manifold_temp = temp_m40_to_p210(pdu->data_field[2]);
	iec->air_inlet_pressure = pressure_0_to_500kpa(pdu->data_field[3]);
	iec->air_filter_diff_pressure = pressure_0_to_12kpa(pdu->data_field[4]);

	two_bytes = TWOBYTES(pdu->data_field[6], pdu->data_field[5]);
	iec->exhaust_gas_temp = temp_m273_to_p1735(two_bytes);

	iec->coolant_filter_diff_pressure = pressure_0_to_125kpa(pdu->data_field[7]);

	return (void*) iec;
}


void IECInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_iec_typ *iec = (j1939_iec_typ*) pdv;

	fprintf(fp, "IEC");
	print_timestamp(fp, &iec->timestamp);
	if (numeric) {
		fprintf(fp, " %.3f", iec->particulate_inlet_pressure);
		fprintf(fp, " %.3f", iec->boost_pressure);
		fprintf(fp, " %.3f", iec->intake_manifold_temp);
		fprintf(fp, " %.3f", iec->air_inlet_pressure);
		fprintf(fp, " %.3f", iec->air_filter_diff_pressure);
		fprintf(fp, " %.3f", iec->exhaust_gas_temp);
		fprintf(fp, " %.3f", iec->coolant_filter_diff_pressure);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Particulate trap inlet pressure %.3f\n",
				iec->particulate_inlet_pressure);
		fprintf(fp, " Boost pressure %.3f\n", iec->boost_pressure);
		fprintf(fp, " Intake manifold temperature %.3f\n",
				iec->intake_manifold_temp);
		fprintf(fp, " Air inlet pressure %.3f\n", iec->air_inlet_pressure);
		fprintf(fp, " Air filter differential pressure %.3f\n",
				iec->air_filter_diff_pressure);
		fprintf(fp, " Exhaust gas temperature %.3f\n", iec->exhaust_gas_temp);
		fprintf(fp, " Coolant filter differential pressure %.3f\n",
				iec->coolant_filter_diff_pressure);
	}
}


void *IECInterpreter::import(vector<string> &tokens) {
	j1939_iec_typ *iec = new j1939_iec_typ();

	import_timestamp(&iec->timestamp, tokens[1]);
	iec->particulate_inlet_pressure = stof(tokens[2]);
	iec->boost_pressure = stof(tokens[3]);
	iec->intake_manifold_temp = stof(tokens[4]);
	iec->air_inlet_pressure = stof(tokens[5]);
	iec->air_filter_diff_pressure = stof(tokens[6]);
	iec->exhaust_gas_temp = stof(tokens[7]);
	iec->coolant_filter_diff_pressure = stof(tokens[8]);

	return (void*) iec;
}


int VEPInterpreter::pgn() {
	return VEP;
}


void *VEPInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_vep_typ *vep = new j1939_vep_typ();
	vep->timestamp = pdu->timestamp;
	int two_bytes;

	vep->net_battery_current = current_m125_to_p125amp(pdu->data_field[0]);
	vep->alternator_current = current_0_to_250amp(pdu->data_field[1]);

	two_bytes = TWOBYTES(pdu->data_field[3], pdu->data_field[2]);
	vep->alternator_potential = voltage(two_bytes);

	two_bytes = TWOBYTES(pdu->data_field[5], pdu->data_field[4]);
	vep->electrical_potential = voltage(two_bytes);

	two_bytes = TWOBYTES(pdu->data_field[7], pdu->data_field[6]);
	vep->battery_potential = voltage(two_bytes);

	return (void*) vep;
}


void VEPInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_vep_typ *vep = (j1939_vep_typ*) pdv;

	fprintf(fp, "VEP");
	print_timestamp(fp, &vep->timestamp);
	if (numeric) {
		fprintf(fp, " %.3f", vep->net_battery_current);
		fprintf(fp, " %.3f", vep->alternator_current);
		fprintf(fp, " %.3f", vep->alternator_potential);
		fprintf(fp, " %.3f", vep->electrical_potential);
		fprintf(fp, " %.3f", vep->battery_potential);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Net battery current %.3f\n", vep->net_battery_current);
		fprintf(fp, " Alternator current %.3f\n", vep->alternator_current);
		fprintf(fp, " Alternator potential %.3f\n", vep->alternator_potential);
		fprintf(fp, " Electrical potential %.3f\n", vep->electrical_potential);
		fprintf(fp, " Battery potential %.3f\n", vep->battery_potential);
	}
}


void *VEPInterpreter::import(vector<string> &tokens) {
	j1939_vep_typ *vep = new j1939_vep_typ();

	import_timestamp(&vep->timestamp, tokens[1]);
	vep->net_battery_current = stof(tokens[2]);
	vep->alternator_current = stof(tokens[3]);
	vep->alternator_potential = stof(tokens[4]);
	vep->electrical_potential = stof(tokens[5]);
	vep->battery_potential = stof(tokens[6]);

	return (void*) vep;
}


int TFInterpreter::pgn() {
	return TF;
}


void *TFInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_tf_typ *tf = new j1939_tf_typ();
	tf->timestamp = pdu->timestamp;
	unsigned short two_bytes;

	tf->clutch_pressure = pressure_0_to_4000kpa(pdu->data_field[0]);
	tf->oil_level = percent_0_to_100(pdu->data_field[1]);
	tf->diff_pressure = pressure_0_to_500kpa(pdu->data_field[2]);
	tf->oil_pressure = pressure_0_to_4000kpa(pdu->data_field[3]);

	two_bytes = TWOBYTES(pdu->data_field[5], pdu->data_field[4]);
	tf->oil_temp = temp_m273_to_p1735(two_bytes);

	return (void*) tf;
}


void TFInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_tf_typ *tf = (j1939_tf_typ*) pdv;

	fprintf(fp, "TF");
	print_timestamp(fp, &tf->timestamp);
	if (numeric) {
		fprintf(fp, " %.3f", tf->clutch_pressure);
		fprintf(fp, " %.3f", tf->oil_level);
		fprintf(fp, " %.3f", tf->diff_pressure);
		fprintf(fp, " %.3f", tf->oil_pressure);
		fprintf(fp, " %.3f", tf->oil_temp);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Clutch pressure %.3f\n", tf->clutch_pressure);
		fprintf(fp, " Transmission oil level %.3f\n", tf->oil_level);
		fprintf(fp, " Filter differential pressure %.3f\n", tf->diff_pressure);
		fprintf(fp, " Transmission oil pressure %.3f\n", tf->oil_pressure);
		fprintf(fp, " Transmission oil temperature %.3f\n", tf->oil_temp);
	}
}


void *TFInterpreter::import(vector<string> &tokens) {
	j1939_tf_typ *tf = new j1939_tf_typ();

	import_timestamp(&tf->timestamp, tokens[1]);
	tf->clutch_pressure = stof(tokens[2]);
	tf->oil_level = stof(tokens[3]);
	tf->diff_pressure = stof(tokens[4]);
	tf->oil_pressure = stof(tokens[5]);
	tf->oil_temp = stof(tokens[6]);

	return (void*) tf;
}


int RFInterpreter::pgn() {
	return RF;
}


void *RFInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_rf_typ *rf = new j1939_rf_typ();
	rf->timestamp = pdu->timestamp;
	rf->pressure = pressure_0_to_4000kpa(pdu->data_field[0]);
	rf->oil_temp = temp_m40_to_p210(pdu->data_field[1]);
	return (void*) rf;
}


void RFInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_rf_typ *rf = (j1939_rf_typ*) pdv;

	fprintf(fp, "RF");
	print_timestamp(fp, &rf->timestamp);
	if (numeric) {
		fprintf(fp, " %.3f", rf->pressure);
		fprintf(fp, " %.3f", rf->oil_temp);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Hydraulic retarder pressure %.3f\n", rf->pressure);
		fprintf(fp, " Hydraulic retarder temperature %.3f\n", rf->oil_temp);
	}
}


void *RFInterpreter::import(vector<string> &tokens) {
	j1939_rf_typ *rf = new j1939_rf_typ();

	import_timestamp(&rf->timestamp, tokens[1]);
	rf->pressure = stof(tokens[2]);
	rf->oil_temp = stof(tokens[3]);

	return (void*) rf;
}


int HRVDInterpreter::pgn() {
	return HRVD;
}


void *HRVDInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_hrvd_typ *hrvd = new j1939_hrvd_typ();
	hrvd->timestamp = pdu->timestamp;
	unsigned int four_bytes;

	four_bytes = FOURBYTES(pdu->data_field[3], pdu->data_field[2],
		pdu->data_field[2], pdu->data_field[0]);
	hrvd->vehicle_distance = hr_distance_in_km(four_bytes);

	four_bytes = FOURBYTES(pdu->data_field[7], pdu->data_field[6],
		pdu->data_field[5], pdu->data_field[4]);
	hrvd->trip_distance = hr_distance_in_km(four_bytes);

	return (void*) hrvd;
}


void HRVDInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_hrvd_typ *hrvd = (j1939_hrvd_typ*) pdv;

	fprintf(fp, "HRVD");
	print_timestamp(fp, &hrvd->timestamp);
	if (numeric) {
		fprintf(fp, " %.3f", hrvd->vehicle_distance);
		fprintf(fp, " %.3f", hrvd->trip_distance);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Vehicle distance %.3f\n", hrvd->vehicle_distance);
		fprintf(fp, " Trip distance %.3f\n", hrvd->trip_distance);
	}
}


void *HRVDInterpreter::import(vector<string> &tokens) {
	j1939_hrvd_typ *hrvd = new j1939_hrvd_typ();

	import_timestamp(&hrvd->timestamp, tokens[1]);
	hrvd->vehicle_distance = stof(tokens[2]);
	hrvd->trip_distance = stof(tokens[3]);

	return (void*) hrvd;
}


int FDInterpreter::pgn() {
	return FD;
}


void *FDInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_fd_typ *fd = new j1939_fd_typ();
	fd->timestamp = pdu->timestamp;
	fd->prcnt_fan_spd = percent_0_to_100(pdu->data_field[0]);
	fd->fan_drive_state = LONIBBLE(pdu->data_field[1]);
	return (void*) fd;
}


void FDInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_fd_typ *fd = (j1939_fd_typ*) pdv;

	fprintf(fp, "FD");
	print_timestamp(fp, &fd->timestamp);
	if (numeric) {
		fprintf(fp, " %.3f", fd->prcnt_fan_spd);
		fprintf(fp, " %d", fd->fan_drive_state);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Estimated percent fan speed %.3f\n", fd->prcnt_fan_spd);
		fprintf(fp, " Fan drive state %d\n", fd->fan_drive_state);
	}
}


void *FDInterpreter::import(vector<string> &tokens) {
	j1939_fd_typ *fd = new j1939_fd_typ();

	import_timestamp(&fd->timestamp, tokens[1]);
	fd->prcnt_fan_spd = stof(tokens[2]);
	fd->fan_drive_state = stoi(tokens[3]);

	return (void*) fd;
}


int GFI2Interpreter::pgn() {
	return GFI2;
}


void *GFI2Interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_gfi2_typ *gfi2 = new j1939_gfi2_typ();
	gfi2->timestamp = pdu->timestamp;
	int two_bytes;

	two_bytes = TWOBYTES(pdu->data_field[1], pdu->data_field[0]);
	gfi2->fuel_flow_rate1 = two_bytes * 0.1;

	two_bytes = TWOBYTES(pdu->data_field[3], pdu->data_field[2]);
	gfi2->fuel_flow_rate2 = two_bytes * 0.1;

	gfi2->fuel_valve_pos1 = percent_0_to_100(pdu->data_field[4]);
	gfi2->fuel_valve_pos2 = percent_0_to_100(pdu->data_field[5]);

	return (void*) gfi2;
}


void GFI2Interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_gfi2_typ *gfi2 = (j1939_gfi2_typ*) pdv;

	fprintf(fp, "GFI2");
	print_timestamp(fp, &gfi2->timestamp);
	if (numeric) {
		fprintf(fp, " %.3f", gfi2->fuel_flow_rate1);
		fprintf(fp, " %.3f", gfi2->fuel_flow_rate2);
		fprintf(fp, " %.3f", gfi2->fuel_valve_pos1);
		fprintf(fp, " %.3f", gfi2->fuel_valve_pos2);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Fuel flow rate 1 %.3f\n", gfi2->fuel_flow_rate1);
		fprintf(fp, " Fuel flow rate 2 %.3f\n", gfi2->fuel_flow_rate2);
		fprintf(fp, " Fuel valve 1 position %.3f\n", gfi2->fuel_valve_pos1);
		fprintf(fp, " Fuel valve 2 position %.3f\n", gfi2->fuel_valve_pos2);
	}
}


void *GFI2Interpreter::import(vector<string> &tokens) {
	j1939_gfi2_typ *gfi2 = new j1939_gfi2_typ();

	import_timestamp(&gfi2->timestamp, tokens[1]);
	gfi2->fuel_flow_rate1 = stof(tokens[2]);
	gfi2->fuel_flow_rate2 = stof(tokens[3]);
	gfi2->fuel_valve_pos1 = stof(tokens[4]);
	gfi2->fuel_valve_pos2 = stof(tokens[5]);

	return (void*) gfi2;
}


int EIInterpreter::pgn() {
	return EI;
}

void *EIInterpreter::convert(j1939_pdu_typ *pdu) {
	j1939_ei_typ *ei = new j1939_ei_typ();
	ei->timestamp = pdu->timestamp;
	int data;

	ei->pre_filter_oil_pressure = pressure_0_to_1000kpa(pdu->data_field[0]);
	data = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	ei->exhaust_gas_pressure = pressure_m250_to_p252kpa(data);
	ei->rack_position = percent_0_to_100(pdu->data_field[3]);
	data = TWOBYTES(pdu->data_field[5], pdu->data_field[4]);
	ei->eng_gas_mass_flow = mass_flow(data);
	data = TWOBYTES(pdu->data_field[7], pdu->data_field[6]);
	ei->inst_estimated_brake_power = power_in_kw(data);

	return (void*) ei;
}


void EIInterpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_ei_typ *ei = (j1939_ei_typ*) pdv;

	fprintf(fp, "EI");
	print_timestamp(fp, &ei->timestamp);
	if (numeric) {
		fprintf(fp, " %.3f", ei->pre_filter_oil_pressure);
		fprintf(fp, " %.3f", ei->exhaust_gas_pressure);
		fprintf(fp, " %.3f", ei->rack_position);
		fprintf(fp, " %.3f", ei->eng_gas_mass_flow);
		fprintf(fp, " %.3f", ei->inst_estimated_brake_power);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Pre-filter oil pressure %.3f\n",
				ei->pre_filter_oil_pressure);
		fprintf(fp, " Exhaust gas pressure %.3f\n", ei->exhaust_gas_pressure);
		fprintf(fp, " Rack position %.3f\n", ei->rack_position);
		fprintf(fp, " Natural gas mass flow %.3f\n", ei->eng_gas_mass_flow);
		fprintf(fp, " Instantaneous estimated brake power %.3f\n",
				ei->inst_estimated_brake_power);
	}
}


void *EIInterpreter::import(vector<string> &tokens) {
	j1939_ei_typ *ei = new j1939_ei_typ();

	import_timestamp(&ei->timestamp, tokens[1]);
	ei->pre_filter_oil_pressure = stof(tokens[2]);
	ei->exhaust_gas_pressure = stof(tokens[3]);
	ei->rack_position = stof(tokens[4]);
	ei->eng_gas_mass_flow = stof(tokens[5]);
	ei->inst_estimated_brake_power = stof(tokens[6]);

	return (void*) ei;
}


/* -------------------------------------------------------------------------- */
/* ----------------- To facilitate collecting interpreters ------------------ */
/* -------------------------------------------------------------------------- */


map<int, J1939Interpreter*> get_interpreters() {
    map<int, J1939Interpreter*> interpreters;
    interpreters.insert(make_pair(PDU, new PDUInterpreter()));
    interpreters.insert(make_pair(TSC1, new TSC1Interpreter()));
    interpreters.insert(make_pair(ERC1, new ERC1Interpreter()));
    interpreters.insert(make_pair(EBC1, new EBC1Interpreter()));
    interpreters.insert(make_pair(EBC2, new EBC2Interpreter()));
    interpreters.insert(make_pair(ETC1, new ETC1Interpreter()));
    interpreters.insert(make_pair(ETC2, new ETC2Interpreter()));
    interpreters.insert(make_pair(EEC1, new EEC1Interpreter()));
    interpreters.insert(make_pair(EEC2, new EEC2Interpreter()));
    interpreters.insert(make_pair(EEC3, new EEC3Interpreter()));
    interpreters.insert(make_pair(GFI2, new GFI2Interpreter()));
    interpreters.insert(make_pair(EI, new EIInterpreter()));
    interpreters.insert(make_pair(FD, new FDInterpreter()));
    interpreters.insert(make_pair(HRVD, new HRVDInterpreter()));
    interpreters.insert(make_pair(TURBO, new TURBOInterpreter()));
    interpreters.insert(make_pair(VD, new VDInterpreter()));
    interpreters.insert(make_pair(RCFG, new RCFGInterpreter()));
    interpreters.insert(make_pair(ECFG, new ECFGInterpreter()));
    interpreters.insert(make_pair(ETEMP, new ETEMPInterpreter()));
    interpreters.insert(make_pair(PTO, new PTOInterpreter()));
    interpreters.insert(make_pair(CCVS, new CCVSInterpreter()));
    interpreters.insert(make_pair(LFE, new LFEInterpreter()));
    interpreters.insert(make_pair(AMBC, new AMBCInterpreter()));
    interpreters.insert(make_pair(IEC, new IECInterpreter()));
    interpreters.insert(make_pair(VEP, new VEPInterpreter()));
    interpreters.insert(make_pair(TF, new TFInterpreter()));
    interpreters.insert(make_pair(RF, new RFInterpreter()));

    return interpreters;
}
