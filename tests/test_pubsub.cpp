/**\file
 *
 * test_pubsub.cpp
 *
 * Unit tests for publish/subscribe methods in include/utils/[pub_sub.h,
 * publish.cpp, subscribe.cpp].
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date June 13, 2019
 */
#define BOOST_TEST_MODULE "test_pubsub"
#include <boost/test/unit_test.hpp>
#include "jbus/j1939_utils.h"
#include "utils/pub_sub.h"
#include "tests/tests.h"

BOOST_AUTO_TEST_SUITE( test_PubSub )

BOOST_AUTO_TEST_CASE( test_publishing_subscribing )
{
	PubSub *ps = new PubSub();
	map <int, void*> res;

	/* ---------------------------------------------------------------------- */
	/* -------------------------------- PDU --------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(PDU);

	/* publish */
	j1939_pdu_typ *pdu_published = new j1939_pdu_typ();
	pdu_published->timestamp.hour = 23;
	pdu_published->timestamp.minute = 59;
	pdu_published->timestamp.second = 59;
	pdu_published->timestamp.millisecond = 999;
	pdu_published->priority = 0;
	pdu_published->pdu_format = 1;
	pdu_published->pdu_specific = 2;
	pdu_published->src_address = 3;
	for (int i=0; i<8; ++i)
		pdu_published->data_field[i] = 4+i;
	pdu_published->num_bytes = 8;
	ps->publish(PDU, pdu_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_pdu_typ *pdu_subscribed = (j1939_pdu_typ*) res[PDU];
	BOOST_CHECK(check_pdu(
			pdu_published,
			pdu_subscribed->priority,
			pdu_subscribed->pdu_format,
			pdu_subscribed->pdu_specific,
			pdu_subscribed->src_address,
			pdu_subscribed->data_field,
			pdu_subscribed->num_bytes
	));

	ps->unsubscribe(PDU);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- TSC1 -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(TSC1);

	/* publish */
	j1939_tsc1_typ *tsc1_published = new j1939_tsc1_typ();
	tsc1_published->timestamp.hour = 23;
	tsc1_published->timestamp.minute = 59;
	tsc1_published->timestamp.second = 59;
	tsc1_published->timestamp.millisecond = 999;
	tsc1_published->ovrd_ctrl_m = 0;
	tsc1_published->req_spd_ctrl = 1;
	tsc1_published->ovrd_ctrl_m_pr = 2;
	tsc1_published->req_spd_lim = 3;
	tsc1_published->req_trq_lim = 4;
	tsc1_published->destination_address = 5;
	tsc1_published->src_address = 6;
	ps->publish(TSC1, tsc1_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_tsc1_typ *tsc1_subscribed = (j1939_tsc1_typ*) res[TSC1];
	BOOST_CHECK(check_tsc1(
			tsc1_published,
			tsc1_subscribed->ovrd_ctrl_m,
			tsc1_subscribed->req_spd_ctrl,
			tsc1_subscribed->ovrd_ctrl_m_pr,
			tsc1_subscribed->req_spd_lim,
			tsc1_subscribed->req_trq_lim,
			tsc1_subscribed->destination_address,
			tsc1_subscribed->src_address
	));

	ps->unsubscribe(TSC1);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- EBC1 -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(EBC1);

	/* publish */
	j1939_ebc1_typ *ebc1_published = new j1939_ebc1_typ();
	ebc1_published->timestamp.hour = 23;
	ebc1_published->timestamp.minute = 59;
	ebc1_published->timestamp.second = 59;
	ebc1_published->timestamp.millisecond = 999;
	ebc1_published->asr_engine_ctrl_active = 0;
	ebc1_published->asr_brk_ctrl_active = 1;
	ebc1_published->antilock_brk_active = 2;
	ebc1_published->ebs_brk_switch = 3;
	ebc1_published->brk_pedal_pos = 4;
	ebc1_published->abs_offroad_switch = 5;
	ebc1_published->asr_offroad_switch = 6;
	ebc1_published->asr_hillholder_switch = 7;
	ebc1_published->trac_ctrl_override_switch = 8;
	ebc1_published->accel_interlock_switch = 9;
	ebc1_published->eng_derate_switch = 10;
	ebc1_published->aux_eng_shutdown_switch = 11;
	ebc1_published->accel_enable_switch = 12;
	ebc1_published->eng_retarder_selection = 13;
	ebc1_published->abs_fully_operational = 14;
	ebc1_published->ebs_red_warning = 15;
	ebc1_published->abs_ebs_amber_warning = 16;
	ebc1_published->src_address_ctrl = 17;
	ebc1_published->total_brk_demand = 18;
	ps->publish(EBC1, ebc1_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_ebc1_typ *ebc1_subscribed = (j1939_ebc1_typ*) res[EBC1];
	BOOST_CHECK(check_ebc1(
			ebc1_published,
			ebc1_subscribed->asr_engine_ctrl_active,
			ebc1_subscribed->asr_brk_ctrl_active,
			ebc1_subscribed->antilock_brk_active,
			ebc1_subscribed->ebs_brk_switch,
			ebc1_subscribed->brk_pedal_pos,
			ebc1_subscribed->abs_offroad_switch,
			ebc1_subscribed->asr_offroad_switch,
			ebc1_subscribed->asr_hillholder_switch,
			ebc1_subscribed->trac_ctrl_override_switch,
			ebc1_subscribed->accel_interlock_switch,
			ebc1_subscribed->eng_derate_switch,
			ebc1_subscribed->aux_eng_shutdown_switch,
			ebc1_subscribed->accel_enable_switch,
			ebc1_subscribed->eng_retarder_selection,
			ebc1_subscribed->abs_fully_operational,
			ebc1_subscribed->ebs_red_warning,
			ebc1_subscribed->abs_ebs_amber_warning,
			ebc1_subscribed->src_address_ctrl,
			ebc1_subscribed->total_brk_demand
	));

	ps->unsubscribe(EBC1);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- EBC2 -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(EBC2);

	/* publish */
	j1939_ebc2_typ *ebc2_published = new j1939_ebc2_typ();
	ebc2_published->timestamp.hour = 23;
	ebc2_published->timestamp.minute = 59;
	ebc2_published->timestamp.second = 59;
	ebc2_published->timestamp.millisecond = 999;
	ebc2_published->front_axle_spd = 0;
	ebc2_published->rel_spd_front_left = 1;
	ebc2_published->rel_spd_front_right = 2;
	ebc2_published->rel_spd_rear_left_1 = 3;
	ebc2_published->rel_spd_rear_right_1 = 4;
	ebc2_published->rel_spd_rear_left_2 = 5;
	ebc2_published->rel_spd_rear_right_2 = 6;
	ps->publish(EBC2, ebc2_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_ebc2_typ *ebc2_subscribed = (j1939_ebc2_typ*) res[EBC2];
	BOOST_CHECK(check_ebc2(
			ebc2_published,
			ebc2_subscribed->front_axle_spd,
			ebc2_subscribed->rel_spd_front_left,
			ebc2_subscribed->rel_spd_front_right,
			ebc2_subscribed->rel_spd_rear_left_1,
			ebc2_subscribed->rel_spd_rear_right_1,
			ebc2_subscribed->rel_spd_rear_left_2,
			ebc2_subscribed->rel_spd_rear_right_2
	));

	ps->unsubscribe(EBC2);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- EEC1 -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(EEC1);

	/* publish */
	j1939_eec1_typ *eec1_published = new j1939_eec1_typ();
	eec1_published->timestamp.hour = 23;
	eec1_published->timestamp.minute = 59;
	eec1_published->timestamp.second = 59;
	eec1_published->timestamp.millisecond = 999;
	eec1_published->eng_trq_mode = 0;
	eec1_published->drvr_demand_eng_trq = 1;
	eec1_published->actual_eng_trq = 2;
	eec1_published->eng_spd = 3;
	eec1_published->eng_demand_trq = 4;
	eec1_published->src_address = 5;
	ps->publish(EEC1, eec1_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_eec1_typ *eec1_subscribed = (j1939_eec1_typ*) res[EEC1];
	BOOST_CHECK(check_eec1(
			eec1_published,
			eec1_subscribed->eng_trq_mode,
			eec1_subscribed->drvr_demand_eng_trq,
			eec1_subscribed->actual_eng_trq,
			eec1_subscribed->eng_spd,
			eec1_subscribed->eng_demand_trq,
			eec1_subscribed->src_address
	));

	ps->unsubscribe(EEC1);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- EEC2 -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(EEC2);

	/* publish */
	j1939_eec2_typ *eec2_published = new j1939_eec2_typ();
	eec2_published->timestamp.hour = 23;
	eec2_published->timestamp.minute = 59;
	eec2_published->timestamp.second = 59;
	eec2_published->timestamp.millisecond = 999;
	eec2_published->accel_pedal1_idle = 0;
	eec2_published->accel_pedal_kickdown = 1;
	eec2_published->spd_limit_status = 2;
	eec2_published->accel_pedal2_idle = 3;
	eec2_published->accel_pedal1_pos = 4;
	eec2_published->eng_prcnt_load_curr_spd = 5;
	eec2_published->accel_pedal2_pos = 6;
	eec2_published->act_max_avail_eng_trq = 7;
	ps->publish(EEC2, eec2_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_eec2_typ *eec2_subscribed = (j1939_eec2_typ*) res[EEC2];
	BOOST_CHECK(check_eec2(
			eec2_published,
			eec2_subscribed->accel_pedal1_idle,
			eec2_subscribed->accel_pedal_kickdown,
			eec2_subscribed->spd_limit_status,
			eec2_subscribed->accel_pedal2_idle,
			eec2_subscribed->accel_pedal1_pos,
			eec2_subscribed->eng_prcnt_load_curr_spd,
			eec2_subscribed->accel_pedal2_pos,
			eec2_subscribed->act_max_avail_eng_trq
	));

	ps->unsubscribe(EEC2);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- EEC3 -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(EEC3);

	/* publish */
	j1939_eec3_typ *eec3_published = new j1939_eec3_typ();
	eec3_published->timestamp.hour = 23;
	eec3_published->timestamp.minute = 59;
	eec3_published->timestamp.second = 59;
	eec3_published->timestamp.millisecond = 999;
	eec3_published->nominal_friction = 0;
	eec3_published->desired_operating_spd = 1;
	eec3_published->operating_spd_adjust = 2;
	eec3_published->est_eng_prstic_loss = 3;
	ps->publish(EEC3, eec3_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_eec3_typ *eec3_subscribed = (j1939_eec3_typ*) res[EEC3];
	BOOST_CHECK(check_eec3(
			eec3_published,
			eec3_subscribed->nominal_friction,
			eec3_subscribed->desired_operating_spd,
			eec3_subscribed->operating_spd_adjust,
			eec3_subscribed->est_eng_prstic_loss
	));

	ps->unsubscribe(EEC3);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- ETC1 -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(ETC1);

	/* publish */
	j1939_etc1_typ *etc1_published = new j1939_etc1_typ();
	etc1_published->timestamp.hour = 23;
	etc1_published->timestamp.minute = 59;
	etc1_published->timestamp.second = 59;
	etc1_published->timestamp.millisecond = 999;
	etc1_published->trans_driveline = 0;
	etc1_published->trq_conv_lockup = 1;
	etc1_published->trans_shift = 2;
	etc1_published->tran_output_shaft_spd = 3;
	etc1_published->prcnt_clutch_slip = 4;
	etc1_published->eng_overspd_enable = 5;
	etc1_published->prog_shift_disable = 6;
	etc1_published->trans_input_shaft_spd = 7;
	etc1_published->src_address_ctrl = 8;
	ps->publish(ETC1, etc1_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_etc1_typ *etc1_subscribed = (j1939_etc1_typ*) res[ETC1];
	BOOST_CHECK(check_etc1(
			etc1_published,
			etc1_subscribed->trans_driveline,
			etc1_subscribed->trq_conv_lockup,
			etc1_subscribed->trans_shift,
			etc1_subscribed->tran_output_shaft_spd,
			etc1_subscribed->prcnt_clutch_slip,
			etc1_subscribed->eng_overspd_enable,
			etc1_subscribed->prog_shift_disable,
			etc1_subscribed->trans_input_shaft_spd,
			etc1_subscribed->src_address_ctrl
	));

	ps->unsubscribe(ETC1);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- ETC2 -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(ETC2);

	/* publish */
	j1939_etc2_typ *etc2_published = new j1939_etc2_typ();
	etc2_published->timestamp.hour = 23;
	etc2_published->timestamp.minute = 59;
	etc2_published->timestamp.second = 59;
	etc2_published->timestamp.millisecond = 999;
	etc2_published->trans_selected_gear = 0;
	etc2_published->trans_act_gear_ratio = 1;
	etc2_published->trans_current_gear = 2;
	etc2_published->range_selected = 3;
	etc2_published->range_attained = 4;
	ps->publish(ETC2, etc2_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_etc2_typ *etc2_subscribed = (j1939_etc2_typ*) res[ETC2];
	BOOST_CHECK(check_etc2(
			etc2_published,
			etc2_subscribed->trans_selected_gear,
			etc2_subscribed->trans_act_gear_ratio,
			etc2_subscribed->trans_current_gear,
			etc2_subscribed->range_selected,
			etc2_subscribed->range_attained
	));

	ps->unsubscribe(ETC2);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- ERC1 -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(ERC1);

	/* publish */
	j1939_erc1_typ *erc1_published = new j1939_erc1_typ();
	erc1_published->timestamp.hour = 23;
	erc1_published->timestamp.minute = 59;
	erc1_published->timestamp.second = 59;
	erc1_published->timestamp.millisecond = 999;
	erc1_published->trq_mode = 0;
	erc1_published->enable_brake_assist = 1;
	erc1_published->enable_shift_assist = 2;
	erc1_published->actual_ret_pcnt_trq = 3;
	erc1_published->intended_ret_pcnt_trq = 4;
	erc1_published->rq_brake_light = 5;
	erc1_published->src_address_ctrl = 6;
	erc1_published->drvrs_demand_prcnt_trq = 7;
	erc1_published->selection_nonengine = 8;
	erc1_published->max_available_prcnt_trq = 9;
	ps->publish(ERC1, erc1_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_erc1_typ *erc1_subscribed = (j1939_erc1_typ*) res[ERC1];
	BOOST_CHECK(check_erc1(
			erc1_published,
			erc1_subscribed->trq_mode,
			erc1_subscribed->enable_brake_assist,
			erc1_subscribed->enable_shift_assist,
			erc1_subscribed->actual_ret_pcnt_trq,
			erc1_subscribed->intended_ret_pcnt_trq,
			erc1_subscribed->rq_brake_light,
			erc1_subscribed->src_address_ctrl,
			erc1_subscribed->drvrs_demand_prcnt_trq,
			erc1_subscribed->selection_nonengine,
			erc1_subscribed->max_available_prcnt_trq
	));

	ps->unsubscribe(ERC1);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* --------------------------------- TF --------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(TF);

	/* publish */
	j1939_tf_typ *tf_published = new j1939_tf_typ();
	tf_published->timestamp.hour = 23;
	tf_published->timestamp.minute = 59;
	tf_published->timestamp.second = 59;
	tf_published->timestamp.millisecond = 999;
	tf_published->clutch_pressure = 0;
	tf_published->oil_level = 1;
	tf_published->diff_pressure = 2;
	tf_published->oil_pressure = 3;
	tf_published->oil_temp = 4;
	ps->publish(TF, tf_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_tf_typ *tf_subscribed = (j1939_tf_typ*) res[TF];
	BOOST_CHECK(check_tf(
			tf_published,
			tf_subscribed->clutch_pressure,
			tf_subscribed->oil_level,
			tf_subscribed->diff_pressure,
			tf_subscribed->oil_pressure,
			tf_subscribed->oil_temp
	));

	ps->unsubscribe(TF);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- CCVS -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(CCVS);

	/* publish */
	j1939_ccvs_typ *ccvs_published = new j1939_ccvs_typ();
	ccvs_published->timestamp.hour = 23;
	ccvs_published->timestamp.minute = 59;
	ccvs_published->timestamp.second = 59;
	ccvs_published->timestamp.millisecond = 999;
	ccvs_published->two_spd_axle_switch = 0;
	ccvs_published->parking_brk_switch = 1;
	ccvs_published->cc_pause_switch = 2;
	ccvs_published->park_brk_release = 3;
	ccvs_published->vehicle_spd = 4;
	ccvs_published->cc_active = 5;
	ccvs_published->cc_enable_switch = 6;
	ccvs_published->brk_switch = 7;
	ccvs_published->clutch_switch = 8;
	ccvs_published->cc_set_switch = 9;
	ccvs_published->cc_coast_switch = 10;
	ccvs_published->cc_resume_switch = 11;
	ccvs_published->cc_accel_switch = 12;
	ccvs_published->cc_set_speed = 13;
	ccvs_published->pto_state = 14;
	ccvs_published->cc_state = 15;
	ccvs_published->eng_idle_incr_switch = 16;
	ccvs_published->eng_idle_decr_switch = 17;
	ccvs_published->eng_test_mode_switch = 18;
	ccvs_published->eng_shutdown_override = 19;
	ps->publish(CCVS, ccvs_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_ccvs_typ *ccvs_subscribed = (j1939_ccvs_typ*) res[CCVS];
	BOOST_CHECK(check_ccvs(
			ccvs_published,
			ccvs_subscribed->two_spd_axle_switch,
			ccvs_subscribed->parking_brk_switch,
			ccvs_subscribed->cc_pause_switch,
			ccvs_subscribed->park_brk_release,
			ccvs_subscribed->vehicle_spd,
			ccvs_subscribed->cc_active,
			ccvs_subscribed->cc_enable_switch,
			ccvs_subscribed->brk_switch,
			ccvs_subscribed->clutch_switch,
			ccvs_subscribed->cc_set_switch,
			ccvs_subscribed->cc_coast_switch,
			ccvs_subscribed->cc_resume_switch,
			ccvs_subscribed->cc_accel_switch,
			ccvs_subscribed->cc_set_speed,
			ccvs_subscribed->pto_state,
			ccvs_subscribed->cc_state,
			ccvs_subscribed->eng_idle_incr_switch,
			ccvs_subscribed->eng_idle_decr_switch,
			ccvs_subscribed->eng_test_mode_switch,
			ccvs_subscribed->eng_shutdown_override
	));

	ps->unsubscribe(CCVS);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- LFE --------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(LFE);

	/* publish */
	j1939_lfe_typ *lfe_published = new j1939_lfe_typ();
	lfe_published->timestamp.hour = 23;
	lfe_published->timestamp.minute = 59;
	lfe_published->timestamp.second = 59;
	lfe_published->timestamp.millisecond = 999;
	lfe_published->eng_fuel_rate = 0;
	lfe_published->eng_inst_fuel_economy = 1;
	lfe_published->eng_avg_fuel_economy = 2;
	lfe_published->eng_throttle1_pos = 3;
	lfe_published->eng_throttle2_pos = 4;
	ps->publish(LFE, lfe_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_lfe_typ *lfe_subscribed = (j1939_lfe_typ*) res[LFE];
	BOOST_CHECK(check_lfe(
			lfe_published,
			lfe_subscribed->eng_fuel_rate,
			lfe_subscribed->eng_inst_fuel_economy,
			lfe_subscribed->eng_avg_fuel_economy,
			lfe_subscribed->eng_throttle1_pos,
			lfe_subscribed->eng_throttle2_pos
	));

	ps->unsubscribe(LFE);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* --------------------------------- RF --------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(RF);

	/* publish */
	j1939_rf_typ *rf_published = new j1939_rf_typ();
	rf_published->timestamp.hour = 23;
	rf_published->timestamp.minute = 59;
	rf_published->timestamp.second = 59;
	rf_published->timestamp.millisecond = 999;
	rf_published->pressure = 0;
	rf_published->oil_temp = 1;
	ps->publish(RF, rf_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_rf_typ *rf_subscribed = (j1939_rf_typ*) res[RF];
	BOOST_CHECK(check_rf(
			rf_published,
			rf_subscribed->pressure,
			rf_subscribed->oil_temp
	));

	ps->unsubscribe(RF);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* ------------------------------- TURBO -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(TURBO);

	/* publish */
	j1939_turbo_typ *turbo_published = new j1939_turbo_typ();
	turbo_published->timestamp.hour = 23;
	turbo_published->timestamp.minute = 59;
	turbo_published->timestamp.second = 59;
	turbo_published->timestamp.millisecond = 999;
	turbo_published->turbo_lube_oil_pressure = 0;
	turbo_published->turbo_speed = 1;
	ps->publish(TURBO, turbo_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_turbo_typ *turbo_subscribed = (j1939_turbo_typ*) res[TURBO];
	BOOST_CHECK(check_turbo(
			turbo_published,
			turbo_subscribed->turbo_lube_oil_pressure,
			turbo_subscribed->turbo_speed
	));

	ps->unsubscribe(TURBO);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* --------------------------------- VD --------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(VD);

	/* publish */
	j1939_vd_typ *vd_published = new j1939_vd_typ();
	vd_published->timestamp.hour = 23;
	vd_published->timestamp.minute = 59;
	vd_published->timestamp.second = 59;
	vd_published->timestamp.millisecond = 999;
	vd_published->trip_dist = 0;
	vd_published->tot_vehicle_dist = 1;
	ps->publish(VD, vd_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_vd_typ *vd_subscribed = (j1939_vd_typ*) res[VD];
	BOOST_CHECK(check_vd(
			vd_published,
			vd_subscribed->trip_dist,
			vd_subscribed->tot_vehicle_dist
	));

	ps->unsubscribe(VD);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- RCFG -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(RCFG);

	/* publish */
	j1939_rcfg_typ *rcfg_published = new j1939_rcfg_typ();
	rcfg_published->timestamp.hour = 23;
	rcfg_published->timestamp.minute = 59;
	rcfg_published->timestamp.second = 59;
	rcfg_published->timestamp.millisecond = 999;
	rcfg_published->retarder_type = 0;
	rcfg_published->retarder_loc = 1;
	rcfg_published->retarder_ctrl_steps = 2;
	for (int i=0; i<5; ++i) {
		rcfg_published->retarder_speed[i] = 3+i;
		rcfg_published->percent_torque[i] = 8+i;
	}
	rcfg_published->reference_retarder_trq = 13;
	ps->publish(RCFG, rcfg_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_rcfg_typ *rcfg_subscribed = (j1939_rcfg_typ*) res[RCFG];
//	BOOST_CHECK(check_rcfg(
//			rcfg_published,
//			rcfg_subscribed->retarder_type,
//			rcfg_subscribed->retarder_loc,
//			rcfg_subscribed->retarder_ctrl_steps,
//			rcfg_subscribed->retarder_speed,
//			rcfg_subscribed->percent_torque,
//			rcfg_subscribed->reference_retarder_trq
//	));

	ps->unsubscribe(RCFG);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- ECFG -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(ECFG);

	/* publish */
	j1939_ecfg_typ *ecfg_published = new j1939_ecfg_typ();
	ecfg_published->timestamp.hour = 23;
	ecfg_published->timestamp.minute = 59;
	ecfg_published->timestamp.second = 59;
	ecfg_published->timestamp.millisecond = 999;
	for (int i=0; i<5; ++i)
		ecfg_published->percent_trq[i] = i;
	for (int i=0; i<7; ++i)
		ecfg_published->engine_spd[i] = 5+i;
	ecfg_published->gain_endspeed_governor = 12;
	ecfg_published->reference_eng_trq = 13;
	ecfg_published->max_momentary_overide_time = 14;
	ecfg_published->spd_ctrl_lower_lim = 15;
	ecfg_published->spd_ctrl_upper_lim = 16;
	ecfg_published->trq_ctrl_lower_lim = 17;
	ecfg_published->trq_ctrl_upper_lim = 18;
	ecfg_published->receive_status = 19;
	ps->publish(ECFG, ecfg_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_ecfg_typ *ecfg_subscribed = (j1939_ecfg_typ*) res[ECFG];
//	BOOST_CHECK(check_ecfg(
//			ecfg_published,
//			ecfg_subscribed->engine_spd,
//			ecfg_subscribed->percent_trq,
//			ecfg_subscribed->gain_endspeed_governor,
//			ecfg_subscribed->reference_eng_trq,
//			ecfg_subscribed->max_momentary_overide_time,
//			ecfg_subscribed->spd_ctrl_lower_lim,
//			ecfg_subscribed->spd_ctrl_upper_lim,
//			ecfg_subscribed->trq_ctrl_lower_lim,
//			ecfg_subscribed->trq_ctrl_upper_lim,
//			ecfg_subscribed->receive_status
//	));

	ps->unsubscribe(ECFG);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* ------------------------------- ETEMP -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(ETEMP);

	/* publish */
	j1939_etemp_typ *etemp_published = new j1939_etemp_typ();
	etemp_published->timestamp.hour = 23;
	etemp_published->timestamp.minute = 59;
	etemp_published->timestamp.second = 59;
	etemp_published->timestamp.millisecond = 999;
	etemp_published->eng_coolant_temp = 0,
	etemp_published->fuel_temp = 1;
	etemp_published->eng_oil_temp = 2;
	etemp_published->turbo_oil_temp = 3;
	etemp_published->eng_intercooler_temp = 4;
	etemp_published->eng_intercooler_thermostat_opening = 5;
	ps->publish(ETEMP, etemp_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_etemp_typ *etemp_subscribed = (j1939_etemp_typ*) res[ETEMP];
	BOOST_CHECK(check_etemp(
			etemp_published,
			etemp_subscribed->eng_coolant_temp,
			etemp_subscribed->fuel_temp,
			etemp_subscribed->eng_oil_temp,
			etemp_subscribed->turbo_oil_temp,
			etemp_subscribed->eng_intercooler_temp,
			etemp_subscribed->eng_intercooler_thermostat_opening
	));

	ps->unsubscribe(ETEMP);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- PTO --------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(PTO);

	/* publish */
	j1939_pto_typ *pto_published = new j1939_pto_typ();
	pto_published->timestamp.hour = 23;
	pto_published->timestamp.minute = 59;
	pto_published->timestamp.second = 59;
	pto_published->timestamp.millisecond = 999;
	pto_published->oil_temp = 0;
	pto_published->speed = 1;
	pto_published->set_speed = 2;
	pto_published->enable_switch = 3;
	pto_published->remote_preprogramm_status = 4;
	pto_published->remote_variable_spd_status = 5;
	pto_published->set_switch = 6;
	pto_published->coast_decel_switch = 7;
	pto_published->resume_switch = 8;
	pto_published->accel_switch = 9;
	ps->publish(PTO, pto_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_pto_typ *pto_subscribed = (j1939_pto_typ*) res[PTO];
	BOOST_CHECK(check_pto(
			pto_published,
			pto_subscribed->oil_temp,
			pto_subscribed->speed,
			pto_subscribed->set_speed,
			pto_subscribed->enable_switch,
			pto_subscribed->remote_preprogramm_status,
			pto_subscribed->remote_variable_spd_status,
			pto_subscribed->set_switch,
			pto_subscribed->coast_decel_switch,
			pto_subscribed->resume_switch,
			pto_subscribed->accel_switch
	));

	ps->unsubscribe(PTO);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- AMBC -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(AMBC);

	/* publish */
	j1939_ambc_typ *ambc_published = new j1939_ambc_typ();
	ambc_published->timestamp.hour = 23;
	ambc_published->timestamp.minute = 59;
	ambc_published->timestamp.second = 59;
	ambc_published->timestamp.millisecond = 999;
	ambc_published->barometric_pressure = 0;
	ambc_published->cab_interior_temp = 1;
	ambc_published->ambient_air_temp = 2;
	ambc_published->air_inlet_temp = 3;
	ambc_published->road_surface_temp = 4;
	ps->publish(AMBC, ambc_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_ambc_typ *ambc_subscribed = (j1939_ambc_typ*) res[AMBC];
	BOOST_CHECK(check_ambc(
			ambc_published,
			ambc_subscribed->barometric_pressure,
			ambc_subscribed->cab_interior_temp,
			ambc_subscribed->ambient_air_temp,
			ambc_subscribed->air_inlet_temp,
			ambc_subscribed->road_surface_temp
	));

	ps->unsubscribe(AMBC);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- IEC --------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(IEC);

	/* publish */
	j1939_iec_typ *iec_published = new j1939_iec_typ();
	iec_published->timestamp.hour = 23;
	iec_published->timestamp.minute = 59;
	iec_published->timestamp.second = 59;
	iec_published->timestamp.millisecond = 999;
	iec_published->particulate_inlet_pressure = 0;
	iec_published->boost_pressure = 1;
	iec_published->intake_manifold_temp = 2;
	iec_published->air_inlet_pressure = 3;
	iec_published->air_filter_diff_pressure = 4;
	iec_published->exhaust_gas_temp = 5;
	iec_published->coolant_filter_diff_pressure = 6;
	ps->publish(IEC, iec_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_iec_typ *iec_subscribed = (j1939_iec_typ*) res[IEC];
	BOOST_CHECK(check_iec(
			iec_published,
			iec_subscribed->particulate_inlet_pressure,
			iec_subscribed->boost_pressure,
			iec_subscribed->intake_manifold_temp,
			iec_subscribed->air_inlet_pressure,
			iec_subscribed->air_filter_diff_pressure,
			iec_subscribed->exhaust_gas_temp,
			iec_subscribed->coolant_filter_diff_pressure
	));

	ps->unsubscribe(IEC);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- VEP --------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(VEP);

	/* publish */
	j1939_vep_typ *vep_published = new j1939_vep_typ();
	vep_published->timestamp.hour = 23;
	vep_published->timestamp.minute = 59;
	vep_published->timestamp.second = 59;
	vep_published->timestamp.millisecond = 999;
	vep_published->net_battery_current = 0;
	vep_published->alternator_current = 1;
	vep_published->alternator_potential = 2;
	vep_published->electrical_potential = 3;
	vep_published->battery_potential = 4;
	ps->publish(VEP, vep_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_vep_typ *vep_subscribed = (j1939_vep_typ*) res[VEP];
	BOOST_CHECK(check_vep(
			vep_published,
			vep_subscribed->net_battery_current,
			vep_subscribed->alternator_current,
			vep_subscribed->alternator_potential,
			vep_subscribed->electrical_potential,
			vep_subscribed->battery_potential
	));

	ps->unsubscribe(VEP);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- HRVD -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(HRVD);

	/* publish */
	j1939_hrvd_typ *hrvd_published = new j1939_hrvd_typ();
	hrvd_published->timestamp.hour = 23;
	hrvd_published->timestamp.minute = 59;
	hrvd_published->timestamp.second = 59;
	hrvd_published->timestamp.millisecond = 999;
	hrvd_published->vehicle_distance = 0;
	hrvd_published->trip_distance = 1;
	ps->publish(HRVD, hrvd_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_hrvd_typ *hrvd_subscribed = (j1939_hrvd_typ*) res[HRVD];
	BOOST_CHECK(check_hrvd(
			hrvd_published,
			hrvd_subscribed->vehicle_distance,
			hrvd_subscribed->trip_distance
	));

	ps->unsubscribe(HRVD);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* --------------------------------- FD --------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(FD);

	/* publish */
	j1939_fd_typ *fd_published = new j1939_fd_typ();
	fd_published->timestamp.hour = 23;
	fd_published->timestamp.minute = 59;
	fd_published->timestamp.second = 59;
	fd_published->timestamp.millisecond = 999;
	fd_published->prcnt_fan_spd = 0;
	fd_published->fan_drive_state = 1;
	ps->publish(FD, fd_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_fd_typ *fd_subscribed = (j1939_fd_typ*) res[FD];
	BOOST_CHECK(check_fd(
			fd_published,
			fd_subscribed->prcnt_fan_spd,
			fd_subscribed->fan_drive_state
	));

	ps->unsubscribe(FD);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* -------------------------------- GFI2 -------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(GFI2);

	/* publish */
	j1939_gfi2_typ *gfi2_published = new j1939_gfi2_typ();
	gfi2_published->timestamp.hour = 23;
	gfi2_published->timestamp.minute = 59;
	gfi2_published->timestamp.second = 59;
	gfi2_published->timestamp.millisecond = 999;
	gfi2_published->fuel_flow_rate1 = 0;
	gfi2_published->fuel_flow_rate2 = 1;
	gfi2_published->fuel_valve_pos1 = 2;
	gfi2_published->fuel_valve_pos2 = 3;
	ps->publish(GFI2, gfi2_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_gfi2_typ *gfi2_subscribed = (j1939_gfi2_typ*) res[GFI2];
	BOOST_CHECK(check_gfi2(
			gfi2_published,
			gfi2_subscribed->fuel_flow_rate1,
			gfi2_subscribed->fuel_flow_rate2,
			gfi2_subscribed->fuel_valve_pos1,
			gfi2_subscribed->fuel_valve_pos2
	));

	ps->unsubscribe(GFI2);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* ---------------------------------------------------------------------- */
	/* --------------------------------- EI --------------------------------- */
	/* ---------------------------------------------------------------------- */
	ps->subscribe(EI);

	/* publish */
	j1939_ei_typ *ei_published = new j1939_ei_typ();
	ei_published->timestamp.hour = 23;
	ei_published->timestamp.minute = 59;
	ei_published->timestamp.second = 59;
	ei_published->timestamp.millisecond = 999;
	ei_published->pre_filter_oil_pressure = 0;
	ei_published->exhaust_gas_pressure = 1;
	ei_published->rack_position = 2;
	ei_published->eng_gas_mass_flow = 3;
	ei_published->inst_estimated_brake_power = 4;
	ps->publish(EI, ei_published);

	/* subscribe */
	res = ps->get_subscription_results();
	j1939_ei_typ *ei_subscribed = (j1939_ei_typ*) res[EI];
	BOOST_CHECK(check_ei(
			ei_published,
			ei_subscribed->pre_filter_oil_pressure,
			ei_subscribed->exhaust_gas_pressure,
			ei_subscribed->rack_position,
			ei_subscribed->eng_gas_mass_flow,
			ei_subscribed->inst_estimated_brake_power
	));

	ps->unsubscribe(EI);
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */
	/* ---------------------------------------------------------------------- */


	/* Clear memory. */

	delete pdu_published;
	delete tsc1_published;
	delete ebc1_published;
	delete ebc2_published;
	delete eec1_published;
	delete eec2_published;
	delete eec3_published;
	delete etc1_published;
	delete etc2_published;
	delete erc1_published;
	delete tf_published;
	delete ccvs_published;
	delete lfe_published;
	delete rf_published;
	delete turbo_published;
	delete vd_published;
	delete rcfg_published;
	delete ecfg_published;
	delete etemp_published;
	delete pto_published;
	delete ambc_published;
	delete iec_published;
	delete vep_published;
	delete hrvd_published;
	delete fd_published;
	delete gfi2_published;
	delete ei_published;

	delete pdu_subscribed;
	delete tsc1_subscribed;
	delete ebc1_subscribed;
	delete ebc2_subscribed;
	delete eec1_subscribed;
	delete eec2_subscribed;
	delete eec3_subscribed;
	delete etc1_subscribed;
	delete etc2_subscribed;
	delete erc1_subscribed;
	delete tf_subscribed;
	delete ccvs_subscribed;
	delete lfe_subscribed;
	delete rf_subscribed;
	delete turbo_subscribed;
	delete vd_subscribed;
	delete rcfg_subscribed;
	delete ecfg_subscribed;
	delete etemp_subscribed;
	delete pto_subscribed;
	delete ambc_subscribed;
	delete iec_subscribed;
	delete vep_subscribed;
	delete hrvd_subscribed;
	delete fd_subscribed;
	delete gfi2_subscribed;
	delete ei_subscribed;
}

BOOST_AUTO_TEST_SUITE_END()
