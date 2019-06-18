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
#include "tests/tests.h"
#include "jbus/j1939_utils.h"
#include "jbus/j1939_interpreters.h"
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>


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

BOOST_AUTO_TEST_CASE( test_pgn_pdu )
{
	PDUInterpreter *interpreter = new PDUInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), PDU);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_pdu )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
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
    fp = fopen("/tmp/test.txt", "w");
	expected = {
    	"PDU 23:59:59.999 0 1 2 3 8 4 5 6 7 8 9 10 11"
	};
	interpreter->print((void*)pdu, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
    fp = fopen("/tmp/test.txt", "w");
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
	fclose(fp);
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

BOOST_AUTO_TEST_CASE( test_pgn_tsc1 )
{
	TSC1Interpreter *interpreter = new TSC1Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), TSC1);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_tsc1 )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	TSC1Interpreter *interpreter = new TSC1Interpreter();
	j1939_tsc1_typ *tsc1;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0b00011011, 0, 0, 120, 0, 0, 0, 0};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	pdu->src_address = 10;
	pdu->pdu_specific = 0;
	tsc1 = (j1939_tsc1_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_tsc1(tsc1, 3, 2, 1, 0, -5, 0, 10));

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

BOOST_AUTO_TEST_CASE( test_pgn_ebc1 )
{
	EBC1Interpreter *interpreter = new EBC1Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), EBC1);
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
	new_data_field = {0b00011011, 10, 0b00011011, 0b00011011, 10, 0b00011011,
			10, 10};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	ebc1 = (j1939_ebc1_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_ebc1(ebc1, 3, 2, 1, 0, 4, 3, 2, 1, 0, 3, 2, 1, 0, 4,
			3, 2, 1, 10, -9.6));

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
		" Source address 17 (0x11)",
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

BOOST_AUTO_TEST_CASE( test_pgn_ebc2 )
{
	EBC2Interpreter *interpreter = new EBC2Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), EBC2);
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
	new_data_field = {0, 1, 2, 3, 4, 5, 6, 7};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	ebc2 = (j1939_ebc2_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_ebc2(ebc2, 0.278, -2.135, -2.118, -2.101, -2.083, -2.066,
			-2.049));

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

BOOST_AUTO_TEST_CASE( test_pgn_eec1 )
{
	EEC1Interpreter *interpreter = new EEC1Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), EEC1);
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
	new_data_field = {0b00001011, 125, 125, 100, 0, 22, 0, 125};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	eec1 = (j1939_eec1_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_eec1(eec1, 11, 0, 0, 12.5, 0, 22));

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

BOOST_AUTO_TEST_CASE( test_pgn_eec2 )
{
	EEC2Interpreter *interpreter = new EEC2Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), EEC2);
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
	new_data_field = {0b00011011, 1, 2, 3, 4, 5, 6, 7};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	eec2 = (j1939_eec2_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_eec2(eec2, 3, 2, 1, 0, 0.4, 2.0, 1.6, 2.4));

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

BOOST_AUTO_TEST_CASE( test_pgn_eec3 )
{
	EEC3Interpreter *interpreter = new EEC3Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), EEC3);
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
	new_data_field = {125, 100, 0, 100, 100, 0, 0, 0};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	eec3 = (j1939_eec3_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_eec3(eec3, 0, 12.5, 100, -25));
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

BOOST_AUTO_TEST_CASE( test_pgn_etc1 )
{
	ETC1Interpreter *interpreter = new ETC1Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), ETC1);
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
	new_data_field = {0b00011011, 100, 0, 100, 0b00001011, 100, 0, 100};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	etc1 = (j1939_etc1_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_etc1(etc1, 3, 2, 1, 12.5, 40, 3, 2, 12.5, 100));

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

BOOST_AUTO_TEST_CASE( test_pgn_etc2 )
{
	ETC2Interpreter *interpreter = new ETC2Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), ETC2);
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
	new_data_field = {125, 100, 0, 120, 11, 0, 12, 0};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	etc2 = (j1939_etc2_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_etc2(etc2, 0, 0.1, -5, 11, 12));

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

/* -------------------------------------------------------------------------- */
/* ---------------------------- ERC1 Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_ERC1Interpreter )

BOOST_AUTO_TEST_CASE( test_pgn_erc1 )
{
	ERC1Interpreter *interpreter = new ERC1Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), ERC1);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_erc1 )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	ERC1Interpreter *interpreter = new ERC1Interpreter();
	j1939_erc1_typ *erc1;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0b00011011, 0, 0, 0, 0, 0, 0, 0};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	erc1 = (j1939_erc1_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_erc1(erc1, 11, 1, 0, -125, -125, 0, 0, -125, 0, -125));

	// free memory
	delete pdu;
	delete erc1;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_erc1 )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	ERC1Interpreter *interpreter = new ERC1Interpreter();
	j1939_erc1_typ *erc1 = new j1939_erc1_typ();
	erc1->timestamp.hour = 23;
	erc1->timestamp.minute = 59;
	erc1->timestamp.second = 59;
	erc1->timestamp.millisecond = 999;
	erc1->trq_mode = 0;
	erc1->enable_brake_assist = 1;
	erc1->enable_shift_assist = 2;
	erc1->actual_ret_pcnt_trq = 3;
	erc1->intended_ret_pcnt_trq = 4;
	erc1->rq_brake_light = 5;
	erc1->src_address_ctrl = 6;
	erc1->drvrs_demand_prcnt_trq = 7;
	erc1->selection_nonengine = 8;
	erc1->max_available_prcnt_trq = 9;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"ERC1 23:59:59.999 2 1 0 3.00 4.00 5 6 7 8.00 9"
    };
	interpreter->print((void*)erc1, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
	expected = {
		"ERC1 23:59:59.999",
		" Enable shift assist status 2",
		" Enable brake assist status 1",
		" Engine retarder torque mode 0",
		" Actual retarder percent torque 3.00",
		" Intended retarder percent torque 4.00",
		" Retarder requesting brake light 5",
		" Source address 6 (0x6)",
		" Drivers demand retarder percent torque 7",
		" Retarder selection Non-eng 8.00",
		" Actual maximum available retarder percent torque 9",
	};
	interpreter->print((void*)erc1, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete erc1;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_erc1 )
{
	// initialize variables
	ERC1Interpreter *interpreter = new ERC1Interpreter();

	// import and test something
    vector<string> tokens {
    	"ERC1", "23:59:59.999", "2", "1", "0", "3", "4", "5", "6", "7", "8", "9"
    };
	j1939_erc1_typ *erc1 = (j1939_erc1_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_erc1(erc1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
	BOOST_CHECK(check_timestamp_equal(erc1->timestamp, 23, 59, 59, 999));

	// free memory
	delete erc1;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ----------------------------- TF Interpreter ----------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_TFInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_tf )
{
	TFInterpreter *interpreter = new TFInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), TF);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_tf )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	TFInterpreter *interpreter = new TFInterpreter();
	j1939_tf_typ *tf;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {10, 10, 10, 10, 0, 0, 0, 0};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	tf = (j1939_tf_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_tf(tf, 160, 4, 20, 160, -273));
	BOOST_CHECK(check_timestamp_equal(tf->timestamp, 0, 0, 0, 0));

	// free memory
	delete pdu;
	delete tf;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_tf )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	TFInterpreter *interpreter = new TFInterpreter();
	j1939_tf_typ *tf = new j1939_tf_typ();
	tf->timestamp.hour = 23;
	tf->timestamp.minute = 59;
	tf->timestamp.second = 59;
	tf->timestamp.millisecond = 999;
	tf->clutch_pressure = 0;
	tf->oil_level = 1;
	tf->diff_pressure = 2;
	tf->oil_pressure = 3;
	tf->oil_temp = 4;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"TF 23:59:59.999 0.000 1.000 2.000 3.000 4.000"
    };
	interpreter->print((void*)tf, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
	expected = {
		"TF 23:59:59.999",
		" Clutch pressure 0.000",
		" Transmission oil level 1.000",
		" Filter differential pressure 2.000",
		" Transmission oil pressure 3.000",
		" Transmission oil temperature 4.000",
	};
	interpreter->print((void*)tf, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete tf;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_tf )
{
	// initialize variables
	TFInterpreter *interpreter = new TFInterpreter();

	// import and test something
    vector<string> tokens {
    	"TF", "23:59:59.999", "0", "1", "2", "3", "4"
    };
	j1939_tf_typ *tf = (j1939_tf_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_tf(tf, 0, 1, 2, 3, 4));
	BOOST_CHECK(check_timestamp_equal(tf->timestamp, 23, 59, 59, 999));

	// free memory
	delete tf;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- CCVS Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_CCVSInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_ccvs )
{
	CCVSInterpreter *interpreter = new CCVSInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), CCVS);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_ccvs )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	CCVSInterpreter *interpreter = new CCVSInterpreter();
	j1939_ccvs_typ *ccvs;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {
			0b00011011, 0, 0, 0b00011011,
			0b00011011, 36, 0b00011011, 0b00011011};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	ccvs = (j1939_ccvs_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_ccvs(ccvs, 3, 2, 1, 0, 0, 3, 2, 1, 0, 3, 2, 1, 0, 10, 27,
			0, 3, 2, 1, 0));

	// free memory
	delete pdu;
	delete ccvs;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_ccvs )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	CCVSInterpreter *interpreter = new CCVSInterpreter();
	j1939_ccvs_typ *ccvs = new j1939_ccvs_typ();
	ccvs->timestamp.hour = 23;
	ccvs->timestamp.minute = 59;
	ccvs->timestamp.second = 59;
	ccvs->timestamp.millisecond = 999;
	ccvs->two_spd_axle_switch = 0;
	ccvs->parking_brk_switch = 1;
	ccvs->cc_pause_switch = 2;
	ccvs->park_brk_release = 3;
	ccvs->vehicle_spd = 4;
	ccvs->cc_active = 5;
	ccvs->cc_enable_switch = 6;
	ccvs->brk_switch = 7;
	ccvs->clutch_switch = 8;
	ccvs->cc_set_switch = 9;
	ccvs->cc_coast_switch = 10;
	ccvs->cc_resume_switch = 11;
	ccvs->cc_accel_switch = 12;
	ccvs->cc_set_speed = 13;
	ccvs->pto_state = 14;
	ccvs->cc_state = 15;
	ccvs->eng_idle_incr_switch = 16;
	ccvs->eng_idle_decr_switch = 17;
	ccvs->eng_test_mode_switch = 18;
	ccvs->eng_shutdown_override = 19;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"CCVS 23:59:59.999 1 3 0 4.000 8 7 2 6 5 12 11 10 9 13.000 15 14 19 18 "
    	"17 16"
    };
	interpreter->print((void*)ccvs, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
	expected = {
		"CCVS 23:59:59.999",
		" Parking brake 1",
		" Parking brake inhibit 3",
		" Two speed axle switch 0",
		" Vehicle speed (meters/sec) 4.000",
		" Clutch switch 8",
		" Brake switch 7",
		" Cruise control pause 2",
		" Cruise control enable 6",
		" Cruise control active 5",
		" Cruise control accelerate 12",
		" Cruise control resume 11",
		" Cruise control coast 10",
		" Cruise control set 9",
		" Cruise control set speed 13.000",
		" Cruise control state 15",
		" PTO state 14",
		" Engine shutdown override 19",
		" Engine test mode 18",
		" Idle decrement 17",
		" Idle increment 16",
	};
	interpreter->print((void*)ccvs, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete ccvs;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_ccvs )
{
	// initialize variables
	CCVSInterpreter *interpreter = new CCVSInterpreter();

	// import and test something
    vector<string> tokens {
    	"CCVS", "23:59:59.999", "1", "3", "0", "4.000", "8", "7", "2", "6", "5",
		"12", "11", "10", "9", "13", "15", "14", "19", "18", "17", "16"
    };
	j1939_ccvs_typ *ccvs = (j1939_ccvs_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_ccvs(ccvs, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
			14, 15, 16, 17, 18, 19));
	BOOST_CHECK(check_timestamp_equal(ccvs->timestamp, 23, 59, 59, 999));

	// free memory
	delete ccvs;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ----------------------------- LFE Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_LFEInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_lfe )
{
	LFEInterpreter *interpreter = new LFEInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), LFE);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_lfe )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	LFEInterpreter *interpreter = new LFEInterpreter();
	j1939_lfe_typ *lfe;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0, 0, 0, 0, 0, 0, 10, 10};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	lfe = (j1939_lfe_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_lfe(lfe, 0, 0, 0, 4, 4));

	// free memory
	delete pdu;
	delete lfe;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_lfe )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	LFEInterpreter *interpreter = new LFEInterpreter();
	j1939_lfe_typ *lfe = new j1939_lfe_typ();
	lfe->timestamp.hour = 23;
	lfe->timestamp.minute = 59;
	lfe->timestamp.second = 59;
	lfe->timestamp.millisecond = 999;
	lfe->eng_fuel_rate = 0;
	lfe->eng_inst_fuel_economy = 1;
	lfe->eng_avg_fuel_economy = 2;
	lfe->eng_throttle1_pos = 3;
	lfe->eng_throttle2_pos = 4;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"LFE 23:59:59.999 0.000 1.000 2.000 3.000 4.000"
    };
	interpreter->print((void*)lfe, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
	expected = {
		"LFE 23:59:59.999",
		" Fuel rate (cm3/sec) 0.000",
		" Instantaneous fuel economy (m/cm3) 1.000",
		" Average fuel economy (m/cm3) 2.000",
		" Throttle 1 position (percent) 3.000",
		" Throttle 2 position (percent) 4.000",
	};
	interpreter->print((void*)lfe, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete lfe;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_lfe )
{
	// initialize variables
	LFEInterpreter *interpreter = new LFEInterpreter();

	// import and test something
    vector<string> tokens {
    	"LFE", "23:59:59.999", "0", "1", "2", "3", "4"
    };
	j1939_lfe_typ *lfe = (j1939_lfe_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_lfe(lfe, 0, 1, 2, 3, 4));
	BOOST_CHECK(check_timestamp_equal(lfe->timestamp, 23, 59, 59, 999));

	// free memory
	delete lfe;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ----------------------------- RF Interpreter ----------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_RFInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_rf )
{
	RFInterpreter *interpreter = new RFInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), RF);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_rf )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	RFInterpreter *interpreter = new RFInterpreter();
	j1939_rf_typ *rf;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {100, 100, 0, 0, 0, 0, 0, 0};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	rf = (j1939_rf_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_rf(rf, 1600, 60));

	// free memory
	delete pdu;
	delete rf;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_rf )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	RFInterpreter *interpreter = new RFInterpreter();
	j1939_rf_typ *rf = new j1939_rf_typ();
	rf->timestamp.hour = 23;
	rf->timestamp.minute = 59;
	rf->timestamp.second = 59;
	rf->timestamp.millisecond = 999;
	rf->pressure = 0;
	rf->oil_temp = 1;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"RF 23:59:59.999 0.000 1.000"
    };
	interpreter->print((void*)rf, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
	expected = {
		"RF 23:59:59.999",
		" Hydraulic retarder pressure 0.000",
		" Hydraulic retarder temperature 1.000",
	};
	interpreter->print((void*)rf, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete rf;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_rf )
{
	// initialize variables
	RFInterpreter *interpreter = new RFInterpreter();

	// import and test something
    vector<string> tokens {
    	"RF", "23:59:59.999", "0", "1"
    };
	j1939_rf_typ *rf = (j1939_rf_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_rf(rf, 0, 1));
	BOOST_CHECK(check_timestamp_equal(rf->timestamp, 23, 59, 59, 999));

	// free memory
	delete rf;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ----------------------------- TC1 Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

//BOOST_AUTO_TEST_SUITE( test_TC1Interpreter )
//
//bool check_tc1(
//		j1939_tc1_typ *tc1,
//		int disengage_driveline,
//		int trq_conv_lockup_disable,
//		int gear_shift_inhibit,
//		float pcnt_clutch_slip,
//		int req_gear,
//		int ddl_rear_axle2,
//		int ddl_rear_axle1,
//		int ddl_front_axle2,
//		int ddl_front_axle1,
//		int ddl_central_rear,
//		int ddl_central_front,
//		int ddl_central) {
//	return (tc1->disengage_driveline == disengage_driveline &&
//			tc1->trq_conv_lockup_disable == trq_conv_lockup_disable &&
//			tc1->gear_shift_inhibit == gear_shift_inhibit &&
//			tc1->pcnt_clutch_slip == pcnt_clutch_slip &&
//			tc1->req_gear == req_gear &&
//			tc1->ddl_rear_axle2 == ddl_rear_axle2 &&
//			tc1->ddl_rear_axle1 == ddl_rear_axle1 &&
//			tc1->ddl_front_axle2 == ddl_front_axle2 &&
//			tc1->ddl_front_axle1 == ddl_front_axle1 &&
//			tc1->ddl_central_rear == ddl_central_rear &&
//			tc1->ddl_central_front == ddl_central_front &&
//			tc1->ddl_central == ddl_central);
//}
//
//BOOST_AUTO_TEST_CASE( test_pgn )
//{
//	TC1Interpreter *interpreter = new TC1Interpreter();
//	BOOST_CHECK_EQUAL(interpreter->pgn, TC1);
//	delete interpreter;
//}
//
//BOOST_AUTO_TEST_CASE( test_convert )
//{
//    BOOST_CHECK_EQUAL( add( 2,2 ), 4 );
//}
//
//BOOST_AUTO_TEST_CASE( test_print )
//{
//    BOOST_CHECK_EQUAL( add( 2,2 ), 4 );
//}
//
//BOOST_AUTO_TEST_CASE( test_import )
//{
//    BOOST_CHECK_EQUAL( add( 2,2 ), 4 );
//}
//
//BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- TURBO Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_TURBOInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_turbo )
{
	TURBOInterpreter *interpreter = new TURBOInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), TURBO);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_turbo )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	TURBOInterpreter *interpreter = new TURBOInterpreter();
	j1939_turbo_typ *turbo;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {100, 100, 0, 0, 0, 0, 0, 0};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	turbo = (j1939_turbo_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_turbo(turbo, 400, 400));

	// free memory
	delete pdu;
	delete turbo;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_turbo )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	TURBOInterpreter *interpreter = new TURBOInterpreter();
	j1939_turbo_typ *turbo = new j1939_turbo_typ();
	turbo->timestamp.hour = 23;
	turbo->timestamp.minute = 59;
	turbo->timestamp.second = 59;
	turbo->timestamp.millisecond = 999;
	turbo->turbo_lube_oil_pressure = 0;
	turbo->turbo_speed = 1;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"TURBO 23:59:59.999 0.00 1.00"
    };
	interpreter->print((void*)turbo, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
	expected = {
		"TURBO 23:59:59.999",
		" Turbocharger lube oil pressure 0.00",
		" Turbocharger speed 1.00",
	};
	interpreter->print((void*)turbo, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete turbo;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_turbo )
{
	// initialize variables
	TURBOInterpreter *interpreter = new TURBOInterpreter();

	// import and test something
    vector<string> tokens {
    	"TURBO", "23:59:59.999", "0", "1"
    };
	j1939_turbo_typ *turbo = (j1939_turbo_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_turbo(turbo, 0, 1));
	BOOST_CHECK(check_timestamp_equal(turbo->timestamp, 23, 59, 59, 999));

	// free memory
	delete turbo;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ----------------------------- VD Interpreter ----------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_VDInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_vd )
{
	VDInterpreter *interpreter = new VDInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), VD);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_vd )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	VDInterpreter *interpreter = new VDInterpreter();
	j1939_vd_typ *vd;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {1, 2, 3, 4, 5, 6, 7, 8};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	vd = (j1939_vd_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_vd(vd, 8413280, 16834752));

	// free memory
	delete pdu;
	delete vd;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_vd )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	VDInterpreter *interpreter = new VDInterpreter();
	j1939_vd_typ *vd = new j1939_vd_typ();
	vd->timestamp.hour = 23;
	vd->timestamp.minute = 59;
	vd->timestamp.second = 59;
	vd->timestamp.millisecond = 999;
	vd->trip_dist = 0;
	vd->tot_vehicle_dist = 1;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"VD 23:59:59.999 0.00 1.00"
    };
	interpreter->print((void*)vd, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
	expected = {
		"VD 23:59:59.999",
		" Trip distance (km) 0.00",
		" Total vehicle distance (km) 1.00",
	};
	interpreter->print((void*)vd, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete vd;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_vd )
{
	// initialize variables
	VDInterpreter *interpreter = new VDInterpreter();

	// import and test something
    vector<string> tokens {
    	"VD", "23:59:59.999", "0.00", "1.00"
    };
	j1939_vd_typ *vd = (j1939_vd_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_vd(vd, 0, 1));
	BOOST_CHECK(check_timestamp_equal(vd->timestamp, 23, 59, 59, 999));

	// free memory
	delete vd;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- RCFG Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_RCFGInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_rcfg )
{
	RCFGInterpreter *interpreter = new RCFGInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), RCFG);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_rcfg )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	RCFGInterpreter *interpreter = new RCFGInterpreter();
	j1939_rcfg_typ *rcfg = new j1939_rcfg_typ();
	rcfg->timestamp.hour = 23;
	rcfg->timestamp.minute = 59;
	rcfg->timestamp.second = 59;
	rcfg->timestamp.millisecond = 999;
	rcfg->retarder_type = 0;
	rcfg->retarder_loc = 1;
	rcfg->retarder_ctrl_steps = 2;
	for (int i=0; i<5; ++i) {
		rcfg->retarder_speed[i] = 3+i;
		rcfg->percent_torque[i] = 8+i;
	}
	rcfg->reference_retarder_trq = 13;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"RCFG 23:59:59.999 1 0 2 3.00 4.00 5.00 6.00 7.00 8.00 9.00 10.00 11.00"
    	" 12.00 13.00"
    };
	interpreter->print((void*)rcfg, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
	expected = {
		"RCFG 23:59:59.999",
		" Retarder location 0x1, type 0x0, control 2",
		" Retarder speed 3.00 4.00 5.00 6.00 7.00",
		" Percent torque 8.00 9.00 10.00 11.00 12.00",
		" Reference retarder torque 13.00"
	};
	interpreter->print((void*)rcfg, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete rcfg;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_rcfg )
{
	// initialize variables
	RCFGInterpreter *interpreter = new RCFGInterpreter();

	// import and test something
    vector<string> tokens {
    	"RCFG", "23:59:59.999", "1", "0", "2", "3", "4", "5", "6", "7", "8",
		"9", "10", "11", "12", "13"
    };
	j1939_rcfg_typ *rcfg = (j1939_rcfg_typ*) interpreter->import(tokens);
	float retarder_speed[5] {3, 4, 5, 6, 7};
	float percent_torque[5] {8, 9, 10, 11, 12};
	BOOST_CHECK(check_rcfg(rcfg, 0, 1, 2, retarder_speed, percent_torque, 13));
	BOOST_CHECK(check_timestamp_equal(rcfg->timestamp, 23, 59, 59, 999));

	// free memory
	delete rcfg;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- ECFG Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_ECFGInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_ecfg )
{
	ECFGInterpreter *interpreter = new ECFGInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), ECFG);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_ecfg )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	ECFGInterpreter *interpreter = new ECFGInterpreter();
	j1939_ecfg_typ *ecfg = new j1939_ecfg_typ();
	ecfg->timestamp.hour = 23;
	ecfg->timestamp.minute = 59;
	ecfg->timestamp.second = 59;
	ecfg->timestamp.millisecond = 999;
	for (int i=0; i<5; ++i)
		ecfg->percent_trq[i] = i;
	for (int i=0; i<7; ++i)
		ecfg->engine_spd[i] = 5+i;
	ecfg->gain_endspeed_governor = 12;
	ecfg->reference_eng_trq = 13;
	ecfg->max_momentary_overide_time = 14;
	ecfg->spd_ctrl_lower_lim = 15;
	ecfg->spd_ctrl_upper_lim = 16;
	ecfg->trq_ctrl_lower_lim = 17;
	ecfg->trq_ctrl_upper_lim = 18;
	ecfg->receive_status = 19;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"ECFG 23:59:59.999 0x13 5.00 6.00 7.00 8.00 9.00 10.00 11.00 0.00 1.00 "
    	"2.00 3.00 4.00 12.00 13.00 14.00 15.00 16.00 17.00 18.00"
    };
	interpreter->print((void*)ecfg, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"ECFG 23:59:59.999",
		" Engine configuration received mask 0x13",
		" Engine speed 5.00 6.00 7.00 8.00 9.00 10.00 11.00",
		" Percent torque 0.00 1.00 2.00 3.00 4.00",
		" Gain endspeed governor 12.00",
		" Reference engine torque 13.00",
		" Max Momentary Override Time 14.00",
		" Speed Control Lower Limit 15.00",
		" Speed Control Upper Limit 16.00",
		" Torque Control Lower Limit 17.00",
		" Torque Control Upper Limit 18.00",
    };
	interpreter->print((void*)ecfg, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete ecfg;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* --------------------------- ETEMP Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_ETEMPInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_etemp )
{
	ETEMPInterpreter *interpreter = new ETEMPInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), ETEMP);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_etemp )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	ETEMPInterpreter *interpreter = new ETEMPInterpreter();
	j1939_etemp_typ *etemp;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0, 40, 0, 0, 100, 0, 100, 100};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	etemp = (j1939_etemp_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_etemp(etemp, -40, 0, -273, -269.875, 60, 40));

	// free memory
	delete pdu;
	delete etemp;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_etemp )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	ETEMPInterpreter *interpreter = new ETEMPInterpreter();
	j1939_etemp_typ *etemp = new j1939_etemp_typ();
	etemp->timestamp.hour = 23;
	etemp->timestamp.minute = 59;
	etemp->timestamp.second = 59;
	etemp->timestamp.millisecond = 999;
	etemp->eng_coolant_temp = 0,
	etemp->fuel_temp = 1;
	etemp->eng_oil_temp = 2;
	etemp->turbo_oil_temp = 3;
	etemp->eng_intercooler_temp = 4;
	etemp->eng_intercooler_thermostat_opening = 5;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"ETEMP 23:59:59.999 0.000 1.000 2.000 3.000 4.000 5.000"
    };
	interpreter->print((void*)etemp, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"ETEMP 23:59:59.999",
		" Engine coolant temperature 0.000",
		" Fuel temperature 1.000",
		" Engine oil temperature 2.000",
		" Turbo oil temperature 3.000",
		" Engine intercooler temperature 4.000",
		" Engine intercooler thermostat opening 5.000",
    };
	interpreter->print((void*)etemp, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete etemp;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_etemp )
{
	// initialize variables
	ETEMPInterpreter *interpreter = new ETEMPInterpreter();

	// import and test something
    vector<string> tokens {
    	"ETEMP", "23:59:59.999", "0", "1", "2", "3", "4", "5"
    };
	j1939_etemp_typ *etemp = (j1939_etemp_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_etemp(etemp, 0, 1, 2, 3, 4, 5));
	BOOST_CHECK(check_timestamp_equal(etemp->timestamp, 23, 59, 59, 999));

	// free memory
	delete etemp;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- PTO Interpreter ----------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_PTOInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_pto )
{
	PTOInterpreter *interpreter = new PTOInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), PTO);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_pto )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	PTOInterpreter *interpreter = new PTOInterpreter();
	j1939_pto_typ *pto;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {100, 100, 0, 50, 0, 0b01101100, 0b00011011, 0};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	pto = (j1939_pto_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_pto(pto, 60, 12.5, 6.25, 0, 3, 2, 3, 2, 1, 0));

	// free memory
	delete pdu;
	delete pto;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_pto )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	PTOInterpreter *interpreter = new PTOInterpreter();
	j1939_pto_typ *pto = new j1939_pto_typ();
	pto->timestamp.hour = 23;
	pto->timestamp.minute = 59;
	pto->timestamp.second = 59;
	pto->timestamp.millisecond = 999;
	pto->oil_temp = 0;
	pto->speed = 1;
	pto->set_speed = 2;
	pto->enable_switch = 3;
	pto->remote_preprogramm_status = 4;
	pto->remote_variable_spd_status = 5;
	pto->set_switch = 6;
	pto->coast_decel_switch = 7;
	pto->resume_switch = 8;
	pto->accel_switch = 9;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"PTO 23:59:59.999 0.000 1.000 2.000 5 4 3 9 8 7 6"
    };
	interpreter->print((void*)pto, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"PTO 23:59:59.999",
		" PTO oil temperature 0.000",
		" PTO speed 1.000",
		" PTO set speed 2.000",
		" Remote PTO variable speed control switch 5",
		" Remote PTO preprogrammed speed control switch 4",
		" PTO enable switch 3",
		" PTO accelerate switch 9",
		" PTO resume switch 8",
		" PTO coast decelerate switch 7",
		" PTO set switch 6",
    };
	interpreter->print((void*)pto, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete pto;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_pto )
{
	// initialize variables
	PTOInterpreter *interpreter = new PTOInterpreter();

	// import and test something
    vector<string> tokens {
    	"PTO", "23:59:59.999", "0.000", "1.000", "2.000", "5", "4", "3", "9",
		"8", "7", "6"
    };
	j1939_pto_typ *pto = (j1939_pto_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_pto(pto, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9));
	BOOST_CHECK(check_timestamp_equal(pto->timestamp, 23, 59, 59, 999));

	// free memory
	delete pto;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- AMBC Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_AMBCInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_ambc )
{
	AMBCInterpreter *interpreter = new AMBCInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), AMBC);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_ambc )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	AMBCInterpreter *interpreter = new AMBCInterpreter();
	j1939_ambc_typ *ambc;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {100, 0, 0, 0, 0, 0, 0, 0};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	ambc = (j1939_ambc_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_ambc(ambc, 50, -273, -273, -40, -273));

	// free memory
	delete pdu;
	delete ambc;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_ambc )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	AMBCInterpreter *interpreter = new AMBCInterpreter();
	j1939_ambc_typ *ambc = new j1939_ambc_typ();
	ambc->timestamp.hour = 23;
	ambc->timestamp.minute = 59;
	ambc->timestamp.second = 59;
	ambc->timestamp.millisecond = 999;
	ambc->barometric_pressure = 0;
	ambc->cab_interior_temp = 1;
	ambc->ambient_air_temp = 2;
	ambc->air_inlet_temp = 3;
	ambc->road_surface_temp = 4;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"AMBC 23:59:59.999 0.000 1.000 2.000 3.000 4.000"
    };
	interpreter->print((void*)ambc, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"AMBC 23:59:59.999",
		" Barometric pressure 0.000",
		" Cab interior temperature 1.000",
		" Ambient air temperature 2.000",
		" Air inlet temperature 3.000",
		" Road surface temperature 4.000",
    };
	interpreter->print((void*)ambc, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete ambc;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_ambc )
{
	// initialize variables
	AMBCInterpreter *interpreter = new AMBCInterpreter();

	// import and test something
    vector<string> tokens {
    	"AMBC", "23:59:59.999", "0", "1", "2", "3", "4"
    };
	j1939_ambc_typ *ambc = (j1939_ambc_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_ambc(ambc, 0, 1, 2, 3, 4));
	BOOST_CHECK(check_timestamp_equal(ambc->timestamp, 23, 59, 59, 999));

	// free memory
	delete ambc;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- IEC Interpreter ----------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_IECInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_iec )
{
	IECInterpreter *interpreter = new IECInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), IEC);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_iec )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	IECInterpreter *interpreter = new IECInterpreter();
	j1939_iec_typ *iec;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {100, 100, 100, 100, 100, 0, 0, 100};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	iec = (j1939_iec_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_iec(iec, 50, 200, 60, 200, 5, -273, 50));

	// free memory
	delete pdu;
	delete iec;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_iec )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	IECInterpreter *interpreter = new IECInterpreter();
	j1939_iec_typ *iec = new j1939_iec_typ();
	iec->timestamp.hour = 23;
	iec->timestamp.minute = 59;
	iec->timestamp.second = 59;
	iec->timestamp.millisecond = 999;
	iec->particulate_inlet_pressure = 0;
	iec->boost_pressure = 1;
	iec->intake_manifold_temp = 2;
	iec->air_inlet_pressure = 3;
	iec->air_filter_diff_pressure = 4;
	iec->exhaust_gas_temp = 5;
	iec->coolant_filter_diff_pressure = 6;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"IEC 23:59:59.999 0.000 1.000 2.000 3.000 4.000 5.000 6.000"
    };
	interpreter->print((void*)iec, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"IEC 23:59:59.999",
		" Particulate trap inlet pressure 0.000",
		" Boost pressure 1.000",
		" Intake manifold temperature 2.000",
		" Air inlet pressure 3.000",
		" Air filter differential pressure 4.000",
		" Exhaust gas temperature 5.000",
		" Coolant filter differential pressure 6.000",
    };
	interpreter->print((void*)iec, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete iec;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_iec )
{
	// initialize variables
	IECInterpreter *interpreter = new IECInterpreter();

	// import and test something
    vector<string> tokens {
    	"IEC", "23:59:59.999", "0", "1", "2", "3", "4", "5", "6"
    };
	j1939_iec_typ *iec = (j1939_iec_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_iec(iec, 0, 1, 2, 3, 4, 5, 6));
	BOOST_CHECK(check_timestamp_equal(iec->timestamp, 23, 59, 59, 999));

	// free memory
	delete iec;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- VEP Interpreter ----------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_VEPInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_vep )
{
	VEPInterpreter *interpreter = new VEPInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), VEP);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_vep )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	VEPInterpreter *interpreter = new VEPInterpreter();
	j1939_vep_typ *vep;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {15, 15, 1, 0, 2, 0, 3, 0};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	vep = (j1939_vep_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_vep(vep, -110, 15, 0.05, 0.1, 0.15));

	// free memory
	delete pdu;
	delete vep;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_vep )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	VEPInterpreter *interpreter = new VEPInterpreter();
	j1939_vep_typ *vep = new j1939_vep_typ();
	vep->timestamp.hour = 23;
	vep->timestamp.minute = 59;
	vep->timestamp.second = 59;
	vep->timestamp.millisecond = 999;
	vep->net_battery_current = 0;
	vep->alternator_current = 1;
	vep->alternator_potential = 2;
	vep->electrical_potential = 3;
	vep->battery_potential = 4;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"VEP 23:59:59.999 0.000 1.000 2.000 3.000 4.000"
    };
	interpreter->print((void*)vep, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"VEP 23:59:59.999",
		" Net battery current 0.000",
		" Alternator current 1.000",
		" Alternator potential 2.000",
		" Electrical potential 3.000",
		" Battery potential 4.000",
    };
	interpreter->print((void*)vep, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete vep;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_vep )
{
	// initialize variables
	VEPInterpreter *interpreter = new VEPInterpreter();

	// import and test something
    vector<string> tokens {
    	"VEP", "23:59:59.999", "0", "1", "2", "3", "4"
    };
	j1939_vep_typ *vep = (j1939_vep_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_vep(vep, 0, 1, 2, 3, 4));
	BOOST_CHECK(check_timestamp_equal(vep->timestamp, 23, 59, 59, 999));

	// free memory
	delete vep;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- HRVD Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_HRVDInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_hrvd )
{
	HRVDInterpreter *interpreter = new HRVDInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), HRVD);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_hrvd )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	HRVDInterpreter *interpreter = new HRVDInterpreter();
	j1939_hrvd_typ *hrvd;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {1, 2, 3, 4, 5, 6, 7, 8};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	hrvd = (j1939_hrvd_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_hrvd(hrvd, 336531.22, 673390.13));

	// free memory
	delete pdu;
	delete hrvd;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_hrvd )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	HRVDInterpreter *interpreter = new HRVDInterpreter();
	j1939_hrvd_typ *hrvd = new j1939_hrvd_typ();
	hrvd->timestamp.hour = 23;
	hrvd->timestamp.minute = 59;
	hrvd->timestamp.second = 59;
	hrvd->timestamp.millisecond = 999;
	hrvd->vehicle_distance = 0;
	hrvd->trip_distance = 1;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"HRVD 23:59:59.999 0.000 1.000"
    };
	interpreter->print((void*)hrvd, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"HRVD 23:59:59.999",
		" Vehicle distance 0.000",
		" Trip distance 1.000",
    };
	interpreter->print((void*)hrvd, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete hrvd;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_hrvd )
{
	// initialize variables
	HRVDInterpreter *interpreter = new HRVDInterpreter();

	// import and test something
    vector<string> tokens {
    	"HRVD", "23:59:59.999", "0", "1"
    };
	j1939_hrvd_typ *hrvd = (j1939_hrvd_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_hrvd(hrvd, 0, 1));
	BOOST_CHECK(check_timestamp_equal(hrvd->timestamp, 23, 59, 59, 999));

	// free memory
	delete hrvd;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ----------------------------- FD Interpreter ----------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_FDInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_fd )
{
	FDInterpreter *interpreter = new FDInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), FD);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_fd )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	FDInterpreter *interpreter = new FDInterpreter();
	j1939_fd_typ *fd;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {100, 0b00001011, 0, 0, 0, 0, 0, 0};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	fd = (j1939_fd_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_fd(fd, 40, 11));

	// free memory
	delete pdu;
	delete fd;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_fd )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	FDInterpreter *interpreter = new FDInterpreter();
	j1939_fd_typ *fd = new j1939_fd_typ();
	fd->timestamp.hour = 23;
	fd->timestamp.minute = 59;
	fd->timestamp.second = 59;
	fd->timestamp.millisecond = 999;
	fd->prcnt_fan_spd = 0;
	fd->fan_drive_state = 1;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"FD 23:59:59.999 0.000 1"
    };
	interpreter->print((void*)fd, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"FD 23:59:59.999",
		" Estimated percent fan speed 0.000",
		" Fan drive state 1",
    };
	interpreter->print((void*)fd, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete fd;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_fd )
{
	// initialize variables
	FDInterpreter *interpreter = new FDInterpreter();

	// import and test something
    vector<string> tokens {
    	"FD", "23:59:59.999", "0.000", "1"
    };
	j1939_fd_typ *fd = (j1939_fd_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_fd(fd, 0, 1));
	BOOST_CHECK(check_timestamp_equal(fd->timestamp, 23, 59, 59, 999));

	// free memory
	delete fd;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ---------------------------- GFI2 Interpreter ---------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_GFI2Interpreter )

BOOST_AUTO_TEST_CASE( test_pgn_gfi2 )
{
	GFI2Interpreter *interpreter = new GFI2Interpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), GFI2);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_gfi2 )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	GFI2Interpreter *interpreter = new GFI2Interpreter();
	j1939_gfi2_typ *gfi2;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {0, 100, 100, 0, 10, 50, 0, 0};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	gfi2 = (j1939_gfi2_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_gfi2(gfi2, 2560, 10, 4, 20));

	// free memory
	delete pdu;
	delete gfi2;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_gfi2 )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
	GFI2Interpreter *interpreter = new GFI2Interpreter();
	j1939_gfi2_typ *gfi2 = new j1939_gfi2_typ();
	gfi2->timestamp.hour = 23;
	gfi2->timestamp.minute = 59;
	gfi2->timestamp.second = 59;
	gfi2->timestamp.millisecond = 999;
	gfi2->fuel_flow_rate1 = 0;
	gfi2->fuel_flow_rate2 = 1;
	gfi2->fuel_valve_pos1 = 2;
	gfi2->fuel_valve_pos2 = 3;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"GFI2 23:59:59.999 0.000 1.000 2.000 3.000"
    };
	interpreter->print((void*)gfi2, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
    expected = {
    	"GFI2 23:59:59.999",
		" Fuel flow rate 1 0.000",
		" Fuel flow rate 2 1.000",
		" Fuel valve 1 position 2.000",
		" Fuel valve 2 position 3.000",
    };
	interpreter->print((void*)gfi2, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete gfi2;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_gfi2 )
{
	// initialize variables
	GFI2Interpreter *interpreter = new GFI2Interpreter();

	// import and test something
    vector<string> tokens {
    	"GFI2", "23:59:59.999", "0", "1", "2", "3"
    };
	j1939_gfi2_typ *gfi2 = (j1939_gfi2_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_gfi2(gfi2, 0, 1, 2, 3));
	BOOST_CHECK(check_timestamp_equal(gfi2->timestamp, 23, 59, 59, 999));

	// free memory
	delete gfi2;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()

/* -------------------------------------------------------------------------- */
/* ----------------------------- EI Interpreter ----------------------------- */
/* -------------------------------------------------------------------------- */

BOOST_AUTO_TEST_SUITE( test_EIInterpreter )

BOOST_AUTO_TEST_CASE( test_pgn_ei )
{
	EIInterpreter *interpreter = new EIInterpreter();
	BOOST_CHECK_EQUAL(interpreter->pgn(), EI);
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_convert_ei )
{
	// initialize an interpreter and PDU variable to match the data type
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	EIInterpreter *interpreter = new EIInterpreter();
	j1939_ei_typ *ei;
	vector<int> new_data_field;

	// try a data-point
	new_data_field = {100, 128, 0, 100, 100, 0, 100, 0};
	for (int i=0; i<8; ++i)
		pdu->data_field[i] = new_data_field[i];
	ei = (j1939_ei_typ*) interpreter->convert(pdu);
	BOOST_CHECK(check_ei(ei, 400, -249, 40, 5, 50));

	// free memory
	delete pdu;
	delete ei;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_print_ei )
{
	// initialize variables
	FILE *fp;
	vector<string> expected;
    EIInterpreter *interpreter = new EIInterpreter();
	j1939_ei_typ *ei = new j1939_ei_typ();
	ei->timestamp.hour = 23;
	ei->timestamp.minute = 59;
	ei->timestamp.second = 59;
	ei->timestamp.millisecond = 999;
	ei->pre_filter_oil_pressure = 0;
	ei->exhaust_gas_pressure = 1;
	ei->rack_position = 2;
	ei->eng_gas_mass_flow = 3;
	ei->inst_estimated_brake_power = 4;

	// test the numeric case
	fp = fopen("/tmp/test.txt", "w");
	expected = {
    	"EI 23:59:59.999 0.000 1.000 2.000 3.000 4.000"
    };
	interpreter->print((void*)ei, fp, true);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// test the non-numeric case
	fp = fopen("/tmp/test.txt", "w");
	expected = {
    	"EI 23:59:59.999",
		" Pre-filter oil pressure 0.000",
		" Exhaust gas pressure 1.000",
		" Rack position 2.000",
		" Natural gas mass flow 3.000",
		" Instantaneous estimated brake power 4.000",
    };
	interpreter->print((void*)ei, fp, false);
	fclose(fp);
	BOOST_CHECK(check_import_match("/tmp/test.txt", expected));

	// free memory
	delete ei;
	delete interpreter;
}

BOOST_AUTO_TEST_CASE( test_import_ei )
{
	// initialize variables
	EIInterpreter *interpreter = new EIInterpreter();

	// import and test something
    vector<string> tokens {
    	"EI", "23:59:59.999", "0", "1", "2", "3", "4"
    };
	j1939_ei_typ *ei = (j1939_ei_typ*) interpreter->import(tokens);
	BOOST_CHECK(check_ei(ei, 0, 1, 2, 3, 4));
	BOOST_CHECK(check_timestamp_equal(ei->timestamp, 23, 59, 59, 999));

	// free memory
	delete ei;
	delete interpreter;
}

BOOST_AUTO_TEST_SUITE_END()
