/**\file
 *
 * utils.h
 *
 * Utility methods for all tests in this folder. These include:
 *
 * - Assertions for custom objects such as J1939 objects.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date June 13, 2019
 */
#include "jbus/j1939_utils.h"
#include "jbus/j1939_interpreters.h"


#ifndef TESTS_UTILS_H_
#define TESTS_UTILS_H_

/** Validate the values of a timestamp object.
 *
 * @param time the tested timestamp
 * @param hour expected hour value
 * @param minute expected minute value
 * @param second expected second value
 * @param millisecond expected millisecond value
 * @return true if the values match, false otherwise
 */
extern bool check_timestamp_equal(
		timestamp_t time, int hour, int minute, int second, int millisecond);

extern bool check_pdu(
		j1939_pdu_typ *pdu,
		int priority,
		int pdu_format,
		int pdu_specific,
		int src_address,
		int data_field[8],
		int num_bytes);

extern bool check_tsc1(
		j1939_tsc1_typ *tsc1,
		int ovrd_ctrl_m,
		int req_spd_ctrl,
		int ovrd_ctrl_m_pr,
		float req_spd_lim,
		float req_trq_lim,
		int destination_address,
		int src_address);

extern bool check_ebc1(
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
		float total_brk_demand);

extern bool check_ebc2(
		j1939_ebc2_typ *ebc2,
		float front_axle_spd,
		float rel_spd_front_left,
		float rel_spd_front_right,
		float rel_spd_rear_left_1,
		float rel_spd_rear_right_1,
		float rel_spd_rear_left_2,
		float rel_spd_rear_right_2);

extern bool check_eec1(
		j1939_eec1_typ *eec1,
		int eng_trq_mode,
		float drvr_demand_eng_trq,
		float actual_eng_trq,
		float eng_spd,
		float eng_demand_trq,
		int src_address);

extern bool check_eec2(
		j1939_eec2_typ *eec2,
		int accel_pedal1_idle,
		int accel_pedal_kickdown,
		int spd_limit_status,
		int accel_pedal2_idle,
		float accel_pedal1_pos,
		float eng_prcnt_load_curr_spd,
		float accel_pedal2_pos,
		float act_max_avail_eng_trq);

extern bool check_eec3(
		j1939_eec3_typ *eec3,
		float nominal_friction,
		float desired_operating_spd,
		int operating_spd_adjust,
		float est_eng_prstic_loss);

extern bool check_etc1(
		j1939_etc1_typ *etc1,
		int trans_driveline,
		int trq_conv_lockup,
		int trans_shift,
		float tran_output_shaft_spd,
		float prcnt_clutch_slip,
		int eng_overspd_enable,
		int prog_shift_disable,
		float trans_input_shaft_spd,
		int src_address_ctrl);

extern bool check_etc2(
		j1939_etc2_typ *etc2,
		int trans_selected_gear,
		float trans_act_gear_ratio,
		int trans_current_gear,
		int range_selected,
		int range_attained);

extern bool check_erc1(
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
		int max_available_prcnt_trq);

extern bool check_tf(
		j1939_tf_typ *tf,
		float clutch_pressure,
		float oil_level,
		float diff_pressure,
		float oil_pressure,
		float oil_temp);

extern bool check_ccvs(
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
		int eng_shutdown_override);

extern bool check_lfe(
		j1939_lfe_typ *lfe,
		float eng_fuel_rate,
		float eng_inst_fuel_economy,
		float eng_avg_fuel_economy,
		float eng_throttle1_pos,
		float eng_throttle2_pos);

extern bool check_rf(
		j1939_rf_typ *rf,
		float pressure,
		float oil_temp);

extern bool check_turbo(
		j1939_turbo_typ *turbo,
		float turbo_lube_oil_pressure,
		float turbo_speed);

extern bool check_vd(
		j1939_vd_typ *vd,
		float trip_dist,
		float tot_vehicle_dist);

extern bool check_rcfg(
		j1939_rcfg_typ *rcfg,
		int retarder_type,
		int retarder_loc,
		int retarder_ctrl_steps,
		float retarder_speed[5],
		float percent_torque[5],
		float reference_retarder_trq);

extern bool check_ecfg(
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
		int receive_status);

extern bool check_etemp(
		j1939_etemp_typ *etemp,
		float eng_coolant_temp,
		float fuel_temp,
		float eng_oil_temp,
		float turbo_oil_temp,
		float eng_intercooler_temp,
		float eng_intercooler_thermostat_opening);

extern bool check_pto(
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
		int accel_switch);

extern bool check_ambc(
		j1939_ambc_typ *ambc,
		float barometric_pressure,
		float cab_interior_temp,
		float ambient_air_temp,
		float air_inlet_temp,
		float road_surface_temp);

extern bool check_iec(
		j1939_iec_typ *iec,
		float particulate_inlet_pressure,
		float boost_pressure,
		float intake_manifold_temp,
		float air_inlet_pressure,
		float air_filter_diff_pressure,
		float exhaust_gas_temp,
		float coolant_filter_diff_pressure);

extern bool check_vep(
		j1939_vep_typ *vep,
		float net_battery_current,
		float alternator_current,
		float alternator_potential,
		float electrical_potential,
		float battery_potential);

extern bool check_hrvd(
		j1939_hrvd_typ *hrvd,
		float vehicle_distance,
		float trip_distance);

extern bool check_fd(
		j1939_fd_typ *fd,
		float prcnt_fan_spd,
		int fan_drive_state);

extern bool check_gfi2(
		j1939_gfi2_typ *gfi2,
		float fuel_flow_rate1,
		float fuel_flow_rate2,
		float fuel_valve_pos1,
		float fuel_valve_pos2);

extern bool check_ei(
		j1939_ei_typ *ei,
		float pre_filter_oil_pressure,
		float exhaust_gas_pressure,
		float rack_position,
		float eng_gas_mass_flow,
		float inst_estimated_brake_power);

#endif /* TESTS_UTILS_H_ */
