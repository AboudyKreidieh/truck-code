/**\file
 *
 * test_translate_pdu.cpp
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 13, 2019
 */

#define BOOST_TEST_MODULE "test_translate_pdu"
#include <boost/test/unit_test.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

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


BOOST_AUTO_TEST_SUITE( MasterSuite )

BOOST_AUTO_TEST_CASE( run_test )
{
	vector<string> expected;

	// test the numeric case
	system("../../../../translate_pdu ../../../../tests/data/j1939_brake.dbg -o /tmp/translate_numeric.txt -n");
	expected = {
		"TSC1 16:03:12.074 0 11 3 3 3 8031.875 -255.000",
		"EBC1 16:03:12.084 0 0 0 0 0.00 3 3 3 0 3 3 3 3 -255.00 1 0 3 11 254.000",
		"EBC2 16:03:12.085 0.000 0.000 0.000 0.000 0.000 -255.000 -255.000"
	};
	BOOST_CHECK(check_import_match("/tmp/translate_numeric.txt", expected));

	// test the non-numeric case
	system("../../../../translate_pdu ../../../../tests/data/j1939_brake.dbg -o /tmp/translate.txt");
	expected = {
		"TSC1 16:03:12.074",
		" Destination 0 ",
		" Source address 11",
		" Override Control Mode priority 3",
		" Requested speed control conditions 3",
		" Override control mode 3",
		" Requested speed/speed limit  8031.875",
		" Requested torque/torque limit -255.000",
		"EBC1 16:03:12.084",
		" EBS brake switch status 0 ",
		" ABS active status 0 ",
		" ASR brake control status 0 ",
		" ASR engine control active status 0 ",
		" Brake pedal position 0.00",
		" Traction control override switch status 3 ",
		" Hill holder switch status 3 ",
		" ABS off road switch status 3 ",
		" ASR off road switch status 0 ",
		" Remote accelerator enable switch status 3 ",
		" Auxiliary engine shutdown switch status 3 ",
		" Engine derate switch status 3 ",
		" Accelerator interlock switch status 3 ",
		" Percent engine retarder torque selected -255.00 ",
		" ABS/EBS amber warning state 1 ",
		" EBS red warning state 0 ",
		" ABS fully operational 3 ",
		" Source address 11 (0xb)",
		" Total brake demand 254.000",
		"EBC2 16:03:12.085",
		" Front axle speed 0.000",
		" Front left wheel relative speed 0.000",
		" Front right wheel relative speed 0.000",
		" Rear 1 left wheel relative speed 0.000",
		" Rear 1 left wheel relative speed 0.000",
		" Rear 2 left wheel relative speed -255.000",
		" Rear 2 left wheel relative speed -255.000",
	};
	BOOST_CHECK(check_import_match("/tmp/translate.txt", expected));
}

BOOST_AUTO_TEST_SUITE_END()
