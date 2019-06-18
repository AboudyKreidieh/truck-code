/**\file
 *
 * test_logger.cpp
 *
 * Tests for the methods in include/logger/[logger.h, logger.cpp] and
 * src/[log.cpp, sql2txt.cpp].
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date June 13, 2019
 */
#define BOOST_TEST_MODULE "test_logger"
#include <boost/test/unit_test.hpp>
#include "logger/logger.h"
#include "jbus/j1939_utils.h"
#include "jbus/j1939_struct.h"
#include <string>
#include <vector>
#include <fstream>


BOOST_AUTO_TEST_SUITE( test_Logger )

BOOST_AUTO_TEST_CASE( test_all )
{
	/* Initialize the database in some fixed path. */
	bool db_append = false;
	string db_path = "/dev/test_logdir";
	vector<int> db_types {
		PDU, TSC1, ERC1, EBC1, EBC2, ETC1, ETC2, EEC1, EEC2, EEC3, GFI2, EI, FD,
		HRVD, TURBO, VD, RCFG, TCFG, ECFG, ETEMP, PTO, CCVS, LFE, AMBC, IEC,
		VEP, TF, RF
	};
	DBManager *db = new DBManager();
	db->init(db_path, db_types, db_append);

	/* Add a message of each type to the QDB database. */

	j1939_pdu_typ *pdu_stored = new j1939_pdu_typ();
	pdu_stored->timestamp.hour = 23;
	pdu_stored->timestamp.minute = 59;
	pdu_stored->timestamp.second = 59;
	pdu_stored->timestamp.millisecond = 999;
	pdu_stored->priority = 0;
	pdu_stored->pdu_format = 1;
	pdu_stored->pdu_specific = 2;
	pdu_stored->src_address = 3;
	for (int i=0; i<8; ++i)
		pdu_stored->data_field[i] = 4+i;
	pdu_stored->num_bytes = 8;
	db->store(PDU, (void*)pdu_stored);

	j1939_tsc1_typ *tsc1_stored = new j1939_tsc1_typ();
	tsc1_stored->timestamp.hour = 23;
	tsc1_stored->timestamp.minute = 59;
	tsc1_stored->timestamp.second = 59;
	tsc1_stored->timestamp.millisecond = 999;
	tsc1_stored->ovrd_ctrl_m = 0;
	tsc1_stored->req_spd_ctrl = 1;
	tsc1_stored->ovrd_ctrl_m_pr = 2;
	tsc1_stored->req_spd_lim = 3;
	tsc1_stored->req_trq_lim = 4;
	tsc1_stored->destination_address = 5;
	tsc1_stored->src_address = 6;
	db->store(TSC1, (void*)tsc1_stored);

	j1939_ebc1_typ *ebc1_stored = new j1939_ebc1_typ();
	ebc1_stored->timestamp.hour = 23;
	ebc1_stored->timestamp.minute = 59;
	ebc1_stored->timestamp.second = 59;
	ebc1_stored->timestamp.millisecond = 999;
	ebc1_stored->asr_engine_ctrl_active = 0;
	ebc1_stored->asr_brk_ctrl_active = 1;
	ebc1_stored->antilock_brk_active = 2;
	ebc1_stored->ebs_brk_switch = 3;
	ebc1_stored->brk_pedal_pos = 4;
	ebc1_stored->abs_offroad_switch = 5;
	ebc1_stored->asr_offroad_switch = 6;
	ebc1_stored->asr_hillholder_switch = 7;
	ebc1_stored->trac_ctrl_override_switch = 8;
	ebc1_stored->accel_interlock_switch = 9;
	ebc1_stored->eng_derate_switch = 10;
	ebc1_stored->aux_eng_shutdown_switch = 11;
	ebc1_stored->accel_enable_switch = 12;
	ebc1_stored->eng_retarder_selection = 13;
	ebc1_stored->abs_fully_operational = 14;
	ebc1_stored->ebs_red_warning = 15;
	ebc1_stored->abs_ebs_amber_warning = 16;
	ebc1_stored->src_address_ctrl = 17;
	ebc1_stored->total_brk_demand = 18;
	db->store(EBC1, (void*)ebc1_stored);

	j1939_ebc2_typ *ebc2_stored = new j1939_ebc2_typ();
	ebc2_stored->timestamp.hour = 23;
	ebc2_stored->timestamp.minute = 59;
	ebc2_stored->timestamp.second = 59;
	ebc2_stored->timestamp.millisecond = 999;
	ebc2_stored->front_axle_spd = 0;
	ebc2_stored->rel_spd_front_left = 1;
	ebc2_stored->rel_spd_front_right = 2;
	ebc2_stored->rel_spd_rear_left_1 = 3;
	ebc2_stored->rel_spd_rear_right_1 = 4;
	ebc2_stored->rel_spd_rear_left_2 = 5;
	ebc2_stored->rel_spd_rear_right_2 = 6;
	db->store(EBC2, (void*)ebc2_stored);

	j1939_eec1_typ *eec1_stored = new j1939_eec1_typ();
	eec1_stored->timestamp.hour = 23;
	eec1_stored->timestamp.minute = 59;
	eec1_stored->timestamp.second = 59;
	eec1_stored->timestamp.millisecond = 999;
	eec1_stored->eng_trq_mode = 0;
	eec1_stored->drvr_demand_eng_trq = 1;
	eec1_stored->actual_eng_trq = 2;
	eec1_stored->eng_spd = 3;
	eec1_stored->eng_demand_trq = 4;
	eec1_stored->src_address = 5;
	db->store(EEC1, (void*)eec1_stored);

	j1939_eec2_typ *eec2_stored = new j1939_eec2_typ();
	eec2_stored->timestamp.hour = 23;
	eec2_stored->timestamp.minute = 59;
	eec2_stored->timestamp.second = 59;
	eec2_stored->timestamp.millisecond = 999;
	eec2_stored->accel_pedal1_idle = 0;
	eec2_stored->accel_pedal_kickdown = 1;
	eec2_stored->spd_limit_status = 2;
	eec2_stored->accel_pedal2_idle = 3;
	eec2_stored->accel_pedal1_pos = 4;
	eec2_stored->eng_prcnt_load_curr_spd = 5;
	eec2_stored->accel_pedal2_pos = 6;
	eec2_stored->act_max_avail_eng_trq = 7;
	db->store(EEC2, (void*)eec2_stored);

	j1939_eec3_typ *eec3_stored = new j1939_eec3_typ();
	eec3_stored->timestamp.hour = 23;
	eec3_stored->timestamp.minute = 59;
	eec3_stored->timestamp.second = 59;
	eec3_stored->timestamp.millisecond = 999;
	eec3_stored->nominal_friction = 0;
	eec3_stored->desired_operating_spd = 1;
	eec3_stored->operating_spd_adjust = 2;
	eec3_stored->est_eng_prstic_loss = 3;
	db->store(EEC3, (void*)eec3_stored);

	j1939_etc1_typ *etc1_stored = new j1939_etc1_typ();
	etc1_stored->timestamp.hour = 23;
	etc1_stored->timestamp.minute = 59;
	etc1_stored->timestamp.second = 59;
	etc1_stored->timestamp.millisecond = 999;
	etc1_stored->trans_driveline = 0;
	etc1_stored->trq_conv_lockup = 1;
	etc1_stored->trans_shift = 2;
	etc1_stored->tran_output_shaft_spd = 3;
	etc1_stored->prcnt_clutch_slip = 4;
	etc1_stored->eng_overspd_enable = 5;
	etc1_stored->prog_shift_disable = 6;
	etc1_stored->trans_input_shaft_spd = 7;
	etc1_stored->src_address_ctrl = 8;
	db->store(ETC1, (void*)etc1_stored);

	j1939_etc2_typ *etc2_stored = new j1939_etc2_typ();
	etc2_stored->timestamp.hour = 23;
	etc2_stored->timestamp.minute = 59;
	etc2_stored->timestamp.second = 59;
	etc2_stored->timestamp.millisecond = 999;
	etc2_stored->trans_selected_gear = 0;
	etc2_stored->trans_act_gear_ratio = 1;
	etc2_stored->trans_current_gear = 2;
	etc2_stored->range_selected = 3;
	etc2_stored->range_attained = 4;
	db->store(ETC2, (void*)etc2_stored);

	j1939_erc1_typ *erc1_stored = new j1939_erc1_typ();
	erc1_stored->timestamp.hour = 23;
	erc1_stored->timestamp.minute = 59;
	erc1_stored->timestamp.second = 59;
	erc1_stored->timestamp.millisecond = 999;
	erc1_stored->trq_mode = 0;
	erc1_stored->enable_brake_assist = 1;
	erc1_stored->enable_shift_assist = 2;
	erc1_stored->actual_ret_pcnt_trq = 3;
	erc1_stored->intended_ret_pcnt_trq = 4;
	erc1_stored->rq_brake_light = 5;
	erc1_stored->src_address_ctrl = 6;
	erc1_stored->drvrs_demand_prcnt_trq = 7;
	erc1_stored->selection_nonengine = 8;
	erc1_stored->max_available_prcnt_trq = 9;
	db->store(ERC1, (void*)erc1_stored);

	j1939_tf_typ *tf_stored = new j1939_tf_typ();
	tf_stored->timestamp.hour = 23;
	tf_stored->timestamp.minute = 59;
	tf_stored->timestamp.second = 59;
	tf_stored->timestamp.millisecond = 999;
	tf_stored->clutch_pressure = 0;
	tf_stored->oil_level = 1;
	tf_stored->diff_pressure = 2;
	tf_stored->oil_pressure = 3;
	tf_stored->oil_temp = 4;
	db->store(TF, (void*)tf_stored);

	j1939_ccvs_typ *ccvs_stored = new j1939_ccvs_typ();
	ccvs_stored->timestamp.hour = 23;
	ccvs_stored->timestamp.minute = 59;
	ccvs_stored->timestamp.second = 59;
	ccvs_stored->timestamp.millisecond = 999;
	ccvs_stored->two_spd_axle_switch = 0;
	ccvs_stored->parking_brk_switch = 1;
	ccvs_stored->cc_pause_switch = 2;
	ccvs_stored->park_brk_release = 3;
	ccvs_stored->vehicle_spd = 4;
	ccvs_stored->cc_active = 5;
	ccvs_stored->cc_enable_switch = 6;
	ccvs_stored->brk_switch = 7;
	ccvs_stored->clutch_switch = 8;
	ccvs_stored->cc_set_switch = 9;
	ccvs_stored->cc_coast_switch = 10;
	ccvs_stored->cc_resume_switch = 11;
	ccvs_stored->cc_accel_switch = 12;
	ccvs_stored->cc_set_speed = 13;
	ccvs_stored->pto_state = 14;
	ccvs_stored->cc_state = 15;
	ccvs_stored->eng_idle_incr_switch = 16;
	ccvs_stored->eng_idle_decr_switch = 17;
	ccvs_stored->eng_test_mode_switch = 18;
	ccvs_stored->eng_shutdown_override = 19;
	db->store(CCVS, (void*)ccvs_stored);

	j1939_lfe_typ *lfe_stored = new j1939_lfe_typ();
	lfe_stored->timestamp.hour = 23;
	lfe_stored->timestamp.minute = 59;
	lfe_stored->timestamp.second = 59;
	lfe_stored->timestamp.millisecond = 999;
	lfe_stored->eng_fuel_rate = 0;
	lfe_stored->eng_inst_fuel_economy = 1;
	lfe_stored->eng_avg_fuel_economy = 2;
	lfe_stored->eng_throttle1_pos = 3;
	lfe_stored->eng_throttle2_pos = 4;
	db->store(LFE, (void*)lfe_stored);

	j1939_rf_typ *rf_stored = new j1939_rf_typ();
	rf_stored->timestamp.hour = 23;
	rf_stored->timestamp.minute = 59;
	rf_stored->timestamp.second = 59;
	rf_stored->timestamp.millisecond = 999;
	rf_stored->pressure = 0;
	rf_stored->oil_temp = 1;
	db->store(RF, (void*)rf_stored);

	j1939_turbo_typ *turbo_stored = new j1939_turbo_typ();
	turbo_stored->timestamp.hour = 23;
	turbo_stored->timestamp.minute = 59;
	turbo_stored->timestamp.second = 59;
	turbo_stored->timestamp.millisecond = 999;
	turbo_stored->turbo_lube_oil_pressure = 0;
	turbo_stored->turbo_speed = 1;
	db->store(TURBO, (void*)turbo_stored);

	j1939_vd_typ *vd_stored = new j1939_vd_typ();
	vd_stored->timestamp.hour = 23;
	vd_stored->timestamp.minute = 59;
	vd_stored->timestamp.second = 59;
	vd_stored->timestamp.millisecond = 999;
	vd_stored->trip_dist = 0;
	vd_stored->tot_vehicle_dist = 1;
	db->store(VD, (void*)vd_stored);

	j1939_rcfg_typ *rcfg_stored = new j1939_rcfg_typ();
	rcfg_stored->timestamp.hour = 23;
	rcfg_stored->timestamp.minute = 59;
	rcfg_stored->timestamp.second = 59;
	rcfg_stored->timestamp.millisecond = 999;
	rcfg_stored->retarder_type = 0;
	rcfg_stored->retarder_loc = 1;
	rcfg_stored->retarder_ctrl_steps = 2;
	for (int i=0; i<5; ++i) {
		rcfg_stored->retarder_speed[i] = 3+i;
		rcfg_stored->percent_torque[i] = 8+i;
	}
	rcfg_stored->reference_retarder_trq = 13;
	db->store(RCFG, (void*)rcfg_stored);

	j1939_ecfg_typ *ecfg_stored = new j1939_ecfg_typ();
	ecfg_stored->timestamp.hour = 23;
	ecfg_stored->timestamp.minute = 59;
	ecfg_stored->timestamp.second = 59;
	ecfg_stored->timestamp.millisecond = 999;
	for (int i=0; i<5; ++i)
		ecfg_stored->percent_trq[i] = i;
	for (int i=0; i<7; ++i)
		ecfg_stored->engine_spd[i] = 5+i;
	ecfg_stored->gain_endspeed_governor = 12;
	ecfg_stored->reference_eng_trq = 13;
	ecfg_stored->max_momentary_overide_time = 14;
	ecfg_stored->spd_ctrl_lower_lim = 15;
	ecfg_stored->spd_ctrl_upper_lim = 16;
	ecfg_stored->trq_ctrl_lower_lim = 17;
	ecfg_stored->trq_ctrl_upper_lim = 18;
	ecfg_stored->receive_status = 19;
	db->store(ECFG, (void*)ecfg_stored);

	j1939_etemp_typ *etemp_stored = new j1939_etemp_typ();
	etemp_stored->timestamp.hour = 23;
	etemp_stored->timestamp.minute = 59;
	etemp_stored->timestamp.second = 59;
	etemp_stored->timestamp.millisecond = 999;
	etemp_stored->eng_coolant_temp = 0,
	etemp_stored->fuel_temp = 1;
	etemp_stored->eng_oil_temp = 2;
	etemp_stored->turbo_oil_temp = 3;
	etemp_stored->eng_intercooler_temp = 4;
	etemp_stored->eng_intercooler_thermostat_opening = 5;
	db->store(ETEMP, (void*)etemp_stored);

	j1939_pto_typ *pto_stored = new j1939_pto_typ();
	pto_stored->timestamp.hour = 23;
	pto_stored->timestamp.minute = 59;
	pto_stored->timestamp.second = 59;
	pto_stored->timestamp.millisecond = 999;
	pto_stored->oil_temp = 0;
	pto_stored->speed = 1;
	pto_stored->set_speed = 2;
	pto_stored->enable_switch = 3;
	pto_stored->remote_preprogramm_status = 4;
	pto_stored->remote_variable_spd_status = 5;
	pto_stored->set_switch = 6;
	pto_stored->coast_decel_switch = 7;
	pto_stored->resume_switch = 8;
	pto_stored->accel_switch = 9;
	db->store(PTO, (void*)pto_stored);

	j1939_ambc_typ *ambc_stored = new j1939_ambc_typ();
	ambc_stored->timestamp.hour = 23;
	ambc_stored->timestamp.minute = 59;
	ambc_stored->timestamp.second = 59;
	ambc_stored->timestamp.millisecond = 999;
	ambc_stored->barometric_pressure = 0;
	ambc_stored->cab_interior_temp = 1;
	ambc_stored->ambient_air_temp = 2;
	ambc_stored->air_inlet_temp = 3;
	ambc_stored->road_surface_temp = 4;
	db->store(AMBC, (void*)ambc_stored);

	j1939_iec_typ *iec_stored = new j1939_iec_typ();
	iec_stored->timestamp.hour = 23;
	iec_stored->timestamp.minute = 59;
	iec_stored->timestamp.second = 59;
	iec_stored->timestamp.millisecond = 999;
	iec_stored->particulate_inlet_pressure = 0;
	iec_stored->boost_pressure = 1;
	iec_stored->intake_manifold_temp = 2;
	iec_stored->air_inlet_pressure = 3;
	iec_stored->air_filter_diff_pressure = 4;
	iec_stored->exhaust_gas_temp = 5;
	iec_stored->coolant_filter_diff_pressure = 6;
	db->store(IEC, (void*)iec_stored);

	j1939_vep_typ *vep_stored = new j1939_vep_typ();
	vep_stored->timestamp.hour = 23;
	vep_stored->timestamp.minute = 59;
	vep_stored->timestamp.second = 59;
	vep_stored->timestamp.millisecond = 999;
	vep_stored->net_battery_current = 0;
	vep_stored->alternator_current = 1;
	vep_stored->alternator_potential = 2;
	vep_stored->electrical_potential = 3;
	vep_stored->battery_potential = 4;
	db->store(VEP, (void*)vep_stored);

	j1939_hrvd_typ *hrvd_stored = new j1939_hrvd_typ();
	hrvd_stored->timestamp.hour = 23;
	hrvd_stored->timestamp.minute = 59;
	hrvd_stored->timestamp.second = 59;
	hrvd_stored->timestamp.millisecond = 999;
	hrvd_stored->vehicle_distance = 0;
	hrvd_stored->trip_distance = 1;
	db->store(HRVD, (void*)hrvd_stored);

	j1939_fd_typ *fd_stored = new j1939_fd_typ();
	fd_stored->timestamp.hour = 23;
	fd_stored->timestamp.minute = 59;
	fd_stored->timestamp.second = 59;
	fd_stored->timestamp.millisecond = 999;
	fd_stored->prcnt_fan_spd = 0;
	fd_stored->fan_drive_state = 1;
	db->store(FD, (void*)fd_stored);

	j1939_gfi2_typ *gfi2_stored = new j1939_gfi2_typ();
	gfi2_stored->timestamp.hour = 23;
	gfi2_stored->timestamp.minute = 59;
	gfi2_stored->timestamp.second = 59;
	gfi2_stored->timestamp.millisecond = 999;
	gfi2_stored->fuel_flow_rate1 = 0;
	gfi2_stored->fuel_flow_rate2 = 1;
	gfi2_stored->fuel_valve_pos1 = 2;
	gfi2_stored->fuel_valve_pos2 = 3;
	db->store(GFI2, (void*)gfi2_stored);

	j1939_ei_typ *ei_stored = new j1939_ei_typ();
	ei_stored->timestamp.hour = 23;
	ei_stored->timestamp.minute = 59;
	ei_stored->timestamp.second = 59;
	ei_stored->timestamp.millisecond = 999;
	ei_stored->pre_filter_oil_pressure = 0;
	ei_stored->exhaust_gas_pressure = 1;
	ei_stored->rack_position = 2;
	ei_stored->eng_gas_mass_flow = 3;
	ei_stored->inst_estimated_brake_power = 4;
	db->store(EI, (void*)ei_stored);

	/* Transfer the elements in the database to a txt file. */
	system("../../../../sql2txt /dev/test_logdir -o /tmp/test_logger_out.txt -n");

	/* Check the validity of the elements in the txt file. */
    ifstream in("/tmp/test_logger_out.txt");
    char line[255];
    string pgn_type;

    while (in) {
        /* Read data from the text file. */
        in.getline(line, 255);
        string str = line;

        /* Grab the first element, which represents the type of message. */
        pgn_type = strtok(line, " ");

        if (pgn_type.compare("PDU") != 0)
        	BOOST_CHECK(str.compare("PDU 23:59:59.999 0 1 2 3 8 4 5 6 7 8 9 10 11"));

        else if (pgn_type.compare("TSC1") != 0)
        	BOOST_CHECK(str.compare("TSC1 23:59:59.999 5 6 2 1 0 3.000 4.000"));

        else if (pgn_type.compare("EBC1") != 0)
        	BOOST_CHECK(str.compare("EBC1 23:59:59.999 3 2 1 0 4.00 8 7 5 6 12 11 10 9 13.00 16 15 14 17 18.000"));

        else if (pgn_type.compare("EBC2") != 0)
			BOOST_CHECK(str.compare("EBC2 23:59:59.999 0.000 1.000 2.000 3.000 4.000 5.000 6.000"));

        else if (pgn_type.compare("EEC1") != 0)
			BOOST_CHECK(str.compare("EEC1 23:59:59.999 0 1.00 2.00 4.00 3.000 5"));

        else if (pgn_type.compare("EEC2") != 0)
        	BOOST_CHECK(str.compare("EEC2 23:59:59.999 2 1 0 3 4.00 6.00 5.00 7.00"));

        else if (pgn_type.compare("EEC3") != 0)
        	BOOST_CHECK(str.compare("EEC3 23:59:59.999 0.00 3.00 2 1.00"));

        else if (pgn_type.compare("ERC1") != 0)
			BOOST_CHECK(str.compare("ERC1 23:59:59.999 2 1 0 3.00 4.00 5 6 7 8.00 9"));

		else if (pgn_type.compare("ETC1") != 0)
			BOOST_CHECK(str.compare("ETC1 23:59:59.999 2 1 0 3.00 4.00 6 5 7.00 8"));

		else if (pgn_type.compare("ETC2") != 0)
			BOOST_CHECK(str.compare("ETC2 23:59:59.999 0 1.00 2 3 4"));

		else if (pgn_type.compare("TURBO") != 0)
			BOOST_CHECK(str.compare("TURBO 23:59:59.999 0.00 1.00"));

		else if (pgn_type.compare("VD") != 0)
			BOOST_CHECK(str.compare("VD 23:59:59.999 0.00 1.00"));

		else if (pgn_type.compare("RCFG") != 0)
			BOOST_CHECK(str.compare("RCFG 23:59:59.999 1 0 2 3.00 4.00 5.00 6.00 7.00 8.00 9.00 10.00 11.00 12.00 13.00"));

		else if (pgn_type.compare("ECFG") != 0)
			BOOST_CHECK(str.compare("ECFG 23:59:59.999 0x13 5.00 6.00 7.00 8.00 9.00 10.00 11.00 0.00 1.00 2.00 3.00 4.00 12.00 13.00 14.00 15.00 16.00 17.00 18.00"));

		else if (pgn_type.compare("ETEMP") != 0)
			BOOST_CHECK(str.compare("ETEMP 23:59:59.999 0.000 1.000 2.000 3.000 4.000 5.000"));

		else if (pgn_type.compare("PTO") != 0)
			BOOST_CHECK(str.compare("PTO 23:59:59.999 0.000 1.000 2.000 5 4 3 9 8 7 6"));

		else if (pgn_type.compare("CCVS") != 0)
			BOOST_CHECK(str.compare("CCVS 23:59:59.999 1 3 0 4.000 8 7 2 6 5 12 11 10 9 13.000 15 14 19 18 17 16"));

		else if (pgn_type.compare("LFE") != 0)
			BOOST_CHECK(str.compare("LFE 23:59:59.999 0.000 1.000 2.000 3.000 4.000"));

		else if (pgn_type.compare("AMBC") != 0)
			BOOST_CHECK(str.compare("AMBC 23:59:59.999 0.000 1.000 2.000 3.000 4.000"));

		else if (pgn_type.compare("IEC") != 0)
			BOOST_CHECK(str.compare("IEC 23:59:59.999 0.000 1.000 2.000 3.000 4.000 5.000 6.000"));

		else if (pgn_type.compare("VEP") != 0)
			BOOST_CHECK(str.compare("VEP 23:59:59.999 0.000 1.000 2.000 3.000 4.000"));

		else if (pgn_type.compare("TF") != 0)
			BOOST_CHECK(str.compare("TF 23:59:59.999 0.000 1.000 2.000 3.000 4.000"));

		else if (pgn_type.compare("RF") != 0)
			BOOST_CHECK(str.compare("RF 23:59:59.999 0.000 1.000"));

		else if (pgn_type.compare("HRVD") != 0)
			BOOST_CHECK(str.compare("HRVD 23:59:59.999 0.000 1.000"));

		else if (pgn_type.compare("FD") != 0)
			BOOST_CHECK(str.compare("FD 23:59:59.999 0.000 1"));

		else if (pgn_type.compare("GFI2") != 0)
			BOOST_CHECK(str.compare("GFI2 23:59:59.999 0.000 1.000 2.000 3.000"));

		else if (pgn_type.compare("EI") != 0)
			BOOST_CHECK(str.compare("EI 23:59:59.999 0.000 1.000 2.000 3.000 4.000"));
    }

	/* Clear memory. */
	delete pdu_stored;
	delete tsc1_stored;
	delete ebc1_stored;
	delete ebc2_stored;
	delete eec1_stored;
	delete eec2_stored;
	delete eec3_stored;
	delete etc1_stored;
	delete etc2_stored;
	delete erc1_stored;
	delete tf_stored;
	delete ccvs_stored;
	delete lfe_stored;
	delete rf_stored;
	delete turbo_stored;
	delete vd_stored;
	delete rcfg_stored;
	delete ecfg_stored;
	delete etemp_stored;
	delete pto_stored;
	delete ambc_stored;
	delete iec_stored;
	delete vep_stored;
	delete hrvd_stored;
	delete fd_stored;
	delete gfi2_stored;
	delete ei_stored;
}

BOOST_AUTO_TEST_SUITE_END()
