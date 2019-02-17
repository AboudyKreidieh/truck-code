/**\file
 *
 * test_j1939_utils.cpp
 *
 * Unit tests to (blank).
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 10, 2019
 */

#define BOOST_TEST_MODULE "test_j1939_interpreters"
#include <boost/test/unit_test.hpp>
#include "jbus/j1939_utils.h"
#include "jbus/j1939_interpreters.h"
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>

/** Validate the values of a timestamp object.
 *
 * @param time the tested timestamp
 * @param hour expected hour value
 * @param minute expected minute value
 * @param second expected second value
 * @param millisecond expected millisecond value
 * @return true if the values match, false otherwise
 */
bool check_timestamp_equal(
		timestamp_t time, int hour, int minute, int second, int millisecond) {
	return (time.hour == hour &&
			time.minute == minute &&
			time.second == second &&
			time.millisecond == millisecond);
}

/** Validate the values of in a text file.
 *
 * @param infile file that is being checked
 * @param expected_values list of strings expected from each line
 * @return true if the values match, false otherwise
 */
bool check_import_match(string infile, vector<string> expected_values) {
	// create a connection with the input file
    ifstream in(infile);
    char line[255];

    unsigned int i = 0;
    while (in) {
        // read data from the text file
        in.getline(line, 255);

        // check if the next line matches what is expected
        if (line != expected_values[i]){
        	printf("Expected: %s\n", expected_values[i].c_str());
        	printf("Actual:   %s\n", line);
        	remove(infile.c_str());
        	return false;
        }
        i++;
        if (i>= expected_values.size()) break;
    }

	remove(infile.c_str());
	return true;
}

/* -------------------------------------------------------------------------- */
/* ---------------------------- PDU Interpreter ----------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_PDUInterpreter )

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

BOOST_AUTO_TEST_CASE( test_pgn_pdu )
{
	PDUInterpreter *interpreter = new PDUInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn, PDU);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	pdu->pdu_format = 0;
	pdu->pdu_specific = 255;
	PDUInterpreter *interpreter = new PDUInterpreter();
	j1939_pdu_typ *pdv;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	pdv = (j1939_pdu_typ*) interpreter->convert(pdu);
	int data_field[8] {0, 0, 0, 0, 0, 0, 0, 0};
	BOOST_CHECK(check_pdu(pdv, 0, 0, 0, 0, data_field, 0));
	BOOST_CHECK(check_timestamp_equal(pdv->timestamp, 0, 0, 0, 0));

	// free memory
	delete pdu;
	delete pdv;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_pdu )
{
	// initialize variables
    FILE *fp = fopen("/tmp/test.txt", "w");
	PDUInterpreter *interpreter = new PDUInterpreter();
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	pdu->timestamp.hour = 23;
	pdu->timestamp.minute = 59;
	pdu->timestamp.second = 59;
	pdu->timestamp.millisecond = 999;
	pdu->priority = 0;
	pdu->pdu_format = 1;
	pdu->pdu_specific = 2;
	pdu->src_address = 3;
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = 4+i;
	pdu->num_bytes = 8;

	// test the numeric case
	vector<string> expected {
    	"PDU 23:59:59.999 0 1 2 3 8 4 5 6 7 8 9 10 11"
	};
	interpreter->print((void*)pdu, fp, true);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	expected = {
		"PDU 23:59:59.999",
		" Priority 0",
		" Protocol Data Unit Format (PF) 1",
		" PDU Specific (PS) 2",
		" Source Address 3",
		" Number of bytes 8",
		" Data Field 4 5 6 7 8 9 10 11"
	};
	interpreter->print((void*)pdu, fp, false);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete pdu;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_pdu )
{
	// initialize variables
	PDUInterpreter *interpreter = new PDUInterpreter();

	// import and test something
    vector<string> tokens {
    	"PDU", "23:59:59.999", "0", "1", "2", "3", "8", "4", "5", "6", "7", "8",
		"9", "10", "11"
    };
	j1939_pdu_typ *pdu = (j1939_pdu_typ*) interpreter->import(tokens);
	int data_field[8] {4, 5, 6, 7, 8, 9, 10, 11};
	BOOST_CHECK(check_pdu(pdu, 0, 1, 2, 3, data_field, 8));
	BOOST_CHECK(check_timestamp_equal(pdu->timestamp, 23, 59, 59, 999));

	// free memory
	delete pdu;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- TSC1 Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_TSC1Interpreter )

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

BOOST_AUTO_TEST_CASE( test_pgn_tsc1 )
{
	TSC1Interpreter *interpreter = new TSC1Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn, TSC1);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_tsc1 )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	pdu->pdu_format = 0;
	pdu->pdu_specific = 0;
	TSC1Interpreter *interpreter = new TSC1Interpreter();
	j1939_tsc1_typ *tsc1;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	tsc1 = (j1939_tsc1_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_tsc1(tsc1, 0, 0, 0, 0, 0, 0, 0));
	BOOST_CHECK(check_timestamp_equal(tsc1->timestamp, 0, 0, 0, 0));

	// free memory
	delete pdu;
	delete tsc1;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_tsc1 )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	TSC1Interpreter *interpreter = new TSC1Interpreter();
	j1939_tsc1_typ *tsc1 = new j1939_tsc1_typ();
	tsc1->timestamp.hour = 23;
	tsc1->timestamp.minute = 59;
	tsc1->timestamp.second = 59;
	tsc1->timestamp.millisecond = 999;
	tsc1->ovrd_ctrl_m = 0;
	tsc1->req_spd_ctrl = 1;
	tsc1->ovrd_ctrl_m_pr = 2;
	tsc1->req_spd_lim = 3;
	tsc1->req_trq_lim = 4;
	tsc1->destination_address = 5;
	tsc1->src_address = 6;

	// test the numeric case
    fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"TSC1 23:59:59.999 5 6 2 1 0 3.000 4.000"
    };
	interpreter->print((void*)tsc1, fp, true);
    fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
	expected = {
		"TSC1 23:59:59.999",
		" Destination 5",
		" Source address 6",
		" Override Control Mode priority 2",
		" Requested speed control conditions 1",
		" Override control mode 0",
		" Requested speed/speed limit 3.000",
		" Requested torque/torque limit 4.000",
	};
	interpreter->print((void*)tsc1, fp, false);
    fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete tsc1;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_tsc1 )
{
	// initialize variables
	TSC1Interpreter *interpreter = new TSC1Interpreter();

	// import and test something
    vector<string> tokens {
    	"TSC1", "23:59:59.999", "5", "6", "2", "1", "0", "3", "4"
    };
	j1939_tsc1_typ *tsc1 = (j1939_tsc1_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_tsc1(tsc1, 0, 1, 2, 3, 4, 5, 6));
	BOOST_CHECK(check_timestamp_equal(tsc1->timestamp, 23, 59, 59, 999));

	// free memory
	delete tsc1;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- EBC1 Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_EBC1Interpreter )

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

BOOST_AUTO_TEST_CASE( test_pgn_ebc1 )
{
	EBC1Interpreter *interpreter = new EBC1Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn, EBC1);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_ebc1 )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	pdu->pdu_format = 240;
	pdu->pdu_specific = 1;
	EBC1Interpreter *interpreter = new EBC1Interpreter();
	j1939_ebc1_typ *ebc1;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	ebc1 = (j1939_ebc1_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_ebc1(ebc1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
			14, 15, 16, 17, 18));
	BOOST_CHECK(check_timestamp_equal(ebc1->timestamp, 0, 0, 0, 0));

	// free memory
	delete pdu;
	delete ebc1;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_ebc1 )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	EBC1Interpreter *interpreter = new EBC1Interpreter();
	j1939_ebc1_typ *ebc1 = new j1939_ebc1_typ();
	ebc1->timestamp.hour = 23;
	ebc1->timestamp.minute = 59;
	ebc1->timestamp.second = 59;
	ebc1->timestamp.millisecond = 999;
	ebc1->asr_engine_ctrl_active = 0;
	ebc1->asr_brk_ctrl_active = 1;
	ebc1->antilock_brk_active = 2;
	ebc1->ebs_brk_switch = 3;
	ebc1->brk_pedal_pos = 4;
	ebc1->abs_offroad_switch = 5;
	ebc1->asr_offroad_switch = 6;
	ebc1->asr_hillholder_switch = 7;
	ebc1->trac_ctrl_override_switch = 8;
	ebc1->accel_interlock_switch = 9;
	ebc1->eng_derate_switch = 10;
	ebc1->aux_eng_shutdown_switch = 11;
	ebc1->accel_enable_switch = 12;
	ebc1->eng_retarder_selection = 13;
	ebc1->abs_fully_operational = 14;
	ebc1->ebs_red_warning = 15;
	ebc1->abs_ebs_amber_warning = 16;
	ebc1->src_address_ctrl = 17;
	ebc1->total_brk_demand = 18;

	// test the numeric case
    fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"EBC1 23:59:59.999 3 2 1 0 4.00 8 7 5 6 12 11 10 9 13.00 16 15 14 17 "
    	"18.000"
    };
	interpreter->print((void*)ebc1, fp, true);
    fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
    fp = fopen("/tmp/test.txt", "w");
	expected = {
		"EBC1 23:59:59.999",
		" EBS brake switch status 3",
		" ABS active status 2",
		" ASR brake control status 1",
		" ASR engine control active status 0",
		" Brake pedal position 4.00",
		" Traction control override switch status 8",
		" Hill holder switch status 7",
		" ABS off road switch status 5",
		" ASR off road switch status 6",
		" Remote accelerator enable switch status 12",
		" Auxiliary engine shutdown switch status 11",
		" Engine derate switch status 10",
		" Accelerator interlock switch status 9",
		" Percent engine retarder torque selected 13.00",
		" ABS/EBS amber warning state 16",
		" EBS red warning state 15",
		" ABS fully operational 14",
		" Source address 17 (0x%11)",
		" Total brake demand 18.000",
	};
	interpreter->print((void*)ebc1, fp, false);
    fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete ebc1;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_ebc1 )
{
	// initialize variables
	EBC1Interpreter *interpreter = new EBC1Interpreter();

	// import and test something
    vector<string> tokens {
    	"EBC1", "23:59:59.999", "3", "2", "1", "0", "4.00", "8", "7", "5", "6",
		"12", "11", "10", "9", "13.00", "16", "15", "14", "17", "18.000"
    };
	j1939_ebc1_typ *ebc1 = (j1939_ebc1_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_ebc1(ebc1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
			14, 15, 16, 17, 18));
	BOOST_CHECK(check_timestamp_equal(ebc1->timestamp, 23, 59, 59, 999));

	// free memory
	delete ebc1;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- EBC2 Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_EBC2Interpreter )

bool check_ebc2(
		j1939_ebc2_typ *ebc2,
		float front_axle_spd,
		float rel_spd_front_left,
		float rel_spd_front_right,
		float rel_spd_rear_left_1,
		float rel_spd_rear_right_1,
		float rel_spd_rear_left_2,
		float rel_spd_rear_right_2) {
	return (ebc2->front_axle_spd == front_axle_spd &&
			ebc2->rel_spd_front_left == rel_spd_front_left &&
			ebc2->rel_spd_front_right == rel_spd_front_right &&
			ebc2->rel_spd_rear_left_1 == rel_spd_rear_left_1 &&
			ebc2->rel_spd_rear_right_1 == rel_spd_rear_right_1 &&
			ebc2->rel_spd_rear_left_2 == rel_spd_rear_left_2 &&
			ebc2->rel_spd_rear_right_2 == rel_spd_rear_right_2);
}

BOOST_AUTO_TEST_CASE( test_pgn_ebc2 )
{
	EBC2Interpreter *interpreter = new EBC2Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn, EBC2);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_ebc2 )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	EBC2Interpreter *interpreter = new EBC2Interpreter();
	j1939_ebc2_typ *ebc2;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	ebc2 = (j1939_ebc2_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_ebc2(ebc2, 0, 1, 2, 3, 4, 5, 6));
	BOOST_CHECK(check_timestamp_equal(ebc2->timestamp, 0, 0, 0, 0));

	// free memory
	delete pdu;
	delete ebc2;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_ebc2 )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	EBC2Interpreter *interpreter = new EBC2Interpreter();
	j1939_ebc2_typ *ebc2 = new j1939_ebc2_typ();
	ebc2->timestamp.hour = 23;
	ebc2->timestamp.minute = 59;
	ebc2->timestamp.second = 59;
	ebc2->timestamp.millisecond = 999;
	ebc2->front_axle_spd = 0;
	ebc2->rel_spd_front_left = 1;
	ebc2->rel_spd_front_right = 2;
	ebc2->rel_spd_rear_left_1 = 3;
	ebc2->rel_spd_rear_right_1 = 4;
	ebc2->rel_spd_rear_left_2 = 5;
	ebc2->rel_spd_rear_right_2 = 6;

	// test the numeric case
    fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"EBC2 23:59:59.999 0.000 1.000 2.000 3.000 4.000 5.000 6.000"
    };
	interpreter->print((void*)ebc2, fp, true);
    fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
    fp = fopen("/tmp/test.txt", "w");
	expected = {
		"EBC2 23:59:59.999",
		" Front axle speed 0.000",
		" Front left wheel relative speed 1.000",
		" Front right wheel relative speed 2.000",
		" Rear 1 left wheel relative speed 3.000",
		" Rear 1 left wheel relative speed 4.000",
		" Rear 2 left wheel relative speed 5.000",
		" Rear 2 left wheel relative speed 6.000",
	};
	interpreter->print((void*)ebc2, fp, false);
    fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete ebc2;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_ebc2 )
{
	// initialize variables
	EBC2Interpreter *interpreter = new EBC2Interpreter();

	// import and test something
    vector<string> tokens {
    	"EBC2", "23:59:59.999", "0", "1", "2", "3", "4", "5", "6"
    };
	j1939_ebc2_typ *ebc2 = (j1939_ebc2_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_ebc2(ebc2, 0, 1, 2, 3, 4, 5, 6));
	BOOST_CHECK(check_timestamp_equal(ebc2->timestamp, 23, 59, 59, 999));

	// free memory
	delete ebc2;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- EEC1 Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_EEC1Interpreter )

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

BOOST_AUTO_TEST_CASE( test_pgn_eec1 )
{
	EEC1Interpreter *interpreter = new EEC1Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn, EEC1);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_eec1 )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	pdu->pdu_format = 240;
	pdu->pdu_specific = 4;
	EEC1Interpreter *interpreter = new EEC1Interpreter();
	j1939_eec1_typ *eec1;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	eec1 = (j1939_eec1_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_eec1(eec1, 0, 1, 2, 3, 4, 5));
	BOOST_CHECK(check_timestamp_equal(eec1->timestamp, 0, 0, 0, 0));

	// free memory
	delete pdu;
	delete eec1;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_eec1 )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	EEC1Interpreter *interpreter = new EEC1Interpreter();
	j1939_eec1_typ *eec1 = new j1939_eec1_typ();
	eec1->timestamp.hour = 23;
	eec1->timestamp.minute = 59;
	eec1->timestamp.second = 59;
	eec1->timestamp.millisecond = 999;
	eec1->eng_trq_mode = 0;
	eec1->drvr_demand_eng_trq = 1;
	eec1->actual_eng_trq = 2;
	eec1->eng_spd = 3;
	eec1->eng_demand_trq = 4;
	eec1->src_address = 5;

	// test the numeric case
    fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"EEC1 23:59:59.999 0 1.00 2.00 4.00 3.000 5"
    };
	interpreter->print((void*)eec1, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
    fp = fopen("/tmp/test.txt", "w");
	expected = {
		"EEC1 23:59:59.999",
		" Engine retarder torque mode 0",
		" Driver's demand percent torque 1.00",
		" Actual engine percent torque 2.00",
		" Engine Demand - Percent Torque 4.00",
		" Engine speed (rpm) 3.000",
		" Source address engine control device 5",
	};
	interpreter->print((void*)eec1, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete eec1;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_eec1 )
{
	// initialize variables
	EEC1Interpreter *interpreter = new EEC1Interpreter();

	// import and test something
    vector<string> tokens {
    	"EEC1", "23:59:59.999", "0", "1.00", "2.00", "4.00", "3.000", "5"
    };
	j1939_eec1_typ *eec1 = (j1939_eec1_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_eec1(eec1, 0, 1, 2, 3, 4, 5));
	BOOST_CHECK(check_timestamp_equal(eec1->timestamp, 23, 59, 59, 999));

	// free memory
	delete eec1;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- EEC2 Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_EEC2Interpreter )

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

BOOST_AUTO_TEST_CASE( test_pgn_eec2 )
{
	EEC2Interpreter *interpreter = new EEC2Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn, EEC2);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_eec2 )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	EEC2Interpreter *interpreter = new EEC2Interpreter();
	j1939_eec2_typ *eec2;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	eec2 = (j1939_eec2_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_eec2(eec2, 0, 1, 2, 3, 4, 5, 6, 7));
	BOOST_CHECK(check_timestamp_equal(eec2->timestamp, 0, 0, 0, 0));

	// free memory
	delete pdu;
	delete eec2;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_eec2 )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	EEC2Interpreter *interpreter = new EEC2Interpreter();
	j1939_eec2_typ *eec2 = new j1939_eec2_typ();
	eec2->timestamp.hour = 23;
	eec2->timestamp.minute = 59;
	eec2->timestamp.second = 59;
	eec2->timestamp.millisecond = 999;
	eec2->accel_pedal1_idle = 0;
	eec2->accel_pedal_kickdown = 1;
	eec2->spd_limit_status = 2;
	eec2->accel_pedal2_idle = 3;
	eec2->accel_pedal1_pos = 4;
	eec2->eng_prcnt_load_curr_spd = 5;
	eec2->accel_pedal2_pos = 6;
	eec2->act_max_avail_eng_trq = 7;

	// test the numeric case
    fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"EEC2 23:59:59.999 2 1 0 3 4.00 6.00 5.00 7.00"
    };
	interpreter->print((void*)eec2, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
    fp = fopen("/tmp/test.txt", "w");
	expected = {
		"EEC2 23:59:59.999",
		" Road speed limit 2",
		" Kickpedal active 1",
		" Low idle 1 0",
		" Low idle 2 3",
		" AP1 position 4.00",
		" AP2 position 6.00",
		" Percent load 5.00",
		" Percent torque 7.00",
	};
	interpreter->print((void*)eec2, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete eec2;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_eec2 )
{
	// initialize variables
	EEC2Interpreter *interpreter = new EEC2Interpreter();

	// import and test something
    vector<string> tokens {
    	"EEC2", "23:59:59.999", "2", "1", "0", "3", "4.00", "6.00", "5.00",
		"7.00"
    };
	j1939_eec2_typ *eec2 = (j1939_eec2_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_eec2(eec2, 0, 1, 2, 3, 4, 5, 6, 7));
	BOOST_CHECK(check_timestamp_equal(eec2->timestamp, 23, 59, 59, 999));

	// free memory
	delete eec2;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- EEC3 Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_EEC3Interpreter )

bool check_eec3(
		j1939_eec3_typ *eec3,
		float nominal_friction,
		float desired_operating_spd,
		int operating_spd_adjust,
		float est_eng_prstic_loss) {
	printf(" %.2f", eec3->nominal_friction);
	printf(" %.2f", eec3->est_eng_prstic_loss);
	printf(" %d", eec3->operating_spd_adjust);
	printf(" %.2f", eec3->desired_operating_spd);
	return (eec3->nominal_friction == nominal_friction &&
			eec3->desired_operating_spd == desired_operating_spd &&
			eec3->operating_spd_adjust == operating_spd_adjust &&
			eec3->est_eng_prstic_loss == est_eng_prstic_loss);
}

BOOST_AUTO_TEST_CASE( test_pgn_eec3 )
{
	EEC3Interpreter *interpreter = new EEC3Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn, EEC3);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_eec3 )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	EEC3Interpreter *interpreter = new EEC3Interpreter();
	j1939_eec3_typ *eec3;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	eec3 = (j1939_eec3_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_eec3(eec3, 0, 1, 2, 3));
	BOOST_CHECK(check_timestamp_equal(eec3->timestamp, 0, 0, 0, 0));

	// free memory
	delete pdu;
	delete eec3;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_eec3 )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	EEC3Interpreter *interpreter = new EEC3Interpreter();
	j1939_eec3_typ *eec3 = new j1939_eec3_typ();
	eec3->timestamp.hour = 23;
	eec3->timestamp.minute = 59;
	eec3->timestamp.second = 59;
	eec3->timestamp.millisecond = 999;
	eec3->nominal_friction = 0;
	eec3->desired_operating_spd = 1;
	eec3->operating_spd_adjust = 2;
	eec3->est_eng_prstic_loss = 3;

	// test the numeric case
    fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"EEC3 23:59:59.999 0.00 3.00 2 1.00"
    };
	interpreter->print((void*)eec3, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
    fp = fopen("/tmp/test.txt", "w");
	expected = {
		"EEC3 23:59:59.999",
		" Nominal friction percent torque 0.00",
		" Estimated engine power loss - percent torque 3.00",
		" Desired Operating Speed Asymmetry Adjustment 2",
		" Engine desired operating speed 1.00",
	};
	interpreter->print((void*)eec3, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete eec3;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_eec3 )
{
	// initialize variables
	EEC3Interpreter *interpreter = new EEC3Interpreter();

	// import and test something
    vector<string> tokens {
    	"EEC3", "23:59:59.999", "0.00", "3.00", "2", "1.00"
    };
	j1939_eec3_typ *eec3 = (j1939_eec3_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_eec3(eec3, 0, 1, 2, 3));
	BOOST_CHECK(check_timestamp_equal(eec3->timestamp, 23, 59, 59, 999));

	// free memory
	delete eec3;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- ETC1 Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_ETC1Interpreter )

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

BOOST_AUTO_TEST_CASE( test_pgn_etc1 )
{
	ETC1Interpreter *interpreter = new ETC1Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn, ETC1);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_etc1 )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	ETC1Interpreter *interpreter = new ETC1Interpreter();
	j1939_etc1_typ *etc1;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	etc1 = (j1939_etc1_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_etc1(etc1, 0, 1, 2, 3, 4, 5, 6, 7, 8));
	BOOST_CHECK(check_timestamp_equal(etc1->timestamp, 0, 0, 0, 0));

	// free memory
	delete pdu;
	delete etc1;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_etc1 )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	ETC1Interpreter *interpreter = new ETC1Interpreter();
	j1939_etc1_typ *etc1 = new j1939_etc1_typ();
	etc1->timestamp.hour = 23;
	etc1->timestamp.minute = 59;
	etc1->timestamp.second = 59;
	etc1->timestamp.millisecond = 999;
	etc1->trans_driveline = 0;
	etc1->trq_conv_lockup = 1;
	etc1->trans_shift = 2;
	etc1->tran_output_shaft_spd = 3;
	etc1->prcnt_clutch_slip = 4;
	etc1->eng_overspd_enable = 5;
	etc1->prog_shift_disable = 6;
	etc1->trans_input_shaft_spd = 7;
	etc1->src_address_ctrl = 8;

	// test the numeric case
    fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"ETC1 23:59:59.999 2 1 0 3.00 4.00 6 5 7.00 8"
    };
	interpreter->print((void*)etc1, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
    fp = fopen("/tmp/test.txt", "w");
	expected = {
		"ETC1 23:59:59.999",
		" Shift in progress 2",
		" Torque converter lockup engaged 1",
		" Driveline engaged 0",
		" Output shaft speed 3.00",
		" Percent clutch slip 4.00",
		" Progressive shift disable 6",
		" Momentary engine overspeed enable 5",
		" Input shaft speed 7.00",
		" Source address 8 (0x8)",
	};
	interpreter->print((void*)etc1, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete etc1;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_etc1 )
{
	// initialize variables
	ETC1Interpreter *interpreter = new ETC1Interpreter();

	// import and test something
    vector<string> tokens {
    	"ETC1", "23:59:59.999", "2", "1", "0", "3.00", "4.00", "6", "5", "7.00",
		"8"
    };
	j1939_etc1_typ *etc1 = (j1939_etc1_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_etc1(etc1, 0, 1, 2, 3, 4, 5, 6, 7, 8));
	BOOST_CHECK(check_timestamp_equal(etc1->timestamp, 23, 59, 59, 999));

	// free memory
	delete etc1;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- ETC2 Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_ETC2Interpreter )

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

BOOST_AUTO_TEST_CASE( test_pgn_etc2 )
{
	ETC2Interpreter *interpreter = new ETC2Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn, ETC2);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_etc2 )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	pdu->pdu_format = 240;
	pdu->pdu_specific = 5;
	ETC2Interpreter *interpreter = new ETC2Interpreter();
	j1939_etc2_typ *etc2;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	etc2 = (j1939_etc2_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_etc2(etc2, 0, 1, 2, 3, 4));
	BOOST_CHECK(check_timestamp_equal(etc2->timestamp, 0, 0, 0, 0));

	// free memory
	delete pdu;
	delete etc2;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_etc2 )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	ETC2Interpreter *interpreter = new ETC2Interpreter();
	j1939_etc2_typ *etc2 = new j1939_etc2_typ();
	etc2->timestamp.hour = 23;
	etc2->timestamp.minute = 59;
	etc2->timestamp.second = 59;
	etc2->timestamp.millisecond = 999;
	etc2->trans_selected_gear = 0;
	etc2->trans_act_gear_ratio = 1;
	etc2->trans_current_gear = 2;
	etc2->range_selected = 3;
	etc2->range_attained = 4;

	// test the numeric case
    fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"ETC2 23:59:59.999 0 1.00 2 3 4"
    };
	interpreter->print((void*)etc2, fp, true);
	fclose (fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
    fp = fopen("/tmp/test.txt", "w");
	expected = {
		"ETC2 23:59:59.999",
		" Selected gear 0",
		" Actual gear ratio 1.00",
		" Current gear 2",
		" Trans. requested range 3",
		" Trans. current range 4",
	};
	interpreter->print((void*)etc2, fp, false);
    fclose (fp);
    BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete etc2;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_etc2 )
{
	// initialize variables
	ETC2Interpreter *interpreter = new ETC2Interpreter();

	// import and test something
    vector<string> tokens {
    	"ETC2", "23:59:59.999", "0", "1", "2", "3", "4"
    };
	j1939_etc2_typ *etc2 = (j1939_etc2_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_etc2(etc2, 0, 1, 2, 3, 4));
	BOOST_CHECK(check_timestamp_equal(etc2->timestamp, 23, 59, 59, 999));

	// free memory
	delete etc2;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

///* -------------------------------------------------------------------------- */
///* ---------------------------- ERC1 Interpreter ---------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_ERC1Interpreter )
//
//bool check_erc1(
//		j1939_erc1_typ *erc1,
//		int trq_mode,
//		int enable_brake_assist,
//		int enable_shift_assist,
//		float actual_ret_pcnt_trq,
//		float intended_ret_pcnt_trq,
//		int rq_brake_light,
//		int src_address_ctrl,
//		int drvrs_demand_prcnt_trq,
//		float selection_nonengine,
//		int max_available_prcnt_trq) {
//	return (erc1->trq_mode == trq_mode &&
//			erc1->enable_brake_assist == enable_brake_assist &&
//			erc1->enable_shift_assist == enable_shift_assist &&
//			erc1->actual_ret_pcnt_trq == actual_ret_pcnt_trq &&
//			erc1->intended_ret_pcnt_trq == intended_ret_pcnt_trq &&
//			erc1->rq_brake_light == rq_brake_light &&
//			erc1->src_address_ctrl == src_address_ctrl &&
//			erc1->drvrs_demand_prcnt_trq == drvrs_demand_prcnt_trq &&
//			erc1->selection_nonengine == selection_nonengine &&
//			erc1->max_available_prcnt_trq == max_available_prcnt_trq);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	ERC1Interpreter *interpreter = new ERC1Interpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, ERC1);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	ERC1Interpreter *interpreter = new ERC1Interpreter();
//	j1939_erc1_typ *erc1;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	erc1 = (j1939_erc1_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_erc1(erc1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
//	BOOST_CHECK(check_timestamp_equal(erc1->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete erc1;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	ERC1Interpreter *interpreter = new ERC1Interpreter();
//	j1939_erc1_typ *erc1 = new j1939_erc1_typ();
//	erc1->timestamp.hour = 23;
//	erc1->timestamp.minute = 59;
//	erc1->timestamp.second = 59;
//	erc1->timestamp.millisecond = 999;
//	erc1->trq_mode = 0;
//	erc1->enable_brake_assist = 1;
//	erc1->enable_shift_assist = 2;
//	erc1->actual_ret_pcnt_trq = 3;
//	erc1->intended_ret_pcnt_trq = 4;
//	erc1->rq_brake_light = 5;
//	erc1->src_address_ctrl = 6;
//	erc1->drvrs_demand_prcnt_trq = 7;
//	erc1->selection_nonengine = 8;
//	erc1->max_available_prcnt_trq = 9;
//
//	// test the numeric case
//    vector<string> expected {
//    	"ERC1 23:59:59.999 2 1 0 3 4 5 6 7 8 9"
//    };
//	interpreter->print((void*)erc1, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//	expected = {
//		"ERC1 23:59:59.999 ",
//		" Enable shift assist status 2",
//		" Enable brake assist status 1",
//		" Engine retarder torque mode 0",
//		" Actual retarder percent torque 3",
//		" Intended retarder percent torque 4",
//		" Retarder requesting brake light 5",
//		" Source address 6 (0x06)",
//		" Drivers demand retarder percent torque 7",
//		" Retarder selection Non-eng 8",
//		" Actual maximum available retarder percent torque 9",
//	};
//	interpreter->print((void*)erc1, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete erc1;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	ERC1Interpreter *interpreter = new ERC1Interpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"ERC1", "23:59:59.999", "0", "1", "2", "3", "4", "5", "6", "7", "8",
//		"9"
//    };
//	j1939_erc1_typ *erc1 = (j1939_erc1_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_erc1(erc1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
//	BOOST_CHECK(check_timestamp_equal(erc1->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete erc1;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ----------------------------- TF Interpreter ----------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_TFInterpreter )
//
//bool check_tf(
//		j1939_tf_typ *tf,
//		float clutch_pressure,
//		float oil_level,
//		float diff_pressure,
//		float oil_pressure,
//		float oil_temp) {
//	return (tf->clutch_pressure == clutch_pressure &&
//			tf->oil_level == oil_level &&
//			tf->diff_pressure == diff_pressure &&
//			tf->oil_pressure == oil_pressure &&
//			tf->oil_temp == oil_temp);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	TFInterpreter *interpreter = new TFInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, TF);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	pdu->pdu_format = 254;
//	pdu->pdu_specific = 248;
//	TFInterpreter *interpreter = new TFInterpreter();
//	j1939_tf_typ *tf;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	tf = (j1939_tf_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_tf(tf, 0, 1, 2, 3, 4));
//	BOOST_CHECK(check_timestamp_equal(tf->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete tf;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	TFInterpreter *interpreter = new TFInterpreter();
//	j1939_tf_typ *tf = new j1939_tf_typ();
//	tf->timestamp.hour = 23;
//	tf->timestamp.minute = 59;
//	tf->timestamp.second = 59;
//	tf->timestamp.millisecond = 999;
//	tf->clutch_pressure = 0;
//	tf->oil_level = 1;
//	tf->diff_pressure = 2;
//	tf->oil_pressure = 3;
//	tf->oil_temp = 4;
//
//	// test the numeric case
//    vector<string> expected {
//    	"TF 23:59:59.999 0.000 1.000 2.000 3.000 4.000"
//    };
//	interpreter->print((void*)tf, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//	expected = {
//		"TF 23:59:59.999 ",
//		" Clutch pressure 0.000",
//		" Transmission oil level 1.000",
//		" Filter differential pressure 2.000",
//		" Transmission oil pressure 3.000",
//		" Transmission oil temperature 4.000",
//	};
//	interpreter->print((void*)tf, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete tf;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	TFInterpreter *interpreter = new TFInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"TF", "23:59:59.999", "0", "1", "2", "3", "4"
//    };
//	j1939_tf_typ *tf = (j1939_tf_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_tf(tf, 0, 1, 2, 3, 4));
//	BOOST_CHECK(check_timestamp_equal(tf->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete tf;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ---------------------------- CCVS Interpreter ---------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_CCVSInterpreter )
//
//bool check_ccvs(
//		j1939_ccvs_typ *ccvs,
//		int two_spd_axle_switch,
//		int parking_brk_switch,
//		int cc_pause_switch,
//		int park_brk_release,
//		float vehicle_spd,
//		int cc_active,
//		int cc_enable_switch,
//		int brk_switch,
//		int clutch_switch,
//		int cc_set_switch,
//		int cc_coast_switch,
//		int cc_resume_switch,
//		int cc_accel_switch,
//		float cc_set_speed,
//		int pto_state,
//		int cc_state,
//		int eng_idle_incr_switch,
//		int eng_idle_decr_switch,
//		int eng_test_mode_switch,
//		int eng_shutdown_override) {
//	return (ccvs->two_spd_axle_switch == two_spd_axle_switch &&
//			ccvs->parking_brk_switch == parking_brk_switch &&
//			ccvs->cc_pause_switch == cc_pause_switch &&
//			ccvs->park_brk_release == park_brk_release &&
//			ccvs->vehicle_spd == vehicle_spd &&
//			ccvs->cc_active == cc_active &&
//			ccvs->cc_enable_switch == cc_enable_switch &&
//			ccvs->brk_switch == brk_switch &&
//			ccvs->clutch_switch == clutch_switch &&
//			ccvs->cc_set_switch == cc_set_switch &&
//			ccvs->cc_coast_switch == cc_coast_switch &&
//			ccvs->cc_resume_switch == cc_resume_switch &&
//			ccvs->cc_accel_switch == cc_accel_switch &&
//			ccvs->cc_set_speed == cc_set_speed &&
//			ccvs->pto_state == pto_state &&
//			ccvs->cc_state == cc_state &&
//			ccvs->eng_idle_incr_switch == eng_idle_incr_switch &&
//			ccvs->eng_idle_decr_switch == eng_idle_decr_switch &&
//			ccvs->eng_test_mode_switch == eng_test_mode_switch &&
//			ccvs->eng_shutdown_override == eng_shutdown_override);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	CCVSInterpreter *interpreter = new CCVSInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, CCVS);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	CCVSInterpreter *interpreter = new CCVSInterpreter();
//	j1939_ccvs_typ *ccvs;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	ccvs = (j1939_ccvs_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_ccvs(ccvs, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
//			14, 15, 16, 17, 18, 19));
//	BOOST_CHECK(check_timestamp_equal(ccvs->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete ccvs;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	CCVSInterpreter *interpreter = new CCVSInterpreter();
//	j1939_ccvs_typ *ccvs = new j1939_ccvs_typ();
//	ccvs->timestamp.hour = 23;
//	ccvs->timestamp.minute = 59;
//	ccvs->timestamp.second = 59;
//	ccvs->timestamp.millisecond = 999;
//	ccvs->two_spd_axle_switch = 0;
//	ccvs->parking_brk_switch = 1;
//	ccvs->cc_pause_switch = 2;
//	ccvs->park_brk_release = 3;
//	ccvs->vehicle_spd = 4;
//	ccvs->cc_active = 5;
//	ccvs->cc_enable_switch = 6;
//	ccvs->brk_switch = 7;
//	ccvs->clutch_switch = 8;
//	ccvs->cc_set_switch = 9;
//	ccvs->cc_coast_switch = 10;
//	ccvs->cc_resume_switch = 11;
//	ccvs->cc_accel_switch = 12;
//	ccvs->cc_set_speed = 13;
//	ccvs->pto_state = 14;
//	ccvs->cc_state = 15;
//	ccvs->eng_idle_incr_switch = 16;
//	ccvs->eng_idle_decr_switch = 17;
//	ccvs->eng_test_mode_switch = 18;
//	ccvs->eng_shutdown_override = 19;
//
//	// test the numeric case
//    vector<string> expected {
//    	"CCVS 23:59:59.999 1 3 0 4.000 8 7 2 6 5 12 11 10 9 13 15 14 19 18 17 16"
//    };
//	interpreter->print((void*)ccvs, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//	expected = {
//		"CCVS 23:59:59.999 ",
//		" Parking brake 1",
//		" Parking brake inhibit 3",
//		" Two speed axle switch 0",
//		" Vehicle speed (meters/sec) 4.000",
//		" Clutch switch 8",
//		" Brake switch 7",
//		" Cruise control pause 2",
//		" Cruise control enable 6",
//		" Cruise control active 5",
//		" Cruise control accelerate 12",
//		" Cruise control resume 11",
//		" Cruise control coast 10",
//		" Cruise control set 9",
//		" Cruise control set speed 13",
//		" Cruise control state 15",
//		" PTO state 14",
//		" Engine shutdown override 19",
//		" Engine test mode 18",
//		" Idle decrement 17",
//		" Idle increment 16",
//	};
//	interpreter->print((void*)ccvs, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete ccvs;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	CCVSInterpreter *interpreter = new CCVSInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"CCVS", "23:59:59.999", "1", "3", "0", "4.000", "8", "7", "2", "6", "5",
//		"12", "11", "10", "9", "13", "15", "14", "19", "18", "17", "16"
//    };
//	j1939_ccvs_typ *ccvs = (j1939_ccvs_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_ccvs(ccvs, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
//			14, 15, 16, 17, 18, 19));
//	BOOST_CHECK(check_timestamp_equal(ccvs->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete ccvs;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ----------------------------- LFE Interpreter ---------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_LFEInterpreter )
//
//bool check_lfe(
//		j1939_lfe_typ *lfe,
//		float eng_fuel_rate,
//		float eng_inst_fuel_economy,
//		float eng_avg_fuel_economy,
//		float eng_throttle1_pos,
//		float eng_throttle2_pos) {
//	return (lfe->eng_fuel_rate == eng_fuel_rate &&
//			lfe->eng_inst_fuel_economy == eng_inst_fuel_economy &&
//			lfe->eng_avg_fuel_economy == eng_avg_fuel_economy &&
//			lfe->eng_throttle1_pos == eng_throttle1_pos &&
//			lfe->eng_throttle2_pos == eng_throttle2_pos);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	LFEInterpreter *interpreter = new LFEInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, LFE);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	LFEInterpreter *interpreter = new LFEInterpreter();
//	j1939_lfe_typ *lfe;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	lfe = (j1939_lfe_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_lfe(lfe, 0, 1, 2, 3, 4));
//	BOOST_CHECK(check_timestamp_equal(lfe->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete lfe;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	LFEInterpreter *interpreter = new LFEInterpreter();
//	j1939_lfe_typ *lfe = new j1939_lfe_typ();
//	lfe->timestamp.hour = 23;
//	lfe->timestamp.minute = 59;
//	lfe->timestamp.second = 59;
//	lfe->timestamp.millisecond = 999;
//	lfe->eng_fuel_rate = 0;
//	lfe->eng_inst_fuel_economy = 1;
//	lfe->eng_avg_fuel_economy = 2;
//	lfe->eng_throttle1_pos = 3;
//	lfe->eng_throttle2_pos = 4;
//
//	// test the numeric case
//    vector<string> expected {
//    	"LFE 23:59:59.999 0.000 1.000 2.000 3.000 4.000"
//    };
//	interpreter->print((void*)lfe, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//	expected = {
//		"LFE 23:59:59.999 ",
//		" Fuel rate (cm3/sec) 0.000",
//		" Instantaneous fuel economy (m/cm3) 1.000",
//		" Average fuel economy (m/cm3) 2.000",
//		" Throttle 1 position (percent) 3.000",
//		" Throttle 2 position (percent) 4.000",
//	};
//	interpreter->print((void*)lfe, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete lfe;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	LFEInterpreter *interpreter = new LFEInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"LFE", "23:59:59.999", "0", "1", "2", "3", "4"
//    };
//	j1939_lfe_typ *lfe = (j1939_lfe_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_lfe(lfe, 0, 1, 2, 3, 4));
//	BOOST_CHECK(check_timestamp_equal(lfe->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete lfe;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ----------------------------- RF Interpreter ----------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_RFInterpreter )
//
//bool check_rf(
//		j1939_rf_typ *rf,
//		float pressure,
//		float oil_temp) {
//	return (rf->pressure == pressure &&
//			rf->oil_temp == oil_temp);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	RFInterpreter *interpreter = new RFInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, RF);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	RFInterpreter *interpreter = new RFInterpreter();
//	j1939_rf_typ *rf;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	rf = (j1939_rf_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_rf(rf, 0, 1));
//	BOOST_CHECK(check_timestamp_equal(rf->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete rf;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	RFInterpreter *interpreter = new RFInterpreter();
//	j1939_rf_typ *rf = new j1939_rf_typ();
//	rf->timestamp.hour = 23;
//	rf->timestamp.minute = 59;
//	rf->timestamp.second = 59;
//	rf->timestamp.millisecond = 999;
//	rf->pressure = 0;
//	rf->oil_temp = 1;
//
//	// test the numeric case
//    vector<string> expected {
//    	"RF 23:59:59.999 0 1"
//    };
//	interpreter->print((void*)rf, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//	expected = {
//		"RF 23:59:59.999 ",
//		" Hydraulic retarder pressure 0.000",
//		" Hydraulic retarder temperature 1.000",
//	};
//	interpreter->print((void*)rf, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete rf;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	RFInterpreter *interpreter = new RFInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"RF", "23:59:59.999", "0", "1"
//    };
//	j1939_rf_typ *rf = (j1939_rf_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_rf(rf, 0, 1));
//	BOOST_CHECK(check_timestamp_equal(rf->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete rf;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ----------------------------- TC1 Interpreter ---------------------------- */
///* -------------------------------------------------------------------------- */
//
////BOOST_AUTO_TEST_SUITE( test_TC1Interpreter )
////
////bool check_tc1(
////		j1939_tc1_typ *tc1,
////		int disengage_driveline,
////		int trq_conv_lockup_disable,
////		int gear_shift_inhibit,
////		float pcnt_clutch_slip,
////		int req_gear,
////		int ddl_rear_axle2,
////		int ddl_rear_axle1,
////		int ddl_front_axle2,
////		int ddl_front_axle1,
////		int ddl_central_rear,
////		int ddl_central_front,
////		int ddl_central) {
////	return (tc1->disengage_driveline == disengage_driveline &&
////			tc1->trq_conv_lockup_disable == trq_conv_lockup_disable &&
////			tc1->gear_shift_inhibit == gear_shift_inhibit &&
////			tc1->pcnt_clutch_slip == pcnt_clutch_slip &&
////			tc1->req_gear == req_gear &&
////			tc1->ddl_rear_axle2 == ddl_rear_axle2 &&
////			tc1->ddl_rear_axle1 == ddl_rear_axle1 &&
////			tc1->ddl_front_axle2 == ddl_front_axle2 &&
////			tc1->ddl_front_axle1 == ddl_front_axle1 &&
////			tc1->ddl_central_rear == ddl_central_rear &&
////			tc1->ddl_central_front == ddl_central_front &&
////			tc1->ddl_central == ddl_central);
////}
////
////BOOST_AUTO_TEST_CASE( test_pgn )
////{
////	TC1Interpreter *interpreter = new TC1Interpreter();
////	BOOST_CHECK_EQUAL(interpreter->pgn, TC1);
////	delete interpreter;
////}
////
////BOOST_AUTO_TEST_CASE( test_convert )
////{
////    BOOST_CHECK_EQUAL( add( 2,2 ), 4 );
////}
////
////BOOST_AUTO_TEST_CASE( test_print )
////{
////    BOOST_CHECK_EQUAL( add( 2,2 ), 4 );
////}
////
////BOOST_AUTO_TEST_CASE( test_import )
////{
////    BOOST_CHECK_EQUAL( add( 2,2 ), 4 );
////}
////
////BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ---------------------------- TURBO Interpreter ---------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_TURBOInterpreter )
//
//bool check_turbo(
//		j1939_turbo_typ *turbo,
//		float turbo_lube_oil_pressure,
//		float turbo_speed) {
//	return (turbo->turbo_lube_oil_pressure == turbo_lube_oil_pressure &&
//			turbo->turbo_speed == turbo_speed);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	TURBOInterpreter *interpreter = new TURBOInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, TURBO);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	TURBOInterpreter *interpreter = new TURBOInterpreter();
//	j1939_turbo_typ *turbo;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	turbo = (j1939_turbo_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_turbo(turbo, 0, 1));
//	BOOST_CHECK(check_timestamp_equal(turbo->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete turbo;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	TURBOInterpreter *interpreter = new TURBOInterpreter();
//	j1939_turbo_typ *turbo = new j1939_turbo_typ();
//	turbo->timestamp.hour = 23;
//	turbo->timestamp.minute = 59;
//	turbo->timestamp.second = 59;
//	turbo->timestamp.millisecond = 999;
//	turbo->turbo_lube_oil_pressure = 0;
//	turbo->turbo_speed = 1;
//
//	// test the numeric case
//    vector<string> expected {
//    	"TURBO 23:59:59.999 0 1"
//    };
//	interpreter->print((void*)turbo, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//	expected = {
//		"TURBO 23:59:59.999 ",
//		" Turbocharger lube oil pressure 0.00",
//		" Turbocharger speed 1.00",
//	};
//	interpreter->print((void*)turbo, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete turbo;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	TURBOInterpreter *interpreter = new TURBOInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"TURBO", "23:59:59.999", "0", "1"
//    };
//	j1939_turbo_typ *turbo = (j1939_turbo_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_turbo(turbo, 0, 1));
//	BOOST_CHECK(check_timestamp_equal(turbo->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete turbo;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ----------------------------- VD Interpreter ----------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_VDInterpreter )
//
//bool check_vd(
//		j1939_vd_typ *vd,
//		float trip_dist,
//		float tot_vehicle_dist) {
//	return (vd->trip_dist == trip_dist &&
//			vd->tot_vehicle_dist == tot_vehicle_dist);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	VDInterpreter *interpreter = new VDInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, VD);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	VDInterpreter *interpreter = new VDInterpreter();
//	j1939_vd_typ *vd;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	vd = (j1939_vd_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_vd(vd, 0, 1));
//	BOOST_CHECK(check_timestamp_equal(vd->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete vd;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	VDInterpreter *interpreter = new VDInterpreter();
//	j1939_vd_typ *vd = new j1939_vd_typ();
//	vd->timestamp.hour = 23;
//	vd->timestamp.minute = 59;
//	vd->timestamp.second = 59;
//	vd->timestamp.millisecond = 999;
//	vd->trip_dist = 0;
//	vd->tot_vehicle_dist = 1;
//
//	// test the numeric case
//    vector<string> expected {
//    	"VD 23:59:59.999 0 1"
//    };
//	interpreter->print((void*)vd, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//	expected = {
//		"VD 23:59:59.999 ",
//		" Trip distance (km) 0",
//		" Total vehicle distance (km) 1",
//	};
//	interpreter->print((void*)vd, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete vd;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	VDInterpreter *interpreter = new VDInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"VD", "23:59:59.999", "0", "1"
//    };
//	j1939_vd_typ *vd = (j1939_vd_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_vd(vd, 0, 1));
//	BOOST_CHECK(check_timestamp_equal(vd->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete vd;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ---------------------------- RCFG Interpreter ---------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_RCFGInterpreter )
//
//bool check_rcfg(
//		j1939_rcfg_typ *rcfg,
//		int retarder_type,
//		int retarder_loc,
//		int retarder_ctrl_steps,
//		float retarder_speed[5],
//		float percent_torque[5],
//		float reference_retarder_trq) {
//	// check the lists
//	for (int i=0; i<5; ++i) {
//		if (rcfg->retarder_speed[i] != retarder_speed[i] ||
//				rcfg->percent_torque[i] != percent_torque[i])
//			return false;
//	}
//	return (rcfg->retarder_type == retarder_type &&
//			rcfg->retarder_loc == retarder_loc &&
//			rcfg->retarder_ctrl_steps == retarder_ctrl_steps &&
//			rcfg->reference_retarder_trq == reference_retarder_trq);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	RCFGInterpreter *interpreter = new RCFGInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, RCFG);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	RCFGInterpreter *interpreter = new RCFGInterpreter();
//	j1939_rcfg_typ *rcfg;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	rcfg = (j1939_rcfg_typ*) interpreter->convert(pdu);
//	float retarder_speed[5] {3, 4, 5, 6, 7};
//	float percent_torque[5] {8, 9, 10, 11, 12};
//	BOOST_CHECK(check_rcfg(rcfg, 0, 1, 2, retarder_speed, percent_torque, 13));
//	BOOST_CHECK(check_timestamp_equal(rcfg->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete rcfg;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	RCFGInterpreter *interpreter = new RCFGInterpreter();
//	j1939_rcfg_typ *rcfg = new j1939_rcfg_typ();
//	rcfg->timestamp.hour = 23;
//	rcfg->timestamp.minute = 59;
//	rcfg->timestamp.second = 59;
//	rcfg->timestamp.millisecond = 999;
//	rcfg->retarder_type = 0;
//	rcfg->retarder_loc = 1;
//	rcfg->retarder_ctrl_steps = 2;
//	for (int i=0; i<5; ++i) {
//		rcfg->retarder_speed[i] = 3+i;
//		rcfg->percent_torque[i] = 8+i;
//	}
//	rcfg->reference_retarder_trq = 13;
//
//	// test the numeric case
//    vector<string> expected {
//    	"RCFG 23:59:59.999 1 0 2 3 4 5 6 7 8 9 10 11 12 13"
//    };
//	interpreter->print((void*)rcfg, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//	expected = {
//		"RCFG 23:59:59.999 ",
//		" Retarder location 1, type 0, control 2",
//		" Retarder speed 3 4 5 6 7",
//		" Percent torque 8 9 10 11 12",
//		" Reference retarder torque 13"
//	};
//	interpreter->print((void*)rcfg, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete rcfg;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	RCFGInterpreter *interpreter = new RCFGInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"RCFG", "23:59:59.999", "1", "0", "2", "3", "4", "5", "6", "7", "8",
//		"9", "10", "11", "12", "13"
//    };
//	j1939_rcfg_typ *rcfg = (j1939_rcfg_typ*) interpreter->import(tokens);
//	float retarder_speed[5] {3, 4, 5, 6, 7};
//	float percent_torque[5] {8, 9, 10, 11, 12};
//	BOOST_CHECK(check_rcfg(rcfg, 0, 1, 2, retarder_speed, percent_torque, 13));
//	BOOST_CHECK(check_timestamp_equal(rcfg->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete rcfg;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ---------------------------- ECFG Interpreter ---------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_ECFGInterpreter )
//
//bool check_ecfg(
//		j1939_ecfg_typ *ecfg,
//		float engine_spd[7],
//		float percent_trq[5],
//		float gain_endspeed_governor,
//		float reference_eng_trq,
//		float max_momentary_overide_time,
//		float spd_ctrl_lower_lim,
//		float spd_ctrl_upper_lim,
//		float trq_ctrl_lower_lim,
//		float trq_ctrl_upper_lim,
//		int receive_status) {
//	// check the lists
//	for (int i=0; i<5; ++i) {
//		if (ecfg->percent_trq[i] != percent_trq[i])
//			return false;
//	}
//	for (int i=0; i<7; ++i) {
//		if (ecfg->engine_spd[i] != engine_spd[i])
//			return false;
//	}
//	return (ecfg->gain_endspeed_governor == gain_endspeed_governor &&
//			ecfg->reference_eng_trq == reference_eng_trq &&
//			ecfg->max_momentary_overide_time == max_momentary_overide_time &&
//			ecfg->spd_ctrl_lower_lim == spd_ctrl_lower_lim &&
//			ecfg->spd_ctrl_upper_lim == spd_ctrl_upper_lim &&
//			ecfg->trq_ctrl_lower_lim == trq_ctrl_lower_lim &&
//			ecfg->trq_ctrl_upper_lim == trq_ctrl_upper_lim &&
//			ecfg->receive_status == receive_status);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	ECFGInterpreter *interpreter = new ECFGInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, ECFG);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	ECFGInterpreter *interpreter = new ECFGInterpreter();
//	j1939_ecfg_typ *ecfg;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	ecfg = (j1939_ecfg_typ*) interpreter->convert(pdu);
//	float engine_spd[7] {0, 1, 2, 3, 4, 5, 6};
//	float percent_trq[5] {7, 8, 9, 10, 11};
//	BOOST_CHECK(check_ecfg(ecfg, engine_spd, percent_trq, 12, 13, 14, 15, 16,
//			17, 18, 19));
//	BOOST_CHECK(check_timestamp_equal(ecfg->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete ecfg;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	ECFGInterpreter *interpreter = new ECFGInterpreter();
//	j1939_ecfg_typ *ecfg = new j1939_ecfg_typ();
//	ecfg->timestamp.hour = 23;
//	ecfg->timestamp.minute = 59;
//	ecfg->timestamp.second = 59;
//	ecfg->timestamp.millisecond = 999;
//	for (int i=0; i<5; ++i)
//		ecfg->percent_trq[i] = i;
//	for (int i=0; i<7; ++i)
//		ecfg->engine_spd[i] = 5+i;
//	ecfg->gain_endspeed_governor = 12;
//	ecfg->reference_eng_trq = 13;
//	ecfg->max_momentary_overide_time = 14;
//	ecfg->spd_ctrl_lower_lim = 15;
//	ecfg->spd_ctrl_upper_lim = 16;
//	ecfg->trq_ctrl_lower_lim = 17;
//	ecfg->trq_ctrl_upper_lim = 18;
//	ecfg->receive_status = 19;
//
//	// test the numeric case
//    vector<string> expected {
//    	"ECFG 23:59:59.999 0x13 5.00 6.00 7.00 8.00 9.00 10.00 11.00 0.00 1.00 "
//    	"2.00 3.00 4.00 12.00 13.00 14.00 15.00 16.00 17.00 18.00"
//    };
//	interpreter->print((void*)ecfg, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//    expected = {
//    	"ECFG 23:59:59.999 ",
//		" Engine configuration received mask 0x13",
//		" Engine speed 5.00 6.00 7.00 8.00 9.00 10.00 11.00",
//		" Percent torque 0.00 1.00 2.00 3.00 4.00",
//		" Gain endspeed governor 12.00",
//		" Reference engine torque 13.00",
//		" Max Momentary Override Time 14.00",
//		" Speed Control Lower Limit 15.00",
//		" Speed Control Upper Limit 16.00",
//		" Torque Control Lower Limit 17.00",
//		" Torque Control Upper Limit 18.00",
//    };
//	interpreter->print((void*)ecfg, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete ecfg;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	ECFGInterpreter *interpreter = new ECFGInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"ECFG", "23:59:59.999", "0x13", "5.00", "6.00", "7.00", "8.00", "9.00",
//		"10.00", "11.00", "0.00" "1.00", "2.00", "3.00", "4.00", "12.00",
//		"13.00", "14.00", "15.00", "16.00", "17.00", "18.00"
//    };
//	j1939_ecfg_typ *ecfg = (j1939_ecfg_typ*) interpreter->import(tokens);
//	float engine_spd[7] {0, 1, 2, 3, 4, 5, 6};
//	float percent_trq[5] {7, 8, 9, 10, 11};
//	BOOST_CHECK(check_ecfg(ecfg, engine_spd, percent_trq, 12, 13, 14, 15, 16,
//			17, 18, 19));
//	BOOST_CHECK(check_timestamp_equal(ecfg->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete ecfg;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* --------------------------- ETEMP Interpreter ---------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_ETEMPInterpreter )
//
//bool check_etemp(
//		j1939_etemp_typ *etemp,
//		float eng_coolant_temp,
//		float fuel_temp,
//		float eng_oil_temp,
//		float turbo_oil_temp,
//		float eng_intercooler_temp,
//		float eng_intercooler_thermostat_opening) {
//	return (etemp->eng_coolant_temp == eng_coolant_temp &&
//			etemp->fuel_temp == fuel_temp &&
//			etemp->eng_oil_temp == eng_oil_temp &&
//			etemp->turbo_oil_temp == turbo_oil_temp &&
//			etemp->eng_intercooler_temp == eng_intercooler_temp &&
//			etemp->eng_intercooler_thermostat_opening == eng_intercooler_thermostat_opening);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	ETEMPInterpreter *interpreter = new ETEMPInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, ETEMP);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	ETEMPInterpreter *interpreter = new ETEMPInterpreter();
//	j1939_etemp_typ *etemp;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	etemp = (j1939_etemp_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_etemp(etemp, 0, 1, 2, 3, 4, 5));
//	BOOST_CHECK(check_timestamp_equal(etemp->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete etemp;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	ETEMPInterpreter *interpreter = new ETEMPInterpreter();
//	j1939_etemp_typ *etemp = new j1939_etemp_typ();
//	etemp->timestamp.hour = 23;
//	etemp->timestamp.minute = 59;
//	etemp->timestamp.second = 59;
//	etemp->timestamp.millisecond = 999;
//	etemp->eng_coolant_temp = 0,
//	etemp->fuel_temp = 1;
//	etemp->eng_oil_temp = 2;
//	etemp->turbo_oil_temp = 3;
//	etemp->eng_intercooler_temp = 4;
//	etemp->eng_intercooler_thermostat_opening = 5;
//
//	// test the numeric case
//    vector<string> expected {
//    	"ETEMP 23:59:59.999 0.000 1.000 2.000 3.000 4.000 5.000"
//    };
//	interpreter->print((void*)etemp, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//    expected = {
//    	"ETEMP 23:59:59.999 ",
//		" Engine coolant temperature 0.000",
//		" Fuel temperature 1.000",
//		" Engine oil temperature 2.000",
//		" Turbo oil temperature 3.000",
//		" Engine intercooler temperature 4.000",
//		" Engine intercooler thermostat opening 5.000",
//    };
//	interpreter->print((void*)etemp, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete etemp;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	ETEMPInterpreter *interpreter = new ETEMPInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"ETEMP", "23:59:59.999", "0", "1", "2", "3", "4", "5"
//    };
//	j1939_etemp_typ *etemp = (j1939_etemp_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_etemp(etemp, 0, 1, 2, 3, 4, 5));
//	BOOST_CHECK(check_timestamp_equal(etemp->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete etemp;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ---------------------------- PTO Interpreter ----------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_PTOInterpreter )
//
//bool check_pto(
//		j1939_pto_typ *pto,
//		float oil_temp,
//		float speed,
//		float set_speed,
//		int enable_switch,
//		int remote_preprogramm_status,
//		int remote_variable_spd_status,
//		int set_switch,
//		int coast_decel_switch,
//		int resume_switch,
//		int accel_switch) {
//	return (pto->oil_temp == oil_temp &&
//			pto->speed == speed &&
//			pto->set_speed == set_speed &&
//			pto->enable_switch == enable_switch &&
//			pto->remote_preprogramm_status == remote_preprogramm_status &&
//			pto->remote_variable_spd_status == remote_variable_spd_status &&
//			pto->set_switch == set_switch &&
//			pto->coast_decel_switch == coast_decel_switch &&
//			pto->resume_switch == resume_switch &&
//			pto->accel_switch == accel_switch);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	PTOInterpreter *interpreter = new PTOInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, PTO);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	PTOInterpreter *interpreter = new PTOInterpreter();
//	j1939_pto_typ *pto;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	pto = (j1939_pto_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_pto(pto, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
//	BOOST_CHECK(check_timestamp_equal(pto->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete pto;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	PTOInterpreter *interpreter = new PTOInterpreter();
//	j1939_pto_typ *pto = new j1939_pto_typ();
//	pto->timestamp.hour = 23;
//	pto->timestamp.minute = 59;
//	pto->timestamp.second = 59;
//	pto->timestamp.millisecond = 999;
//	pto->oil_temp = 0;
//	pto->speed = 1;
//	pto->set_speed = 2;
//	pto->enable_switch = 3;
//	pto->remote_preprogramm_status = 4;
//	pto->remote_variable_spd_status = 5;
//	pto->set_switch = 6;
//	pto->coast_decel_switch = 7;
//	pto->resume_switch = 8;
//	pto->accel_switch = 9;
//
//	// test the numeric case
//    vector<string> expected {
//    	"PTO 23:59:59.999 0.000 1.000 2.000 5 4 3 9 8 7 6"
//    };
//	interpreter->print((void*)pto, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//    expected = {
//    	"PTO 23:59:59.999 ",
//		" PTO oil temperature 0.000",
//		" PTO speed 1.000",
//		" PTO set speed 2.000",
//		" Remote PTO variable speed control switch 5",
//		" Remote PTO preprogrammed speed control switch 4",
//		" PTO enable switch 3",
//		" PTO accelerate switch 9",
//		" PTO resume switch 8",
//		" PTO coast decelerate switch 7",
//		" PTO set switch 6",
//    };
//	interpreter->print((void*)pto, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete pto;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	PTOInterpreter *interpreter = new PTOInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"PTO", "23:59:59.999", "0.000", "1.000", "2.000", "5", "4", "3", "9",
//		"8", "7", "6"
//    };
//	j1939_pto_typ *pto = (j1939_pto_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_pto(pto, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
//	BOOST_CHECK(check_timestamp_equal(pto->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete pto;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ---------------------------- AMBC Interpreter ---------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_AMBCInterpreter )
//
//bool check_ambc(
//		j1939_ambc_typ *ambc,
//		float barometric_pressure,
//		float cab_interior_temp,
//		float ambient_air_temp,
//		float air_inlet_temp,
//		float road_surface_temp) {
//	return (ambc->barometric_pressure == barometric_pressure &&
//			ambc->cab_interior_temp == cab_interior_temp &&
//			ambc->ambient_air_temp == ambient_air_temp &&
//			ambc->air_inlet_temp == air_inlet_temp &&
//			ambc->road_surface_temp == road_surface_temp);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	AMBCInterpreter *interpreter = new AMBCInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, AMBC);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	AMBCInterpreter *interpreter = new AMBCInterpreter();
//	j1939_ambc_typ *ambc;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	ambc = (j1939_ambc_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_ambc(ambc, 0, 1, 2, 3, 4));
//	BOOST_CHECK(check_timestamp_equal(ambc->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete ambc;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	AMBCInterpreter *interpreter = new AMBCInterpreter();
//	j1939_ambc_typ *ambc = new j1939_ambc_typ();
//	ambc->timestamp.hour = 23;
//	ambc->timestamp.minute = 59;
//	ambc->timestamp.second = 59;
//	ambc->timestamp.millisecond = 999;
//	ambc->barometric_pressure = 0;
//	ambc->cab_interior_temp = 1;
//	ambc->ambient_air_temp = 2;
//	ambc->air_inlet_temp = 3;
//	ambc->road_surface_temp = 4;
//
//	// test the numeric case
//    vector<string> expected {
//    	"AMBC 23:59:59.999 0.000 1.000 2.000 3.000 4.000"
//    };
//	interpreter->print((void*)ambc, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//    expected = {
//    	"AMBC 23:59:59.999 ",
//		" Barometric pressure 0.000",
//		" Cab interior temperature 1.000",
//		" Ambient air temperature 2.000",
//		" Air inlet temperature 3.000",
//		" Road surface temperature 4.000",
//    };
//	interpreter->print((void*)ambc, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete ambc;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	AMBCInterpreter *interpreter = new AMBCInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"AMBC", "23:59:59.999", "0", "1", "2", "3", "4"
//    };
//	j1939_ambc_typ *ambc = (j1939_ambc_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_ambc(ambc, 0, 1, 2, 3, 4));
//	BOOST_CHECK(check_timestamp_equal(ambc->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete ambc;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ---------------------------- IEC Interpreter ----------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_IECInterpreter )
//
//bool check_iec(
//		j1939_iec_typ *iec,
//		float particulate_inlet_pressure,
//		float boost_pressure,
//		float intake_manifold_temp,
//		float air_inlet_pressure,
//		float air_filter_diff_pressure,
//		float exhaust_gas_temp,
//		float coolant_filter_diff_pressure) {
//	return (iec->particulate_inlet_pressure == particulate_inlet_pressure &&
//			iec->boost_pressure == boost_pressure &&
//			iec->intake_manifold_temp == intake_manifold_temp &&
//			iec->air_inlet_pressure == air_inlet_pressure &&
//			iec->air_filter_diff_pressure == air_filter_diff_pressure &&
//			iec->exhaust_gas_temp == exhaust_gas_temp &&
//			iec->coolant_filter_diff_pressure == coolant_filter_diff_pressure);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	IECInterpreter *interpreter = new IECInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, IEC);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	IECInterpreter *interpreter = new IECInterpreter();
//	j1939_iec_typ *iec;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	iec = (j1939_iec_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_iec(iec, 0, 1, 2, 3, 4, 5, 6));
//	BOOST_CHECK(check_timestamp_equal(iec->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete iec;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	IECInterpreter *interpreter = new IECInterpreter();
//	j1939_iec_typ *iec = new j1939_iec_typ();
//	iec->timestamp.hour = 23;
//	iec->timestamp.minute = 59;
//	iec->timestamp.second = 59;
//	iec->timestamp.millisecond = 999;
//	iec->particulate_inlet_pressure = 0;
//	iec->boost_pressure = 1;
//	iec->intake_manifold_temp = 2;
//	iec->air_inlet_pressure = 3;
//	iec->air_filter_diff_pressure = 4;
//	iec->exhaust_gas_temp = 5;
//	iec->coolant_filter_diff_pressure = 6;
//
//	// test the numeric case
//    vector<string> expected {
//    	"IEC 23:59:59.999 0.000 1.000 2.000 3.000 4.000 5.000 6.000"
//    };
//	interpreter->print((void*)iec, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//    expected = {
//    	"IEC 23:59:59.999 ",
//		" Particulate trap inlet pressure 0.000",
//		" Boost pressure 1.000",
//		" Intake manifold temperature 2.000",
//		" Air inlet pressure 3.000",
//		" Air filter differential pressure 4.000",
//		" Exhaust gas temperature 5.000",
//		" Coolant filter differential pressure 6.000",
//    };
//	interpreter->print((void*)iec, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete iec;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	IECInterpreter *interpreter = new IECInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"IEC", "23:59:59.999", "0", "1", "2", "3", "4"
//    };
//	j1939_iec_typ *iec = (j1939_iec_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_iec(iec, 0, 1, 2, 3, 4, 5, 6));
//	BOOST_CHECK(check_timestamp_equal(iec->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete iec;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ---------------------------- VEP Interpreter ----------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_VECInterpreter )
//
//bool check_vep(
//		j1939_vep_typ *vep,
//		float net_battery_current,
//		float alternator_current,
//		float alternator_potential,
//		float electrical_potential,
//		float battery_potential) {
//	return (vep->net_battery_current == net_battery_current &&
//			vep->alternator_current == alternator_current &&
//			vep->alternator_potential == alternator_potential &&
//			vep->electrical_potential == electrical_potential &&
//			vep->battery_potential == battery_potential);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	VEPInterpreter *interpreter = new VEPInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, VEP);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	VEPInterpreter *interpreter = new VEPInterpreter();
//	j1939_vep_typ *vep;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	vep = (j1939_vep_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_vep(vep, 0, 1, 2, 3, 4));
//	BOOST_CHECK(check_timestamp_equal(vep->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete vep;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	VEPInterpreter *interpreter = new VEPInterpreter();
//	j1939_vep_typ *vep = new j1939_vep_typ();
//	vep->timestamp.hour = 23;
//	vep->timestamp.minute = 59;
//	vep->timestamp.second = 59;
//	vep->timestamp.millisecond = 999;
//	vep->net_battery_current = 0;
//	vep->alternator_current = 1;
//	vep->alternator_potential = 2;
//	vep->electrical_potential = 3;
//	vep->battery_potential = 4;
//
//	// test the numeric case
//    vector<string> expected {
//    	"VEP 23:59:59.999 0.000 1.000 2.000 3.000 4.000"
//    };
//	interpreter->print((void*)vep, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//    expected = {
//    	"VEP 23:59:59.999 ",
//		" Net battery current 0.000",
//		" Alternator current 1.000",
//		" Alternator potential 2.000",
//		" Electrical potential 3.000",
//		" Battery potential 4.000",
//    };
//	interpreter->print((void*)vep, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete vep;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	IECInterpreter *interpreter = new IECInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"VEP", "23:59:59.999", "0", "1", "2", "3", "4"
//    };
//	j1939_vep_typ *vep = (j1939_vep_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_vep(vep, 0, 1, 2, 3, 4));
//	BOOST_CHECK(check_timestamp_equal(vep->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete vep;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ---------------------------- HRVD Interpreter ---------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_HRVDInterpreter )
//
//bool check_hrvd(
//		j1939_hrvd_typ *hrvd,
//		float vehicle_distance,
//		float trip_distance) {
//	return (hrvd->vehicle_distance == vehicle_distance &&
//			hrvd->trip_distance == trip_distance);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	HRVDInterpreter *interpreter = new HRVDInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, HRVD);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	HRVDInterpreter *interpreter = new HRVDInterpreter();
//	j1939_hrvd_typ *hrvd;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	hrvd = (j1939_hrvd_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_hrvd(hrvd, 0, 1));
//	BOOST_CHECK(check_timestamp_equal(hrvd->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete hrvd;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	HRVDInterpreter *interpreter = new HRVDInterpreter();
//	j1939_hrvd_typ *hrvd = new j1939_hrvd_typ();
//	hrvd->timestamp.hour = 23;
//	hrvd->timestamp.minute = 59;
//	hrvd->timestamp.second = 59;
//	hrvd->timestamp.millisecond = 999;
//	hrvd->vehicle_distance = 0;
//	hrvd->trip_distance = 1;
//
//	// test the numeric case
//    vector<string> expected {
//    	"HRVD 23:59:59.999 0.000 1.000"
//    };
//	interpreter->print((void*)hrvd, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//    expected = {
//    	"HRVD 23:59:59.999 ",
//		" Vehicle distance 0.000",
//		" Trip distance 1.000",
//    };
//	interpreter->print((void*)hrvd, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete hrvd;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	HRVDInterpreter *interpreter = new HRVDInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"HRVD", "23:59:59.999", "0", "1"
//    };
//	j1939_hrvd_typ *hrvd = (j1939_hrvd_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_hrvd(hrvd, 0, 1));
//	BOOST_CHECK(check_timestamp_equal(hrvd->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete hrvd;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ----------------------------- FD Interpreter ----------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_FDInterpreter )
//
//bool check_fd(
//		j1939_fd_typ *fd,
//		float prcnt_fan_spd,
//		int fan_drive_state) {
//	return (fd->prcnt_fan_spd == prcnt_fan_spd &&
//			fd->fan_drive_state == fan_drive_state);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	FDInterpreter *interpreter = new FDInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, FD);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	FDInterpreter *interpreter = new FDInterpreter();
//	j1939_fd_typ *fd;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	fd = (j1939_fd_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_fd(fd, 0, 1));
//	BOOST_CHECK(check_timestamp_equal(fd->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete fd;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	FDInterpreter *interpreter = new FDInterpreter();
//	j1939_fd_typ *fd = new j1939_fd_typ();
//	fd->timestamp.hour = 23;
//	fd->timestamp.minute = 59;
//	fd->timestamp.second = 59;
//	fd->timestamp.millisecond = 999;
//	fd->prcnt_fan_spd = 0;
//	fd->fan_drive_state = 1;
//
//	// test the numeric case
//    vector<string> expected {
//    	"FD 23:59:59.999 0.000 1.000"
//    };
//	interpreter->print((void*)fd, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//    expected = {
//    	"FD 23:59:59.999 ",
//		" Estimated percent fan speed 0.000",
//		" Fan drive state 1.000",
//    };
//	interpreter->print((void*)fd, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete fd;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	FDInterpreter *interpreter = new FDInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"FD", "23:59:59.999", "0", "1"
//    };
//	j1939_fd_typ *fd = (j1939_fd_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_fd(fd, 0, 1));
//	BOOST_CHECK(check_timestamp_equal(fd->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete fd;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ---------------------------- GFI2 Interpreter ---------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_GFI2Interpreter )
//
//bool check_gfi2(
//		j1939_gfi2_typ *gfi2,
//		float fuel_flow_rate1,
//		float fuel_flow_rate2,
//		float fuel_valve_pos1,
//		float fuel_valve_pos2) {
//	return (gfi2->fuel_flow_rate1 == fuel_flow_rate1 &&
//			gfi2->fuel_flow_rate2 == fuel_flow_rate2 &&
//			gfi2->fuel_valve_pos1 == fuel_valve_pos1 &&
//			gfi2->fuel_valve_pos2 == fuel_valve_pos2);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	GFI2Interpreter *interpreter = new GFI2Interpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, GFI2);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	GFI2Interpreter *interpreter = new GFI2Interpreter();
//	j1939_gfi2_typ *gfi2;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	gfi2 = (j1939_gfi2_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_gfi2(gfi2, 0, 1, 2, 3));
//	BOOST_CHECK(check_timestamp_equal(gfi2->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete gfi2;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	GFI2Interpreter *interpreter = new GFI2Interpreter();
//	j1939_gfi2_typ *gfi2 = new j1939_gfi2_typ();
//	gfi2->timestamp.hour = 23;
//	gfi2->timestamp.minute = 59;
//	gfi2->timestamp.second = 59;
//	gfi2->timestamp.millisecond = 999;
//	gfi2->fuel_flow_rate1 = 0;
//	gfi2->fuel_flow_rate2 = 1;
//	gfi2->fuel_valve_pos1 = 2;
//	gfi2->fuel_valve_pos2 = 3;
//
//	// test the numeric case
//    vector<string> expected {
//    	"GFI2 23:59:59.999 0.000 1.000 2.000 3.000"
//    };
//	interpreter->print((void*)gfi2, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//    expected = {
//    	"GFI2 23:59:59.999 ",
//		" Fuel flow rate 1 0.000",
//		" Fuel flow rate 2 1.000",
//		" Fuel valve 1 position 2.000",
//		" Fuel valve 2 position 3.000",
//    };
//	interpreter->print((void*)gfi2, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete gfi2;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	GFI2Interpreter *interpreter = new GFI2Interpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"GFI2", "23:59:59.999", "0", "1", "2", "3"
//    };
//	j1939_gfi2_typ *gfi2 = (j1939_gfi2_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_gfi2(gfi2, 0, 1, 2, 3));
//	BOOST_CHECK(check_timestamp_equal(gfi2->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete gfi2;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
//
///* -------------------------------------------------------------------------- */
///* ----------------------------- EI Interpreter ----------------------------- */
///* -------------------------------------------------------------------------- */
//
//BOOST_AUTO_TEST_SUITE( test_EIInterpreter )
//
//bool check_ei(
//		j1939_ei_typ *ei,
//		float pre_filter_oil_pressure,
//		float exhaust_gas_pressure,
//		float rack_position,
//		float eng_gas_mass_flow,
//		float inst_estimated_brake_power) {
//	return (ei->pre_filter_oil_pressure == pre_filter_oil_pressure &&
//			ei->exhaust_gas_pressure == exhaust_gas_pressure &&
//			ei->rack_position == rack_position &&
//			ei->eng_gas_mass_flow == eng_gas_mass_flow &&
//			ei->inst_estimated_brake_power == inst_estimated_brake_power);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	EIInterpreter *interpreter = new EIInterpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, EI);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//	// initialize an interpreter and PDU variable to match the data type
//	j1939_pdu_typ *pdu = new j1939_pdu_typ();
//	EIInterpreter *interpreter = new EIInterpreter();
//	j1939_ei_typ *ei;
//	vector<int> new_data_field;
//
//	// try a data-point
//	new_data_field = {0, 0, 0, 0, 0, 0, 0, 0}; // FIXME
//	for (int i=0; i<8; ++i)
//		pdu->data_field[i] = new_data_field[i];
//	ei = (j1939_ei_typ*) interpreter->convert(pdu);
//	BOOST_CHECK(check_ei(ei, 0, 1, 2, 3, 4));
//	BOOST_CHECK(check_timestamp_equal(ei->timestamp, 0, 0, 0, 0));
//
//	// free memory
//	delete pdu;
//	delete ei;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//	// initialize variables
//    FILE *fp = fopen("/tmp/test.txt", "w");
//	EIInterpreter *interpreter = new EIInterpreter();
//	j1939_ei_typ *ei = new j1939_ei_typ();
//	ei->timestamp.hour = 23;
//	ei->timestamp.minute = 59;
//	ei->timestamp.second = 59;
//	ei->timestamp.millisecond = 999;
//	ei->pre_filter_oil_pressure = 0;
//	ei->exhaust_gas_pressure = 1;
//	ei->rack_position = 2;
//	ei->eng_gas_mass_flow = 3;
//	ei->inst_estimated_brake_power = 4;
//
//	// test the numeric case
//    vector<string> expected {
//    	"EI 23:59:59.999 0.000 1.000 2.000 3.000 4.000"
//    };
//	interpreter->print((void*)ei, fp, true);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// test the non-numeric case
//    expected = {
//    	"EI 23:59:59.999 ",
//		" Pre-filter oil pressure 0.000",
//		" Exhaust gas pressure 1.000",
//		" Rack position 2.000",
//		" Natural gas mass flow 3.000",
//		" Instantaneous estimated brake power 4.000",
//    };
//	interpreter->print((void*)ei, fp, false);
//	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));
//
//	// free memory
//	delete ei;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//	// initialize variables
//	EIInterpreter *interpreter = new EIInterpreter();
//
//	// import and test something
//    vector<string> tokens {
//    	"EI", "23:59:59.999", "0", "1", "2", "3", "4"
//    };
//	j1939_ei_typ *ei = (j1939_ei_typ*) interpreter->import(tokens);
//	BOOST_CHECK(check_ei(ei, 0, 1, 2, 3, 4));
//	BOOST_CHECK(check_timestamp_equal(ei->timestamp, 23, 59, 59, 999));
//
//	// free memory
//	delete ei;
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_SUITE_END()
