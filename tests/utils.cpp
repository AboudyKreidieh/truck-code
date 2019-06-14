/**\file
 *
 * utils.cpp
 *
 * Implements methods in utils.h
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date June 13, 2019
 */
#include "utils.h"
#include "jbus/j1939_utils.h"
#include "jbus/j1939_interpreters.h"


bool check_timestamp_equal(
		timestamp_t time, int hour, int minute, int second, int millisecond) {
	return (time.hour == hour &&
			time.minute == minute &&
			time.second == second &&
			time.millisecond == millisecond);
}

bool check_pdu(
		j1939_pdu_typ *pdu,
		int priority,
		int pdu_format,
		int pdu_specific,
		int src_address,
		int data_field[8],
		int num_bytes) {
	// check the data field
	for (int i=0; i<8; ++i) {
		if (pdu->data_field[i] != data_field[i])
			return false;
	}
	return (pdu->priority == priority &&
			pdu->pdu_format == pdu_format &&
			pdu->pdu_specific == pdu_specific &&
			pdu->src_address == src_address &&
			pdu->num_bytes == num_bytes);
}

bool check_tsc1(
		j1939_tsc1_typ *tsc1,
		int ovrd_ctrl_m,
		int req_spd_ctrl,
		int ovrd_ctrl_m_pr,
		float req_spd_lim,
		float req_trq_lim,
		int destination_address,
		int src_address) {
	return (tsc1->ovrd_ctrl_m == ovrd_ctrl_m &&
			tsc1->req_spd_ctrl == req_spd_ctrl &&
			tsc1->ovrd_ctrl_m_pr == ovrd_ctrl_m_pr &&
			tsc1->req_spd_lim == req_spd_lim &&
			tsc1->req_trq_lim == req_trq_lim &&
			tsc1->destination_address == destination_address &&
			tsc1->src_address == src_address);
}

bool check_ebc1(
		j1939_ebc1_typ *ebc1,
		int asr_engine_ctrl_active,
		int asr_brk_ctrl_active,
		int antilock_brk_active,
		int ebs_brk_switch,
		float brk_pedal_pos,
		int abs_offroad_switch,
		int asr_offroad_switch,
		int asr_hillholder_switch,
		int trac_ctrl_override_switch,
		int accel_interlock_switch,
		int eng_derate_switch,
		int aux_eng_shutdown_switch,
		int accel_enable_switch,
		float eng_retarder_selection,
		int abs_fully_operational,
		int ebs_red_warning,
		int abs_ebs_amber_warning,
		int src_address_ctrl,
		float total_brk_demand) {
	return (ebc1->asr_engine_ctrl_active == asr_engine_ctrl_active &&
			ebc1->asr_brk_ctrl_active == asr_brk_ctrl_active &&
			ebc1->antilock_brk_active == antilock_brk_active &&
			ebc1->ebs_brk_switch == ebs_brk_switch &&
			ebc1->brk_pedal_pos == brk_pedal_pos &&
			ebc1->abs_offroad_switch == abs_offroad_switch &&
			ebc1->asr_offroad_switch == asr_offroad_switch &&
			ebc1->asr_hillholder_switch == asr_hillholder_switch &&
			ebc1->trac_ctrl_override_switch == trac_ctrl_override_switch &&
			ebc1->accel_interlock_switch == accel_interlock_switch &&
			ebc1->eng_derate_switch == eng_derate_switch &&
			ebc1->aux_eng_shutdown_switch == aux_eng_shutdown_switch &&
			ebc1->accel_enable_switch == accel_enable_switch &&
			ebc1->eng_retarder_selection == eng_retarder_selection &&
			ebc1->abs_fully_operational == abs_fully_operational &&
			ebc1->ebs_red_warning == ebs_red_warning &&
			ebc1->abs_ebs_amber_warning == abs_ebs_amber_warning &&
			ebc1->src_address_ctrl == src_address_ctrl &&
			ebc1->total_brk_demand == total_brk_demand);
}

bool check_ebc2(
		j1939_ebc2_typ *ebc2,
		float front_axle_spd,
		float rel_spd_front_left,
		float rel_spd_front_right,
		float rel_spd_rear_left_1,
		float rel_spd_rear_right_1,
		float rel_spd_rear_left_2,
		float rel_spd_rear_right_2) {
	return ((ebc2->front_axle_spd - front_axle_spd) < 0.01 &&
			(ebc2->rel_spd_front_left - rel_spd_front_left) < 0.01 &&
			(ebc2->rel_spd_front_right - rel_spd_front_right) < 0.01 &&
			(ebc2->rel_spd_rear_left_1 - rel_spd_rear_left_1) < 0.01 &&
			(ebc2->rel_spd_rear_right_1 - rel_spd_rear_right_1) < 0.01 &&
			(ebc2->rel_spd_rear_left_2 - rel_spd_rear_left_2) < 0.01 &&
			(ebc2->rel_spd_rear_right_2 - rel_spd_rear_right_2) < 0.01);
}

bool check_eec1(
		j1939_eec1_typ *eec1,
		int eng_trq_mode,
		float drvr_demand_eng_trq,
		float actual_eng_trq,
		float eng_spd,
		float eng_demand_trq,
		int src_address) {
	return (eec1->eng_trq_mode == eng_trq_mode &&
			eec1->drvr_demand_eng_trq == drvr_demand_eng_trq &&
			eec1->actual_eng_trq == actual_eng_trq &&
			eec1->eng_spd == eng_spd &&
			eec1->eng_demand_trq == eng_demand_trq &&
			eec1->src_address == src_address);
}

bool check_eec2(
		j1939_eec2_typ *eec2,
		int accel_pedal1_idle,
		int accel_pedal_kickdown,
		int spd_limit_status,
		int accel_pedal2_idle,
		float accel_pedal1_pos,
		float eng_prcnt_load_curr_spd,
		float accel_pedal2_pos,
		float act_max_avail_eng_trq) {
	return (eec2->accel_pedal1_idle == accel_pedal1_idle &&
			eec2->accel_pedal_kickdown == accel_pedal_kickdown &&
			eec2->spd_limit_status == spd_limit_status &&
			eec2->accel_pedal2_idle == accel_pedal2_idle &&
			eec2->accel_pedal1_pos == accel_pedal1_pos &&
			eec2->eng_prcnt_load_curr_spd == eng_prcnt_load_curr_spd &&
			eec2->accel_pedal2_pos == accel_pedal2_pos &&
			eec2->act_max_avail_eng_trq == act_max_avail_eng_trq);
}

bool check_eec3(
		j1939_eec3_typ *eec3,
		float nominal_friction,
		float desired_operating_spd,
		int operating_spd_adjust,
		float est_eng_prstic_loss) {
	return (eec3->nominal_friction == nominal_friction &&
			eec3->desired_operating_spd == desired_operating_spd &&
			eec3->operating_spd_adjust == operating_spd_adjust &&
			eec3->est_eng_prstic_loss == est_eng_prstic_loss);
}

bool check_etc1(
		j1939_etc1_typ *etc1,
		int trans_driveline,
		int trq_conv_lockup,
		int trans_shift,
		float tran_output_shaft_spd,
		float prcnt_clutch_slip,
		int eng_overspd_enable,
		int prog_shift_disable,
		float trans_input_shaft_spd,
		int src_address_ctrl) {
	return (etc1->trans_driveline == trans_driveline &&
			etc1->trq_conv_lockup == trq_conv_lockup &&
			etc1->trans_shift == trans_shift &&
			etc1->tran_output_shaft_spd == tran_output_shaft_spd &&
			etc1->prcnt_clutch_slip == prcnt_clutch_slip &&
			etc1->eng_overspd_enable == eng_overspd_enable &&
			etc1->prog_shift_disable == prog_shift_disable &&
			etc1->trans_input_shaft_spd == trans_input_shaft_spd &&
			etc1->src_address_ctrl == src_address_ctrl);
}

bool check_etc2(
		j1939_etc2_typ *etc2,
		int trans_selected_gear,
		float trans_act_gear_ratio,
		int trans_current_gear,
		int range_selected,
		int range_attained) {
	return (etc2->trans_selected_gear == trans_selected_gear &&
			etc2->trans_act_gear_ratio == trans_act_gear_ratio &&
			etc2->trans_current_gear == trans_current_gear &&
			etc2->range_selected == range_selected &&
			etc2->range_attained == range_attained);
}

bool check_erc1(
		j1939_erc1_typ *erc1,
		int trq_mode,
		int enable_brake_assist,
		int enable_shift_assist,
		float actual_ret_pcnt_trq,
		float intended_ret_pcnt_trq,
		int rq_brake_light,
		int src_address_ctrl,
		int drvrs_demand_prcnt_trq,
		float selection_nonengine,
		int max_available_prcnt_trq) {
	return (erc1->trq_mode == trq_mode &&
			erc1->enable_brake_assist == enable_brake_assist &&
			erc1->enable_shift_assist == enable_shift_assist &&
			erc1->actual_ret_pcnt_trq == actual_ret_pcnt_trq &&
			erc1->intended_ret_pcnt_trq == intended_ret_pcnt_trq &&
			erc1->rq_brake_light == rq_brake_light &&
			erc1->src_address_ctrl == src_address_ctrl &&
			erc1->drvrs_demand_prcnt_trq == drvrs_demand_prcnt_trq &&
			erc1->selection_nonengine == selection_nonengine &&
			erc1->max_available_prcnt_trq == max_available_prcnt_trq);
}

bool check_tf(
		j1939_tf_typ *tf,
		float clutch_pressure,
		float oil_level,
		float diff_pressure,
		float oil_pressure,
		float oil_temp) {
	return (tf->clutch_pressure == clutch_pressure &&
			tf->oil_level == oil_level &&
			tf->diff_pressure == diff_pressure &&
			tf->oil_pressure == oil_pressure &&
			tf->oil_temp == oil_temp);
}

bool check_ccvs(
		j1939_ccvs_typ *ccvs,
		int two_spd_axle_switch,
		int parking_brk_switch,
		int cc_pause_switch,
		int park_brk_release,
		float vehicle_spd,
		int cc_active,
		int cc_enable_switch,
		int brk_switch,
		int clutch_switch,
		int cc_set_switch,
		int cc_coast_switch,
		int cc_resume_switch,
		int cc_accel_switch,
		float cc_set_speed,
		int pto_state,
		int cc_state,
		int eng_idle_incr_switch,
		int eng_idle_decr_switch,
		int eng_test_mode_switch,
		int eng_shutdown_override) {
	return (ccvs->two_spd_axle_switch == two_spd_axle_switch &&
			ccvs->parking_brk_switch == parking_brk_switch &&
			ccvs->cc_pause_switch == cc_pause_switch &&
			ccvs->park_brk_release == park_brk_release &&
			ccvs->vehicle_spd == vehicle_spd &&
			ccvs->cc_active == cc_active &&
			ccvs->cc_enable_switch == cc_enable_switch &&
			ccvs->brk_switch == brk_switch &&
			ccvs->clutch_switch == clutch_switch &&
			ccvs->cc_set_switch == cc_set_switch &&
			ccvs->cc_coast_switch == cc_coast_switch &&
			ccvs->cc_resume_switch == cc_resume_switch &&
			ccvs->cc_accel_switch == cc_accel_switch &&
			ccvs->cc_set_speed == cc_set_speed &&
			ccvs->pto_state == pto_state &&
			ccvs->cc_state == cc_state &&
			ccvs->eng_idle_incr_switch == eng_idle_incr_switch &&
			ccvs->eng_idle_decr_switch == eng_idle_decr_switch &&
			ccvs->eng_test_mode_switch == eng_test_mode_switch &&
			ccvs->eng_shutdown_override == eng_shutdown_override);
}

bool check_lfe(
		j1939_lfe_typ *lfe,
		float eng_fuel_rate,
		float eng_inst_fuel_economy,
		float eng_avg_fuel_economy,
		float eng_throttle1_pos,
		float eng_throttle2_pos) {
	return (lfe->eng_fuel_rate == eng_fuel_rate &&
			lfe->eng_inst_fuel_economy == eng_inst_fuel_economy &&
			lfe->eng_avg_fuel_economy == eng_avg_fuel_economy &&
			lfe->eng_throttle1_pos == eng_throttle1_pos &&
			lfe->eng_throttle2_pos == eng_throttle2_pos);
}

bool check_rf(
		j1939_rf_typ *rf,
		float pressure,
		float oil_temp) {
	return (rf->pressure == pressure &&
			rf->oil_temp == oil_temp);
}

bool check_turbo(
		j1939_turbo_typ *turbo,
		float turbo_lube_oil_pressure,
		float turbo_speed) {
	return (turbo->turbo_lube_oil_pressure == turbo_lube_oil_pressure &&
			turbo->turbo_speed == turbo_speed);
}

bool check_vd(
		j1939_vd_typ *vd,
		float trip_dist,
		float tot_vehicle_dist) {
	return (vd->trip_dist == trip_dist &&
			vd->tot_vehicle_dist == tot_vehicle_dist);
}

bool check_rcfg(
		j1939_rcfg_typ *rcfg,
		int retarder_type,
		int retarder_loc,
		int retarder_ctrl_steps,
		float retarder_speed[5],
		float percent_torque[5],
		float reference_retarder_trq) {
	// check the lists
	for (int i=0; i<5; i++) {
		if (rcfg->retarder_speed[i] != retarder_speed[i] ||
				rcfg->percent_torque[i] != percent_torque[i])
			return false;
	}
	return (rcfg->retarder_type == retarder_type &&
			rcfg->retarder_loc == retarder_loc &&
			rcfg->retarder_ctrl_steps == retarder_ctrl_steps &&
			rcfg->reference_retarder_trq == reference_retarder_trq);
}

bool check_ecfg(
		j1939_ecfg_typ *ecfg,
		float engine_spd[7],
		float percent_trq[5],
		float gain_endspeed_governor,
		float reference_eng_trq,
		float max_momentary_overide_time,
		float spd_ctrl_lower_lim,
		float spd_ctrl_upper_lim,
		float trq_ctrl_lower_lim,
		float trq_ctrl_upper_lim,
		int receive_status) {
	// check the lists
	for (int i=0; i<5; i++) {
		if (ecfg->percent_trq[i] != percent_trq[i])
			return false;
	}
	for (int i=0; i<7; i++) {
		if (ecfg->engine_spd[i] != engine_spd[i])
			return false;
	}
	return (ecfg->gain_endspeed_governor == gain_endspeed_governor &&
			ecfg->reference_eng_trq == reference_eng_trq &&
			ecfg->max_momentary_overide_time == max_momentary_overide_time &&
			ecfg->spd_ctrl_lower_lim == spd_ctrl_lower_lim &&
			ecfg->spd_ctrl_upper_lim == spd_ctrl_upper_lim &&
			ecfg->trq_ctrl_lower_lim == trq_ctrl_lower_lim &&
			ecfg->trq_ctrl_upper_lim == trq_ctrl_upper_lim &&
			ecfg->receive_status == receive_status);
}

bool check_etemp(
		j1939_etemp_typ *etemp,
		float eng_coolant_temp,
		float fuel_temp,
		float eng_oil_temp,
		float turbo_oil_temp,
		float eng_intercooler_temp,
		float eng_intercooler_thermostat_opening) {
	return (etemp->eng_coolant_temp == eng_coolant_temp &&
			etemp->fuel_temp == fuel_temp &&
			etemp->eng_oil_temp == eng_oil_temp &&
			etemp->turbo_oil_temp == turbo_oil_temp &&
			etemp->eng_intercooler_temp == eng_intercooler_temp &&
			etemp->eng_intercooler_thermostat_opening == eng_intercooler_thermostat_opening);
}

bool check_pto(
		j1939_pto_typ *pto,
		float oil_temp,
		float speed,
		float set_speed,
		int enable_switch,
		int remote_preprogramm_status,
		int remote_variable_spd_status,
		int set_switch,
		int coast_decel_switch,
		int resume_switch,
		int accel_switch) {
	return (pto->oil_temp == oil_temp &&
			pto->speed == speed &&
			pto->set_speed == set_speed &&
			pto->enable_switch == enable_switch &&
			pto->remote_preprogramm_status == remote_preprogramm_status &&
			pto->remote_variable_spd_status == remote_variable_spd_status &&
			pto->set_switch == set_switch &&
			pto->coast_decel_switch == coast_decel_switch &&
			pto->resume_switch == resume_switch &&
			pto->accel_switch == accel_switch);
}

bool check_ambc(
		j1939_ambc_typ *ambc,
		float barometric_pressure,
		float cab_interior_temp,
		float ambient_air_temp,
		float air_inlet_temp,
		float road_surface_temp) {
	return (ambc->barometric_pressure == barometric_pressure &&
			ambc->cab_interior_temp == cab_interior_temp &&
			ambc->ambient_air_temp == ambient_air_temp &&
			ambc->air_inlet_temp == air_inlet_temp &&
			ambc->road_surface_temp == road_surface_temp);
}

bool check_iec(
		j1939_iec_typ *iec,
		float particulate_inlet_pressure,
		float boost_pressure,
		float intake_manifold_temp,
		float air_inlet_pressure,
		float air_filter_diff_pressure,
		float exhaust_gas_temp,
		float coolant_filter_diff_pressure) {
	return (iec->particulate_inlet_pressure == particulate_inlet_pressure &&
			iec->boost_pressure == boost_pressure &&
			iec->intake_manifold_temp == intake_manifold_temp &&
			iec->air_inlet_pressure == air_inlet_pressure &&
			iec->air_filter_diff_pressure == air_filter_diff_pressure &&
			iec->exhaust_gas_temp == exhaust_gas_temp &&
			iec->coolant_filter_diff_pressure == coolant_filter_diff_pressure);
}

bool check_vep(
		j1939_vep_typ *vep,
		float net_battery_current,
		float alternator_current,
		float alternator_potential,
		float electrical_potential,
		float battery_potential) {
	return (vep->net_battery_current == net_battery_current &&
			vep->alternator_current == alternator_current &&
			vep->alternator_potential == alternator_potential &&
			vep->electrical_potential == electrical_potential &&
			vep->battery_potential == battery_potential);
}

bool check_hrvd(
		j1939_hrvd_typ *hrvd,
		float vehicle_distance,
		float trip_distance) {
	return (hrvd->vehicle_distance == vehicle_distance &&
			hrvd->trip_distance == trip_distance);
}

bool check_fd(
		j1939_fd_typ *fd,
		float prcnt_fan_spd,
		int fan_drive_state) {
	return (fd->prcnt_fan_spd == prcnt_fan_spd &&
			fd->fan_drive_state == fan_drive_state);
}

bool check_gfi2(
		j1939_gfi2_typ *gfi2,
		float fuel_flow_rate1,
		float fuel_flow_rate2,
		float fuel_valve_pos1,
		float fuel_valve_pos2) {
	return (gfi2->fuel_flow_rate1 == fuel_flow_rate1 &&
			gfi2->fuel_flow_rate2 == fuel_flow_rate2 &&
			gfi2->fuel_valve_pos1 == fuel_valve_pos1 &&
			gfi2->fuel_valve_pos2 == fuel_valve_pos2);
}

bool check_ei(
		j1939_ei_typ *ei,
		float pre_filter_oil_pressure,
		float exhaust_gas_pressure,
		float rack_position,
		float eng_gas_mass_flow,
		float inst_estimated_brake_power) {
	return (ei->pre_filter_oil_pressure == pre_filter_oil_pressure &&
			ei->exhaust_gas_pressure == exhaust_gas_pressure &&
			ei->rack_position == rack_position &&
			ei->eng_gas_mass_flow == eng_gas_mass_flow &&
			ei->inst_estimated_brake_power == inst_estimated_brake_power);
}
