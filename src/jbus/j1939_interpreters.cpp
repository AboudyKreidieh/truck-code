/**
 * j1939_interpreters.cpp
 *
 *  Created on: May 31, 2018
 *      Author: Abdul Rahman Kreidieh
 */

#include "j1939_interpreters.h"


bool j1939_interpreter::is_type(j1939_pdu_typ *pdu) {
	int target_pgn = (int)pow(2.0,8.0) * pdu->pdu_format + pdu->pdu_specific;
    return (target_pgn == pgn);
}


void *TSC1_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_tsc1_typ *tsc1 = new j1939_tsc1_typ();
	tsc1->timestamp = pdu->timestamp;
	short data;

	tsc1->src_address = pdu->src_address;
	tsc1->destination_address = pdu->pdu_specific;

    tsc1->ovrd_ctrl_m = BITS21(pdu->data_field[0]);
	tsc1->req_spd_ctrl = BITS43(pdu->data_field[0]);
	tsc1->ovrd_ctrl_m_pr = BITS65(pdu->data_field[0]);

	data = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	tsc1->req_spd_lim = speed_in_rpm_2byte(data);

	tsc1->req_trq_lim = percent_m125_to_p125(pdu->data_field[3]);

	return (void*) tsc1;
}


void TSC1_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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
		fprintf(fp, " Destination %d \n", tsc1->destination_address);
		fprintf(fp, " Source address %d\n", tsc1->src_address);
		fprintf(fp, " Override Control Mode priority %d\n",
				tsc1->ovrd_ctrl_m_pr);
		fprintf(fp, " Requested speed control conditions %d\n",
				tsc1->req_spd_ctrl);
		fprintf(fp, " Override control mode %d\n", tsc1->ovrd_ctrl_m);
		fprintf(fp, " Requested speed/speed limit  %.3f\n", tsc1->req_spd_lim);
		fprintf(fp, " Requested torque/torque limit %.3f\n", tsc1->req_trq_lim);
	}
}


void TSC1_interpreter::publish(void *pdv) {
	// j1939_tsc1_typ *tsc1 = (j1939_tsc1_typ*) pdv;
}


void *ERC1_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_erc1_typ *erc1 = new j1939_erc1_typ();
	erc1->timestamp = pdu->timestamp;

	// two-bit fields in data byte 0 indicate status of switch, enabled or
	// disabled.
	unsigned char byte = (unsigned int) pdu->data_field[0];
	erc1->enable_shift_assist = BITS87(byte);
	erc1->enable_brake_assist = BITS65(byte);
	erc1->trq_mode = LONIBBLE(byte);

	erc1->actual_ret_pcnt_trq = percent_m125_to_p125(pdu->data_field[1]);
	erc1->intended_ret_pcnt_trq = percent_m125_to_p125(pdu->data_field[2]);
	erc1->rq_brake_light = BITS43(pdu->data_field[3]);
	erc1->src_address_ctrl = pdu->data_field[4];
	erc1->drvrs_demand_prcnt_trq = pdu->data_field[5];
	erc1->selection_nonengine = pdu->data_field[6];
	erc1->max_available_prcnt_trq = pdu->data_field[7];

	return (void*) erc1;
}


void ERC1_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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
		fprintf(fp, " %d", erc1->selection_nonengine);
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
		fprintf(fp, " Retarder selection Non-eng %d\n",
				erc1->selection_nonengine);
		fprintf(fp, " Actual maximum available retarder percent torque %d\n",
				erc1->max_available_prcnt_trq);
	}
}


void ERC1_interpreter::publish(void *pdv) {
	j1939_erc1_typ *erc1 = (j1939_erc1_typ*) pdv;
}


// FIXME(ak): ASR offroad not covered here.
void *EBC1_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_ebc1_typ *ebc1 = new j1939_ebc1_typ();
	ebc1->timestamp = pdu->timestamp;
	unsigned char byte;

	byte = (unsigned int) pdu->data_field[0];
	ebc1->ebs_brk_switch = BITS87(byte);
	ebc1->antilock_brk_active = BITS65(byte);
	ebc1->asr_brk_ctrl_active = BITS43(byte);
	ebc1->asr_engine_ctrl_active = BITS21(byte);

	ebc1->brk_pedal_pos = percent_0_to_100(pdu->data_field[1]);

	byte = (unsigned int) pdu->data_field[2];
	ebc1->trac_ctrl_override_switch = BITS87(byte);
	ebc1->asr_hillholder_switch = BITS65(byte);

	byte = (unsigned int) pdu->data_field[3];
	ebc1->accel_enable_switch = BITS87(byte);
	ebc1->aux_eng_shutdown_switch = BITS65(byte);
	ebc1->eng_derate_switch = BITS43(byte);
	ebc1->accel_interlock_switch = BITS21(byte);

	ebc1->eng_retarder_selection = percent_0_to_100(pdu->data_field[4]);

	byte = (unsigned int) pdu->data_field[5];
	ebc1->abs_ebs_amber_warning = BITS65(byte);
	ebc1->ebs_red_warning = BITS43(byte);
	ebc1->abs_fully_operational = BITS21(byte);

	ebc1->abs_ebs_amber_warning = pdu->data_field[6];
	// FIXME(ak): where is the command for this?
	ebc1->total_brk_demand = brake_demand(pdu->data_field[7]);

	return (void*) ebc1;
}


void EBC1_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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
		fprintf(fp, " EBS brake switch status %d \n", ebc1->ebs_brk_switch);
		fprintf(fp, " ABS active status %d \n", ebc1->antilock_brk_active);
		fprintf(fp, " ASR brake control status%d \n",
				ebc1->asr_brk_ctrl_active);
		fprintf(fp, " ASR engine control active status %d \n",
				ebc1->asr_engine_ctrl_active);
		fprintf(fp, " Brake pedal position %.2f\n ", ebc1->brk_pedal_pos);
		fprintf(fp, " Traction control override switch status %d \n",
				ebc1->trac_ctrl_override_switch);
		fprintf(fp, " Hill holder switch status %d \n",
				ebc1->asr_hillholder_switch);
		fprintf(fp, " ASR off road switch status %d \n",
				ebc1->asr_offroad_switch);
		fprintf(fp, " Remote accelerator enable switch status %d \n",
				ebc1->accel_enable_switch);
		fprintf(fp, " Auxiliary engine shutdown switch status %d \n",
				ebc1->aux_eng_shutdown_switch);
		fprintf(fp, " Engine derate switch status %d \n",
				ebc1->eng_derate_switch);
		fprintf(fp, " Accelerator interlock switch status %d \n",
				ebc1->accel_interlock_switch);
		fprintf(fp, " Percent engine retarder torque selected %.2f \n",
				ebc1->eng_retarder_selection);
		fprintf(fp, " ABS/EBS amber warning state %d \n",
				ebc1->abs_ebs_amber_warning);
		fprintf(fp, " EBS red warning state %d \n", ebc1->ebs_red_warning);
		fprintf(fp, " ABS fully operational %d \n",
				ebc1->abs_fully_operational);
		fprintf(fp, " Source address %d (0x%0x)\n",
				ebc1->src_address_ctrl, ebc1->src_address_ctrl);
		fprintf(fp, " Total brake demand %.3f\n", ebc1->total_brk_demand);
	}
}


void EBC1_interpreter::publish(void *pdv) {
	j1939_ebc1_typ *ebc1 = (j1939_ebc1_typ*) pdv;
}


void* ETC1_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_etc1_typ *etc1 = new j1939_etc1_typ();
	etc1->timestamp = pdu->timestamp;
	unsigned char byte;
	unsigned short two_bytes;

	// two-bit fields in data byte 0 indicate status of switch, enabled or
	// disabled.
	byte = (unsigned int) pdu->data_field[0];
	etc1->trans_shift = BITS65(byte);
	etc1->trq_conv_lockup = BITS43(byte);
	etc1->trans_driveline = BITS21(byte);

	two_bytes = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	etc1->tran_output_shaft_spd = speed_in_rpm_2byte(two_bytes);

	etc1->prcnt_clutch_slip = percent_0_to_100(pdu->data_field[3]);

	byte = pdu->data_field[4];
	etc1->prog_shift_disable = BITS43(byte);
	etc1->eng_overspd_enable = BITS21(byte);

	two_bytes = TWOBYTES(pdu->data_field[6], pdu->data_field[5]);
	etc1->trans_input_shaft_spd = speed_in_rpm_2byte(two_bytes);

	etc1->src_address_ctrl = pdu->data_field[7];

	return (void*) etc1;
}


void ETC1_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void ETC1_interpreter::publish(void *pdv) {
	j1939_etc1_typ *etc1 = (j1939_etc1_typ*) pdv;
}


void *EEC1_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_eec1_typ *eec1 = new j1939_eec1_typ();
	eec1->timestamp = pdu->timestamp;
	unsigned short two_bytes;

	eec1->eng_trq_mode = LONIBBLE(pdu->data_field[0]);
	eec1->drvr_demand_eng_trq = percent_m125_to_p125(pdu->data_field[1]);
	eec1->actual_eng_trq = percent_m125_to_p125(pdu->data_field[2]);
	eec1->eng_demand_trq = percent_m125_to_p125(pdu->data_field[7]);
	two_bytes = TWOBYTES(pdu->data_field[4], pdu->data_field[3]);
	eec1->eng_spd = speed_in_rpm_2byte(two_bytes);
	eec1->src_address = pdu->data_field[5];

	return (void*) eec1;
}


void EEC1_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void EEC1_interpreter::publish(void *pdv) {
	j1939_eec1_typ *eec1 = (j1939_eec1_typ*) pdv;
}


void *EEC2_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_eec2_typ *eec2 = new j1939_eec2_typ();
	eec2->timestamp = pdu->timestamp;
	unsigned char byte;

	// two-bit fields in data byte 0 indicate active/inactive: 00 active,
	// 01 not active for road speed limit, see p. 140
	byte = (unsigned int) pdu->data_field[0];
	eec2->accel_pedal2_idle = BITS87(byte);
	eec2->spd_limit_status = BITS65(byte);
	eec2->accel_pedal_kickdown = BITS43(byte);
	eec2->accel_pedal1_idle = BITS21(byte);

	eec2->accel_pedal1_pos = percent_0_to_100(pdu->data_field[1]);
	eec2->accel_pedal2_pos = percent_0_to_100(pdu->data_field[4]);
	eec2->act_max_avail_eng_trq = percent_0_to_100(pdu->data_field[6]);
	eec2->eng_prcnt_load_curr_spd = percent_0_to_250(pdu->data_field[2]);

	return (void*) eec2;
}


void EEC2_interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_eec2_typ *eec2 = (j1939_eec2_typ*) pdv;

	fprintf(fp, "EEC2");
	print_timestamp(fp, &eec2->timestamp);
	if (numeric) {
		fprintf(fp, " %d", eec2->spd_limit_status);
		fprintf(fp, " %d", eec2->accel_pedal_kickdown);
		fprintf(fp, " %d", eec2->accel_pedal1_idle);
		fprintf(fp, " %d", eec2->accel_pedal2_idle);
		fprintf(fp, " %.2f", eec2->act_max_avail_eng_trq);
		fprintf(fp, " %.2f", eec2->accel_pedal1_pos);
		fprintf(fp, " %.2f", eec2->accel_pedal2_pos);
		fprintf(fp, " %.2f", eec2->eng_prcnt_load_curr_spd);
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


void EEC2_interpreter::publish(void *pdv) {
	j1939_eec2_typ *eec2 = (j1939_eec2_typ*) pdv;
}


void *ETC2_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_etc2_typ *etc2 = new j1939_etc2_typ();
	etc2->timestamp = pdu->timestamp;
	unsigned short two_bytes;

	etc2->trans_selected_gear = gear_m125_to_p125(pdu->data_field[0]);

	two_bytes = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	etc2->trans_act_gear_ratio = gear_ratio(two_bytes);

	etc2->trans_current_gear = gear_m125_to_p125(pdu->data_field[3]);

	two_bytes = TWOBYTES(pdu->data_field[5], pdu->data_field[4]);
	etc2->range_selected = two_bytes;

	two_bytes = TWOBYTES(pdu->data_field[7], pdu->data_field[6]);
	etc2->range_attained = two_bytes;

	return (void*) etc2;
}


void ETC2_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void ETC2_interpreter::publish(void *pdv) {
	j1939_etc2_typ *etc2 = (j1939_etc2_typ*) pdv;
}


void *TURBO_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_turbo_typ *turbo = new j1939_turbo_typ();
	turbo->timestamp = pdu->timestamp;
	unsigned short two_bytes;

	turbo->turbo_lube_oil_pressure = pressure_0_to_1000kpa(pdu->data_field[0]);

	two_bytes = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	turbo->turbo_speed = rotor_speed_in_rpm(two_bytes);

	return (void*) turbo;
}


void TURBO_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void TURBO_interpreter::publish(void *pdv) {
	j1939_turbo_typ *turbo = (j1939_turbo_typ*) pdv;
}


void *EEC3_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_eec3_typ *eec3 = new j1939_eec3_typ();
	eec3->timestamp = pdu->timestamp;
	unsigned short two_bytes;

	eec3->nominal_friction = percent_m125_to_p125(pdu->data_field[0]);

	two_bytes = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	eec3->desired_operating_spd = 0.125 * two_bytes;

	eec3->est_eng_prstic_loss =
		 percent_m125_to_p125(pdu->data_field[4]);

	// TODO(ak): add asymmetry adjustment
	return (void*) eec3;
}


void EEC3_interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_eec3_typ *eec3 = (j1939_eec3_typ*) pdv;

	fprintf(fp, "EEC3");
	print_timestamp(fp, &eec3->timestamp);
	if (numeric) {
		fprintf(fp," %.2f", eec3->nominal_friction);
		fprintf(fp," %.2f", eec3->est_eng_prstic_loss);
		fprintf(fp," %.2f", eec3->desired_operating_spd);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp," Nominal friction percent torque %.2f\n",
			 eec3->nominal_friction);
		fprintf(fp," Estimated engine power loss - percent torque %.3f\n",
			 eec3->est_eng_prstic_loss);
		fprintf(fp," Engine desired operating speed %.3f\n",
			 eec3->desired_operating_spd);
	}
	// TODO(ak): add asymmetry adjustment
}


void EEC3_interpreter::publish(void *pdv) {
	j1939_eec3_typ *eec3 = (j1939_eec3_typ*) pdv;
}


void *VD_interpreter::convert(j1939_pdu_typ *pdu) {
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


void VD_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void VD_interpreter::publish(void *pdv) {
	j1939_vd_typ *vd = (j1939_vd_typ*) pdv;
}


// TODO (ak): receive status is not covered here
void *RCFG_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_rcfg_typ *rcfg = new j1939_rcfg_typ();
	rcfg->timestamp = pdu->timestamp;
	unsigned short two_bytes;
	unsigned char data[21];	 // to hold data bytes from 3 packets
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


void RCFG_interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_rcfg_typ *rcfg = (j1939_rcfg_typ*) pdv;
	int i;

	fprintf(fp, "RCFG");
	print_timestamp(fp, &rcfg->timestamp);
	if (numeric) {
		fprintf(fp, "\n");
		fprintf(fp, " %x", rcfg->receive_status);
		fprintf(fp, " %d", rcfg->retarder_loc);
		fprintf(fp, " %d", rcfg->retarder_type);
		fprintf(fp, " %d", rcfg->retarder_ctrl_steps);
		for (i = 0; i < 5; i++)
			fprintf(fp, " %7.2f", rcfg->retarder_speed[i]);
		for (i = 0; i < 5; i++)
			fprintf(fp, " %7.2f", rcfg->percent_torque[i]);
		fprintf(fp, " %7.2f", rcfg->reference_retarder_trq);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Retarder configuration received mask 0x%x \n",
				rcfg->receive_status);
		fprintf(fp, " Retarder location 0x%x, type 0x%x, control %d\n",
				rcfg->retarder_loc, rcfg->retarder_type,
				rcfg->retarder_ctrl_steps);

		fprintf(fp, " Retarder speed ");
		for (i = 0; i < 5; i++)
			fprintf(fp, " %7.2f", rcfg->retarder_speed[i]);
		fprintf(fp, "\n");

		fprintf(fp, "Percent torque ");
		for (i = 0; i < 5; i++)
			fprintf(fp, " %7.2f", rcfg->percent_torque[i]);
		fprintf(fp, "\n");

		fprintf(fp, "Reference retarder torque %7.2f\n",
				rcfg->reference_retarder_trq);
	}
}


void RCFG_interpreter::publish(void *pdv) {
	j1939_rcfg_typ *rcfg = (j1939_rcfg_typ*) pdv;
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


void *ECFG_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_ecfg_typ *ecfg = new j1939_ecfg_typ();
	ecfg->timestamp = pdu->timestamp;
	unsigned short two_bytes;
	unsigned char data[28];	 // to hold data bytes from 4 packets
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

	// TODO(ak): receive_status not covered
	return (void*) ecfg;
}


void ECFG_interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_ecfg_typ *ecfg = (j1939_ecfg_typ*) pdv;
	int i;

	fprintf(fp, "ECFG");
	print_timestamp(fp, &ecfg->timestamp);
	if (numeric) {
		fprintf(fp, " %x", ecfg->receive_status);
		for (i = 0; i < 7; i++)
			fprintf(fp, " %7.2f", ecfg->engine_spd[i]);
		for (i = 0; i < 5; i++)
			fprintf(fp, " %7.2f", ecfg->percent_trq[i]);
		fprintf(fp, " %7.2f", ecfg->gain_endspeed_governor);
		fprintf(fp, " %7.2f", ecfg->reference_eng_trq);
		fprintf(fp, " %7.2f", ecfg->max_momentary_overide_time);
		fprintf(fp, " %7.2f", ecfg->spd_ctrl_lower_lim);
		fprintf(fp, " %7.2f", ecfg->spd_ctrl_upper_lim);
		fprintf(fp, " %7.2f", ecfg->trq_ctrl_lower_lim);
		fprintf(fp, " %7.2f", ecfg->trq_ctrl_upper_lim);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Engine configuration received mask 0x%x \n",
				ecfg->receive_status);
		fprintf(fp, " Engine speed ");
		for (i = 0; i < 7; i++)
			fprintf(fp, " %7.2f", ecfg->engine_spd[i]);
		fprintf(fp, "\n Percent torque ");
		for (i = 0; i < 5; i++)
			fprintf(fp, " %7.2f", ecfg->percent_trq[i]);
		fprintf(fp, "\n Gain endspeed governor %7.2f\n",
				ecfg->gain_endspeed_governor);
		fprintf(fp, " Reference engine torque %7.2f\n",
				ecfg->reference_eng_trq);
		fprintf(fp, " Max Momentary Override Time %7.2f\n",
				ecfg->max_momentary_overide_time);
		fprintf(fp, " Speed Control Lower Limit %7.2f\n",
				ecfg->spd_ctrl_lower_lim);
		fprintf(fp, " Speed Control Upper Limit %7.2f\n",
				ecfg->spd_ctrl_upper_lim);
		fprintf(fp, " Torque Control Lower Limit %7.2f\n",
				ecfg->trq_ctrl_lower_lim);
		fprintf(fp, " Torque Control Upper Limit %7.2f\n",
				ecfg->trq_ctrl_upper_lim);
	}
}


void ECFG_interpreter::publish(void *pdv) {
	j1939_ecfg_typ *ecfg = (j1939_ecfg_typ*) pdv;
}


void *ETEMP_interpreter::convert(j1939_pdu_typ *pdu) {
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


void ETEMP_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void ETEMP_interpreter::publish(void *pdv) {
	j1939_etemp_typ *etemp = (j1939_etemp_typ*) pdv;
}


void *PTO_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_pto_typ *pto = new j1939_pto_typ();
	pto->timestamp = pdu->timestamp;
	unsigned short two_bytes;
	unsigned char byte;

	pto->oil_temp = temp_m40_to_p210(pdu->data_field[0]);

	two_bytes = TWOBYTES(pdu->data_field[2], pdu->data_field[1]);
	pto->speed = speed_in_rpm_2byte(two_bytes);

	two_bytes = TWOBYTES(pdu->data_field[4], pdu->data_field[3]);
	pto->set_speed = speed_in_rpm_2byte(two_bytes);

	byte = pdu->data_field[5];
	pto->remote_variable_spd_status = BITS65(byte);
	pto->remote_preprogramm_status = BITS43(byte);
	pto->enable_switch = BITS21(byte);

	byte = pdu->data_field[6];
	pto->accel_switch = BITS87(byte);
	pto->resume_switch = BITS65(byte);
	pto->coast_decel_switch = BITS43(byte);
	pto->set_switch = BITS21(byte);

	return (void*) pto;
}


void PTO_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void PTO_interpreter::publish(void *pdv) {
	j1939_pto_typ *pto = (j1939_pto_typ*) pdv;
}


void *CCVS_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_ccvs_typ *ccvs = new j1939_ccvs_typ();
	ccvs->timestamp = pdu->timestamp;
	unsigned char byte;
	unsigned short two_bytes;

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


void CCVS_interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_ccvs_typ *ccvs = (j1939_ccvs_typ*) pdv;

	fprintf(fp, "CCVS");
	print_timestamp(fp, &ccvs->timestamp);
	if (numeric){
		fprintf(fp, " %d", ccvs->park_brk_release);
		fprintf(fp, " %d", ccvs->parking_brk_switch);
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


void CCVS_interpreter::publish(void *pdv) {
	j1939_ccvs_typ *ccvs = (j1939_ccvs_typ*) pdv;
}


void *LFE_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_lfe_typ *lfe = new j1939_lfe_typ();
	lfe->timestamp = pdu->timestamp;
	unsigned short two_bytes;

	two_bytes = TWOBYTES(pdu->data_field[1], pdu->data_field[0]);
	lfe->eng_fuel_rate = fuel_rate_cm3_per_sec(two_bytes);

	two_bytes = TWOBYTES(pdu->data_field[3], pdu->data_field[2]);
	lfe->eng_inst_fuel_economy = fuel_economy_meters_per_cm3(two_bytes);

	two_bytes = TWOBYTES(pdu->data_field[5], pdu->data_field[4]);
	lfe->eng_avg_fuel_economy = fuel_economy_meters_per_cm3(two_bytes);

	lfe->eng_throttle_valve1_pos = percent_0_to_100(pdu->data_field[6]);

	return (void*) lfe;
}


void LFE_interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_lfe_typ *lfe = (j1939_lfe_typ*) pdv;

	fprintf(fp, "LFE");
	print_timestamp(fp, &lfe->timestamp);
	if (numeric) {
		fprintf(fp, " %.3f", lfe->eng_fuel_rate);
		fprintf(fp, " %.3f", lfe->eng_inst_fuel_economy);
		fprintf(fp, " %.3f", lfe->eng_avg_fuel_economy);
		fprintf(fp, " %.3f", lfe->eng_throttle_valve1_pos);
		fprintf(fp, "\n");
	} else {
		fprintf(fp, "\n");
		fprintf(fp, " Fuel rate (cm3/sec) %.3f\n", lfe->eng_fuel_rate);
		fprintf(fp, " Instantaneous fuel economy (m/cm3) %.3f\n",
				lfe->eng_inst_fuel_economy);
		fprintf(fp, " Average fuel economy (m/cm3) %.3f\n",
				lfe->eng_avg_fuel_economy);
		fprintf(fp, " Throttle position (percent) %.3f\n",
				lfe->eng_throttle_valve1_pos);
	}
}


void LFE_interpreter::publish(void *pdv) {
	j1939_lfe_typ *lfe = (j1939_lfe_typ*) pdv;
}


void *AMBC_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_ambc_typ *ambc = new j1939_ambc_typ();
	ambc->timestamp = pdu->timestamp;
	unsigned short two_bytes;

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


void AMBC_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void AMBC_interpreter::publish(void *pdv) {
	j1939_ambc_typ *ambc = (j1939_ambc_typ*) pdv;
}


void *IEC_interpreter::convert(j1939_pdu_typ *pdu) {
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

	iec->coolant_filter_diff_pressure =
		pressure_0_to_125kpa(pdu->data_field[7]);

	return (void*) iec;
}


void IEC_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void IEC_interpreter::publish(void *pdv) {
	j1939_iec_typ *iec = (j1939_iec_typ*) pdv;
}


void *VEP_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_vep_typ *vep = new j1939_vep_typ();
	vep->timestamp = pdu->timestamp;
	unsigned short two_bytes;

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


void VEP_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void VEP_interpreter::publish(void *pdv) {
	j1939_vep_typ *vep = (j1939_vep_typ*) pdv;
}


void *TF_interpreter::convert(j1939_pdu_typ *pdu) {
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


void TF_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void TF_interpreter::publish(void *pdv) {
	j1939_tf_typ *tf = (j1939_tf_typ*) pdv;
}


void *RF_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_rf_typ *rf = new j1939_rf_typ();
	rf->timestamp = pdu->timestamp;
	rf->pressure = pressure_0_to_4000kpa(pdu->data_field[0]);
	rf->oil_temp = temp_m40_to_p210(pdu->data_field[1]);
	return (void*) rf;
}


void RF_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void RF_interpreter::publish(void *pdv) {
	j1939_rf_typ *rf = (j1939_rf_typ*) pdv;
}


void *HRVD_interpreter::convert(j1939_pdu_typ *pdu) {
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


void HRVD_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void HRVD_interpreter::publish(void *pdv) {
	j1939_hrvd_typ *hrvd = (j1939_hrvd_typ*) pdv;
}


void *EBC2_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_ebc2_typ *ebc2 = new j1939_ebc2_typ();
	ebc2->timestamp = pdu->timestamp;
	unsigned short two_bytes;

	two_bytes = TWOBYTES(pdu->data_field[1], pdu->data_field[0]);
	ebc2->front_axle_spd = wheel_based_mps(two_bytes);
	ebc2->rel_spd_front_left = wheel_based_mps_relative(pdu->data_field[2]);
	ebc2->rel_spd_front_right = wheel_based_mps_relative(pdu->data_field[3]);
	ebc2->rel_spd_rear_left_1 = wheel_based_mps_relative(pdu->data_field[4]);
	ebc2->rel_spd_rear_right_1 = wheel_based_mps_relative(pdu->data_field[5]);
	ebc2->rel_spd_rear_left_2 = wheel_based_mps_relative(pdu->data_field[6]);
	ebc2->rel_spd_rear_right_2 = wheel_based_mps_relative(pdu->data_field[7]);

	return (void*) ebc2;
}


void EBC2_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void EBC2_interpreter::publish(void *pdv) {
	j1939_ebc2_typ *ebc2 = (j1939_ebc2_typ*) pdv;
}


void *FD_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_fd_typ *fd = new j1939_fd_typ();
	fd->timestamp = pdu->timestamp;
	fd->prcnt_fan_spd = percent_0_to_100(pdu->data_field[0]);
	fd->fan_drive_state = LONIBBLE(pdu->data_field[1]);
	return (void*) fd;
}


void FD_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void FD_interpreter::publish(void *pdv) {
	j1939_fd_typ *fd = (j1939_fd_typ*) pdv;
}


// TODO(ak): what about the rest?
void *GFI2_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_gfi2_typ *gfi2 = new j1939_gfi2_typ();
	gfi2->timestamp = pdu->timestamp;
	gfi2->fuel_valve_pos1 = pdu->data_field[2];
	return (void*) gfi2;
}


void GFI2_interpreter::print(void *pdv, FILE *fp, bool numeric) {
	j1939_gfi2_typ *gfi2 = (j1939_gfi2_typ*) pdv;

	fprintf(fp, "GFI2");
	print_timestamp(fp, &gfi2->timestamp);
	if (numeric)
		fprintf(fp, " %.3f\n", gfi2->fuel_valve_pos1);
	else
		fprintf(fp, "\n");
		fprintf(fp, " Fuel valve 1 position %.3f\n", gfi2->fuel_valve_pos1);
}


void GFI2_interpreter::publish(void *pdv) {
	j1939_gfi2_typ *gfi2 = (j1939_gfi2_typ*) pdv;
}


void *EI_interpreter::convert(j1939_pdu_typ *pdu) {
	j1939_ei_typ *ei = new j1939_ei_typ();
	ei->timestamp = pdu->timestamp;
	short data;

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


void EI_interpreter::print(void *pdv, FILE *fp, bool numeric) {
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


void EI_interpreter::publish(void *pdv) {
	j1939_ei_typ *ei = (j1939_ei_typ*) pdv;
}

