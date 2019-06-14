/**\file
 *
 * publish.cpp
 *
 * Implements the publish-related methods in pub_sub.h.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date January 16, 2019
 */

#include "pub_sub.h"
#include <jbus/j1939_utils.h>
#include <sys/pps.h>


PubSub::~PubSub() {}


void PubSub::publish(int type, void *message) {
	switch(type) {
		case PDU   : this->_publish_pdu(message);
				 	 break;
		case TSC1  : this->_publish_tsc1(message);
					 break;
		case EBC1  : this->_publish_ebc1(message);
					 break;
		case EBC2  : this->_publish_ebc2(message);
					 break;
		case EEC1  : this->_publish_eec1(message);
					 break;
		case EEC2  : this->_publish_eec2(message);
					 break;
		case EEC3  : this->_publish_eec3(message);
					 break;
		case ERC1  : this->_publish_erc1(message);
					 break;
		case ETC1  : this->_publish_etc1(message);
					 break;
		case ETC2  : this->_publish_etc2(message);
					 break;
		case TURBO : this->_publish_turbo(message);
					 break;
		case VD    : this->_publish_vd(message);
					 break;
		case RCFG  : this->_publish_rcfg(message);
					 break;
		case ECFG  : this->_publish_ecfg(message);
					 break;
		case ETEMP : this->_publish_etemp(message);
					 break;
		case PTO   : this->_publish_pto(message);
					 break;
		case CCVS  : this->_publish_ccvs(message);
					 break;
		case LFE   : this->_publish_lfe(message);
					 break;
		case AMBC  : this->_publish_ambc(message);
					 break;
		case IEC   : this->_publish_iec(message);
					 break;
		case VEP   : this->_publish_vep(message);
					 break;
		case TF    : this->_publish_tf(message);
					 break;
		case RF    : this->_publish_rf(message);
					 break;
		case HRVD  : this->_publish_hrvd(message);
					 break;
		case FD    : this->_publish_fd(message);
					 break;
		case GFI2  : this->_publish_gfi2(message);
					 break;
		case EI    : this->_publish_ei(message);
					 break;
	}
}


void PubSub::_publish_pdu(void *data) {
	j1939_pdu_typ *pdu = (j1939_pdu_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@PDU");
	encode_timestamp(encoder, &pdu->timestamp);

	pps_encoder_add_int(&encoder, "priority", pdu->priority);
	pps_encoder_add_int(&encoder, "pdu_format", pdu->pdu_format);
	pps_encoder_add_int(&encoder, "pdu_specific", pdu->pdu_specific);
	pps_encoder_add_int(&encoder, "src_address", pdu->src_address);
	pps_encoder_add_int(&encoder, "num_bytes", pdu->num_bytes);
	pps_encoder_start_object(&encoder, "data_field");
	for (int i=0; i<pdu->num_bytes; ++i)
		pps_encoder_add_int(&encoder, to_string(i).c_str(), pdu->data_field[i]);
	pps_encoder_end_object(&encoder);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_tsc1(void *data) {
	j1939_tsc1_typ *tsc1 = (j1939_tsc1_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@TSC1");
	encode_timestamp(encoder, &tsc1->timestamp);

	pps_encoder_add_int(&encoder, "destination_address",
			tsc1->destination_address);
	pps_encoder_add_int(&encoder, "src_address", tsc1->src_address);
	pps_encoder_add_int(&encoder, "ovrd_ctrl_m_pr", tsc1->ovrd_ctrl_m_pr);
	pps_encoder_add_int(&encoder, "req_spd_ctrl", tsc1->req_spd_ctrl);
	pps_encoder_add_int(&encoder, "ovrd_ctrl_m", tsc1->ovrd_ctrl_m);
	pps_encoder_add_double(&encoder, "req_spd_lim", tsc1->req_spd_lim);
	pps_encoder_add_double(&encoder, "req_trq_lim", tsc1->req_trq_lim);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_ebc1(void *data) {
	j1939_ebc1_typ *ebc1 = (j1939_ebc1_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@EBC1");
	encode_timestamp(encoder, &ebc1->timestamp);

	pps_encoder_add_int(&encoder, "ebs_brk_switch", ebc1->ebs_brk_switch);
	pps_encoder_add_int(&encoder, "antilock_brk_active",
			ebc1->antilock_brk_active);
	pps_encoder_add_int(&encoder, "asr_brk_ctrl_active",
			ebc1->asr_brk_ctrl_active);
	pps_encoder_add_int(&encoder, "asr_engine_ctrl_active",
			ebc1->asr_engine_ctrl_active);
	pps_encoder_add_double(&encoder, "brk_pedal_pos", ebc1->brk_pedal_pos);
	pps_encoder_add_int(&encoder, "trac_ctrl_override_switch",
			ebc1->trac_ctrl_override_switch);
	pps_encoder_add_int(&encoder, "asr_hillholder_switch",
			ebc1->asr_hillholder_switch);
	pps_encoder_add_int(&encoder, "abs_offroad_switch",
			ebc1->abs_offroad_switch);
	pps_encoder_add_int(&encoder, "asr_offroad_switch",
			ebc1->asr_offroad_switch);
	pps_encoder_add_int(&encoder, "accel_enable_switch",
			ebc1->accel_enable_switch);
	pps_encoder_add_int(&encoder, "aux_eng_shutdown_switch",
			ebc1->aux_eng_shutdown_switch);
	pps_encoder_add_int(&encoder, "eng_derate_switch", ebc1->eng_derate_switch);
	pps_encoder_add_int(&encoder, "accel_interlock_switch",
			ebc1->accel_interlock_switch);
	pps_encoder_add_double(&encoder, "eng_retarder_selection",
			ebc1->eng_retarder_selection);
	pps_encoder_add_int(&encoder, "abs_ebs_amber_warning",
			ebc1->abs_ebs_amber_warning);
	pps_encoder_add_int(&encoder, "ebs_red_warning", ebc1->ebs_red_warning);
	pps_encoder_add_int(&encoder, "abs_fully_operational",
			ebc1->abs_fully_operational);
	pps_encoder_add_int(&encoder, "src_address_ctrl", ebc1->src_address_ctrl);
	pps_encoder_add_double(&encoder, "total_brk_demand",
			ebc1->total_brk_demand);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_ebc2(void *data) {
	j1939_ebc2_typ *ebc2 = (j1939_ebc2_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@EBC2");
	encode_timestamp(encoder, &ebc2->timestamp);

	pps_encoder_add_double(&encoder, "front_axle_spd", ebc2->front_axle_spd);
	pps_encoder_add_double(&encoder, "rel_spd_front_left",
			ebc2->rel_spd_front_left);
	pps_encoder_add_double(&encoder, "rel_spd_front_right",
			ebc2->rel_spd_front_right);
	pps_encoder_add_double(&encoder, "rel_spd_rear_left_1",
			ebc2->rel_spd_rear_left_1);
	pps_encoder_add_double(&encoder, "rel_spd_rear_right_1",
			ebc2->rel_spd_rear_right_1);
	pps_encoder_add_double(&encoder, "rel_spd_rear_left_2",
			ebc2->rel_spd_rear_left_2);
	pps_encoder_add_double(&encoder, "rel_spd_rear_right_2",
			ebc2->rel_spd_rear_right_2);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_eec1(void *data) {
	j1939_eec1_typ *eec1 = (j1939_eec1_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@EEC1");
	encode_timestamp(encoder, &eec1->timestamp);

	pps_encoder_add_int(&encoder, "eng_trq_mode", eec1->eng_trq_mode);
	pps_encoder_add_double(&encoder, "drvr_demand_eng_trq",
			eec1->drvr_demand_eng_trq);
	pps_encoder_add_double(&encoder, "actual_eng_trq", eec1->actual_eng_trq);
	pps_encoder_add_double(&encoder, "eng_demand_trq", eec1->eng_demand_trq);
	pps_encoder_add_double(&encoder, "eng_spd", eec1->eng_spd);
	pps_encoder_add_int(&encoder, "src_address", eec1->src_address);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_eec2(void *data) {
	j1939_eec2_typ *eec2 = (j1939_eec2_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@EEC2");
	encode_timestamp(encoder, &eec2->timestamp);

	pps_encoder_add_int(&encoder, "spd_limit_status", eec2->spd_limit_status);
	pps_encoder_add_int(&encoder, "accel_pedal_kickdown",
			eec2->accel_pedal_kickdown);
	pps_encoder_add_int(&encoder, "accel_pedal1_idle", eec2->accel_pedal1_idle);
	pps_encoder_add_int(&encoder, "accel_pedal2_idle", eec2->accel_pedal2_idle);
	pps_encoder_add_double(&encoder, "act_max_avail_eng_trq",
			eec2->act_max_avail_eng_trq);
	pps_encoder_add_double(&encoder, "accel_pedal1_pos",
			eec2->accel_pedal1_pos);
	pps_encoder_add_double(&encoder, "accel_pedal2_pos",
			eec2->accel_pedal2_pos);
	pps_encoder_add_double(&encoder, "eng_prcnt_load_curr_spd",
			eec2->eng_prcnt_load_curr_spd);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_eec3(void *data) {
	j1939_eec3_typ *eec3 = (j1939_eec3_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@EEC3");
	encode_timestamp(encoder, &eec3->timestamp);

	pps_encoder_add_double(&encoder, "nominal_friction",
			eec3->nominal_friction);
	pps_encoder_add_double(&encoder, "est_eng_prstic_loss",
			eec3->est_eng_prstic_loss);
	pps_encoder_add_int(&encoder, "operating_spd_adjust",
			eec3->operating_spd_adjust);
	pps_encoder_add_double(&encoder, "desired_operating_spd",
			eec3->desired_operating_spd);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_erc1(void *data) {
	j1939_erc1_typ *erc1 = (j1939_erc1_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@ERC1");
	encode_timestamp(encoder, &erc1->timestamp);

	pps_encoder_add_int(&encoder, "enable_shift_assist",
			erc1->enable_shift_assist);
	pps_encoder_add_int(&encoder, "enable_brake_assist",
			erc1->enable_brake_assist);
	pps_encoder_add_int(&encoder, "trq_mode", erc1->trq_mode);
	pps_encoder_add_double(&encoder, "actual_ret_pcnt_trq",
			erc1->actual_ret_pcnt_trq);
	pps_encoder_add_double(&encoder, "intended_ret_pcnt_trq",
			erc1->intended_ret_pcnt_trq);
	pps_encoder_add_int(&encoder, "rq_brake_light", erc1->rq_brake_light);
	pps_encoder_add_int(&encoder, "src_address_ctrl", erc1->src_address_ctrl);
	pps_encoder_add_int(&encoder, "drvrs_demand_prcnt_trq",
			erc1->drvrs_demand_prcnt_trq);
	pps_encoder_add_double(&encoder, "selection_nonengine",
			erc1->selection_nonengine);
	pps_encoder_add_int(&encoder, "max_available_prcnt_trq",
			erc1->max_available_prcnt_trq);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_etc1(void *data) {
	j1939_etc1_typ *etc1 = (j1939_etc1_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@ETC1");
	encode_timestamp(encoder, &etc1->timestamp);

	pps_encoder_add_int(&encoder, "trans_shift", etc1->trans_shift);
	pps_encoder_add_int(&encoder, "trq_conv_lockup", etc1->trq_conv_lockup);
	pps_encoder_add_int(&encoder, "trans_driveline", etc1->trans_driveline);
	pps_encoder_add_double(&encoder, "tran_output_shaft_spd",
			etc1->tran_output_shaft_spd);
	pps_encoder_add_double(&encoder, "prcnt_clutch_slip",
			etc1->prcnt_clutch_slip);
	pps_encoder_add_int(&encoder, "prog_shift_disable",
			etc1->prog_shift_disable);
	pps_encoder_add_int(&encoder, "eng_overspd_enable",
			etc1->eng_overspd_enable);
	pps_encoder_add_double(&encoder, "trans_input_shaft_spd",
			etc1->trans_input_shaft_spd);
	pps_encoder_add_int(&encoder, "src_address_ctrl", etc1->src_address_ctrl);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_etc2(void *data) {
	j1939_etc2_typ *etc2 = (j1939_etc2_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@ETC2");
	encode_timestamp(encoder, &etc2->timestamp);

	pps_encoder_add_int(&encoder, "trans_selected_gear",
			etc2->trans_selected_gear);
	pps_encoder_add_double(&encoder, "trans_act_gear_ratio",
			etc2->trans_act_gear_ratio);
	pps_encoder_add_int(&encoder, "trans_current_gear",
			etc2->trans_current_gear);
	pps_encoder_add_int(&encoder, "range_selected", etc2->range_selected);
	pps_encoder_add_int(&encoder, "range_attained", etc2->range_attained);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_turbo(void *data) {
	j1939_turbo_typ *turbo = (j1939_turbo_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@TURBO");
	encode_timestamp(encoder, &turbo->timestamp);

	pps_encoder_add_double(&encoder, "turbo_lube_oil_pressure",
			turbo->turbo_lube_oil_pressure);
	pps_encoder_add_double(&encoder, "turbo_speed", turbo->turbo_speed);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_vd(void *data) {
	j1939_vd_typ *vd = (j1939_vd_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@VD");
	encode_timestamp(encoder, &vd->timestamp);

	pps_encoder_add_double(&encoder, "trip_dist", vd->trip_dist);
	pps_encoder_add_double(&encoder, "tot_vehicle_dist", vd->tot_vehicle_dist);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_rcfg(void *data) {
	j1939_rcfg_typ *rcfg = (j1939_rcfg_typ*) data;

	int i;
	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@RCFG");
	encode_timestamp(encoder, &rcfg->timestamp);

	pps_encoder_add_int(&encoder, "retarder_loc", rcfg->retarder_loc);
	pps_encoder_add_int(&encoder, "retarder_type", rcfg->retarder_type);
	pps_encoder_add_int(&encoder, "retarder_ctrl_steps",
			rcfg->retarder_ctrl_steps);
	pps_encoder_add_double(&encoder, "reference_retarder_trq",
			rcfg->reference_retarder_trq);

	pps_encoder_start_object(&encoder, "retarder_speed");
	for (i = 0; i < 5; i++) {
		pps_encoder_add_double(&encoder, to_string(i).c_str(),
				rcfg->retarder_speed[i]);
	}
	pps_encoder_end_object(&encoder);

	pps_encoder_start_object(&encoder, "percent_torque");
	for (i = 0; i < 5; i++) {
		pps_encoder_add_double(&encoder, to_string(i).c_str(),
				rcfg->percent_torque[i]);
	}
	pps_encoder_end_object(&encoder);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_ecfg(void *data) {
	j1939_ecfg_typ *ecfg = (j1939_ecfg_typ*) data;

	int i;
	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@ECFG");
	encode_timestamp(encoder, &ecfg->timestamp);

	pps_encoder_add_int(&encoder, "receive_status", ecfg->receive_status);

	pps_encoder_start_object(&encoder, "engine_spd");
	for (i = 0; i < 7; i++)
		pps_encoder_add_double(&encoder, to_string(i).c_str(), ecfg->engine_spd[i]);
	pps_encoder_end_object(&encoder);

	pps_encoder_start_object(&encoder, "percent_trq");
	for (i = 0; i < 5; i++)
		pps_encoder_add_double(&encoder, to_string(i).c_str(), ecfg->percent_trq[i]);
	pps_encoder_end_object(&encoder);

	pps_encoder_add_double(&encoder, "gain_endspeed_governor",
			ecfg->gain_endspeed_governor);
	pps_encoder_add_double(&encoder, "reference_eng_trq",
			ecfg->reference_eng_trq);
	pps_encoder_add_double(&encoder, "max_momentary_overide_time",
			ecfg->max_momentary_overide_time);
	pps_encoder_add_double(&encoder, "spd_ctrl_lower_lim",
			ecfg->spd_ctrl_lower_lim);
	pps_encoder_add_double(&encoder, "spd_ctrl_upper_lim",
			ecfg->spd_ctrl_upper_lim);
	pps_encoder_add_double(&encoder, "trq_ctrl_lower_lim",
			ecfg->trq_ctrl_lower_lim);
	pps_encoder_add_double(&encoder, "trq_ctrl_upper_lim",
			ecfg->trq_ctrl_upper_lim);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_etemp(void *data) {
	j1939_etemp_typ *etemp = (j1939_etemp_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@ETEMP");
	encode_timestamp(encoder, &etemp->timestamp);

	pps_encoder_add_double(&encoder, "eng_coolant_temp",
			etemp->eng_coolant_temp);
	pps_encoder_add_double(&encoder, "fuel_temp", etemp->fuel_temp);
	pps_encoder_add_double(&encoder, "eng_oil_temp", etemp->eng_oil_temp);
	pps_encoder_add_double(&encoder, "turbo_oil_temp", etemp->turbo_oil_temp);
	pps_encoder_add_double(&encoder, "eng_intercooler_temp",
			etemp->eng_intercooler_temp);
	pps_encoder_add_double(&encoder, "eng_intercooler_thermostat_opening",
			etemp->eng_intercooler_thermostat_opening);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_pto(void *data) {
	j1939_pto_typ *pto = (j1939_pto_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@PTO");
	encode_timestamp(encoder, &pto->timestamp);

	pps_encoder_add_double(&encoder, "oil_temp", pto->oil_temp);
	pps_encoder_add_double(&encoder, "speed", pto->speed);
	pps_encoder_add_double(&encoder, "set_speed", pto->set_speed);
	pps_encoder_add_int(&encoder, "remote_variable_spd_status",
			pto->remote_variable_spd_status);
	pps_encoder_add_int(&encoder, "remote_preprogramm_status",
			pto->remote_preprogramm_status);
	pps_encoder_add_int(&encoder, "enable_switch", pto->enable_switch);
	pps_encoder_add_int(&encoder, "accel_switch", pto->accel_switch);
	pps_encoder_add_int(&encoder, "resume_switch", pto->resume_switch);
	pps_encoder_add_int(&encoder, "coast_decel_switch",
			pto->coast_decel_switch);
	pps_encoder_add_int(&encoder, "set_switch", pto->set_switch);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_ccvs(void *data) {
	j1939_ccvs_typ *ccvs = (j1939_ccvs_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@CCVS");
	encode_timestamp(encoder, &ccvs->timestamp);

	pps_encoder_add_int(&encoder, "park_brk_release", ccvs->park_brk_release);
	pps_encoder_add_int(&encoder, "parking_brk_switch",
			ccvs->parking_brk_switch);
	pps_encoder_add_int(&encoder, "two_spd_axle_switch",
			ccvs->two_spd_axle_switch);
	pps_encoder_add_double(&encoder, "vehicle_spd", ccvs->vehicle_spd);
	pps_encoder_add_int(&encoder, "clutch_switch", ccvs->clutch_switch);
	pps_encoder_add_int(&encoder, "brk_switch", ccvs->brk_switch);
	pps_encoder_add_int(&encoder, "cc_pause_switch", ccvs->cc_pause_switch);
	pps_encoder_add_int(&encoder, "cc_enable_switch", ccvs->cc_enable_switch);
	pps_encoder_add_int(&encoder, "cc_active", ccvs->cc_active);
	pps_encoder_add_int(&encoder, "cc_accel_switch", ccvs->cc_accel_switch);
	pps_encoder_add_int(&encoder, "cc_resume_switch", ccvs->cc_resume_switch);
	pps_encoder_add_int(&encoder, "cc_coast_switch", ccvs->cc_coast_switch);
	pps_encoder_add_int(&encoder, "cc_set_switch", ccvs->cc_set_switch);
	pps_encoder_add_double(&encoder, "cc_set_speed", ccvs->cc_set_speed);
	pps_encoder_add_int(&encoder, "cc_state", ccvs->cc_state);
	pps_encoder_add_int(&encoder, "pto_state", ccvs->pto_state);
	pps_encoder_add_int(&encoder, "eng_shutdown_override",
			ccvs->eng_shutdown_override);
	pps_encoder_add_int(&encoder, "eng_test_mode_switch",
			ccvs->eng_test_mode_switch);
	pps_encoder_add_int(&encoder, "eng_idle_decr_switch",
			ccvs->eng_idle_decr_switch);
	pps_encoder_add_int(&encoder, "eng_idle_incr_switch",
			ccvs->eng_idle_incr_switch);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_lfe(void *data) {
	j1939_lfe_typ *lfe = (j1939_lfe_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@LFE");
	encode_timestamp(encoder, &lfe->timestamp);

	pps_encoder_add_double(&encoder, "eng_fuel_rate", lfe->eng_fuel_rate);
	pps_encoder_add_double(&encoder, "eng_inst_fuel_economy",
			lfe->eng_inst_fuel_economy);
	pps_encoder_add_double(&encoder, "eng_avg_fuel_economy",
			lfe->eng_avg_fuel_economy);
	pps_encoder_add_double(&encoder, "eng_throttle1_pos",
			lfe->eng_throttle1_pos);
	pps_encoder_add_double(&encoder, "eng_throttle2_pos",
			lfe->eng_throttle2_pos);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_ambc(void *data) {
	j1939_ambc_typ *ambc = (j1939_ambc_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@AMBC");
	encode_timestamp(encoder, &ambc->timestamp);

	pps_encoder_add_double(&encoder, "barometric_pressure",
			ambc->barometric_pressure);
	pps_encoder_add_double(&encoder, "cab_interior_temp",
			ambc->cab_interior_temp);
	pps_encoder_add_double(&encoder, "ambient_air_temp",
			ambc->ambient_air_temp);
	pps_encoder_add_double(&encoder, "air_inlet_temp", ambc->air_inlet_temp);
	pps_encoder_add_double(&encoder, "road_surface_temp",
			ambc->road_surface_temp);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_iec(void *data) {
	j1939_iec_typ *iec = (j1939_iec_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@IEC");
	encode_timestamp(encoder, &iec->timestamp);

	pps_encoder_add_double(&encoder, "particulate_inlet_pressure",
			iec->particulate_inlet_pressure);
	pps_encoder_add_double(&encoder, "boost_pressure", iec->boost_pressure);
	pps_encoder_add_double(&encoder, "intake_manifold_temp",
			iec->intake_manifold_temp);
	pps_encoder_add_double(&encoder, "air_inlet_pressure",
			iec->air_inlet_pressure);
	pps_encoder_add_double(&encoder, "air_filter_diff_pressure",
			iec->air_filter_diff_pressure);
	pps_encoder_add_double(&encoder, "exhaust_gas_temp", iec->exhaust_gas_temp);
	pps_encoder_add_double(&encoder, "coolant_filter_diff_pressure",
			iec->coolant_filter_diff_pressure);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_vep(void *data) {
	j1939_vep_typ *vep = (j1939_vep_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@VEP");
	encode_timestamp(encoder, &vep->timestamp);

	pps_encoder_add_double(&encoder, "net_battery_current",
			vep->net_battery_current);
	pps_encoder_add_double(&encoder, "alternator_current",
			vep->alternator_current);
	pps_encoder_add_double(&encoder, "alternator_potential",
			vep->alternator_potential);
	pps_encoder_add_double(&encoder, "electrical_potential",
			vep->electrical_potential);
	pps_encoder_add_double(&encoder, "battery_potential",
			vep->battery_potential);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_tf(void *data) {
	j1939_tf_typ *tf = (j1939_tf_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@TF");
	encode_timestamp(encoder, &tf->timestamp);

	pps_encoder_add_double(&encoder, "clutch_pressure", tf->clutch_pressure);
	pps_encoder_add_double(&encoder, "oil_level", tf->oil_level);
	pps_encoder_add_double(&encoder, "diff_pressure", tf->diff_pressure);
	pps_encoder_add_double(&encoder, "oil_pressure", tf->oil_pressure);
	pps_encoder_add_double(&encoder, "oil_temp", tf->oil_temp);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_rf(void *data) {
	j1939_rf_typ *rf = (j1939_rf_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@RF");
	encode_timestamp(encoder, &rf->timestamp);

	pps_encoder_add_double(&encoder, "pressure", rf->pressure);
	pps_encoder_add_double(&encoder, "oil_temp", rf->oil_temp);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_hrvd(void *data) {
	j1939_hrvd_typ *hrvd = (j1939_hrvd_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@HRVD");
	encode_timestamp(encoder, &hrvd->timestamp);

	pps_encoder_add_double(&encoder, "vehicle_distance", hrvd->vehicle_distance);
	pps_encoder_add_double(&encoder, "trip_distance", hrvd->trip_distance);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_fd(void *data) {
	j1939_fd_typ *fdd = (j1939_fd_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@FD");
	encode_timestamp(encoder, &fdd->timestamp);

	pps_encoder_add_double(&encoder, "prcnt_fan_spd", fdd->prcnt_fan_spd);
	pps_encoder_add_int(&encoder, "fan_drive_state", fdd->fan_drive_state);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_gfi2(void *data) {
	j1939_gfi2_typ *gfi2 = (j1939_gfi2_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@GFI2");
	encode_timestamp(encoder, &gfi2->timestamp);

	pps_encoder_add_double(&encoder, "fuel_flow_rate1", gfi2->fuel_flow_rate1);
	pps_encoder_add_double(&encoder, "fuel_flow_rate2", gfi2->fuel_flow_rate2);
	pps_encoder_add_double(&encoder, "fuel_valve_pos1", gfi2->fuel_valve_pos1);
	pps_encoder_add_double(&encoder, "fuel_valve_pos2", gfi2->fuel_valve_pos2);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}


void PubSub::_publish_ei(void *data) {
	j1939_ei_typ *ei = (j1939_ei_typ*) data;

	// initialize the encoder object
	pps_encoder_t encoder;
	pps_encoder_initialize(&encoder, false);

	// setup the object to be encoded
	pps_encoder_start_object(&encoder, "@EI");
	encode_timestamp(encoder, &ei->timestamp);

	pps_encoder_add_double(&encoder, "pre_filter_oil_pressure",
			ei->pre_filter_oil_pressure);
	pps_encoder_add_double(&encoder, "exhaust_gas_pressure",
			ei->exhaust_gas_pressure);
	pps_encoder_add_double(&encoder, "rack_position", ei->rack_position);
	pps_encoder_add_double(&encoder, "eng_gas_mass_flow",
			ei->eng_gas_mass_flow);
	pps_encoder_add_double(&encoder, "inst_estimated_brake_power",
			ei->inst_estimated_brake_power);

	pps_encoder_end_object(&encoder);

	// perform the data encoding procedure
	if (pps_encoder_buffer(&encoder) != NULL)
		write(this->_fd_pub, pps_encoder_buffer(&encoder), pps_encoder_length(&encoder));
	pps_encoder_cleanup(&encoder);
}
