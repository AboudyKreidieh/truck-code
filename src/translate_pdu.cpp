/**\file
 *
 * translate_pdu.cpp
 *
 * This script translates several PDU-formatted J1939 messages into their
 * message-specific formats, and once it is done, logs the data into an new text
 * file.
 *
 * Arguments:
 *
 * - -v: verbose
 * - -n: numeric
 * - -o: outfile path
 *
 * Usage:
 *  translate_pdu "~/j1939_brake.dbg" -o "~/parsed.txt"
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date May 31, 2018
 */

#include "jbus/j1939_utils.h"
#include "jbus/j1939_struct.h"
#include "jbus/j1939_interpreters.h"
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <fcntl.h>
#include <cstdlib>
#include <iostream>
#include <stdio.h>

using namespace std;


/* Create a mapping from the names of messages to their PGN values.
 *
 * This method initializes a map used to equate a specific PGN value with an
 * interpreter class that can convert and print the messages within the PDU-
 * formatted variable.
 */
static map<string, int> get_pgn_by_name() {
    map<string, int> pgn_by_name;
    pgn_by_name.insert(make_pair("PDU", PDU));
    pgn_by_name.insert(make_pair("TSC1", TSC1));
    pgn_by_name.insert(make_pair("EXAC", EXAC));
    pgn_by_name.insert(make_pair("RQST", RQST));
    pgn_by_name.insert(make_pair("ERC1", ERC1));
    pgn_by_name.insert(make_pair("EBC1", EBC1));
    pgn_by_name.insert(make_pair("EBC2", EBC2));
    pgn_by_name.insert(make_pair("ETC1", ETC1));
    pgn_by_name.insert(make_pair("ETC2", ETC2));
    pgn_by_name.insert(make_pair("EEC1", EEC1));
    pgn_by_name.insert(make_pair("EEC2", EEC2));
    pgn_by_name.insert(make_pair("EEC3", EEC3));
    pgn_by_name.insert(make_pair("GFI2", GFI2));
    pgn_by_name.insert(make_pair("EI", EI));
    pgn_by_name.insert(make_pair("FD", FD));
    pgn_by_name.insert(make_pair("HRVD", HRVD));
    pgn_by_name.insert(make_pair("TURBO", TURBO));
    pgn_by_name.insert(make_pair("VD", VD));
    pgn_by_name.insert(make_pair("RCFG", RCFG));
    pgn_by_name.insert(make_pair("TCFG", TCFG));
    pgn_by_name.insert(make_pair("ECFG", ECFG));
    pgn_by_name.insert(make_pair("ETEMP", ETEMP));
    pgn_by_name.insert(make_pair("PTO", PTO));
    pgn_by_name.insert(make_pair("CCVS", CCVS));
    pgn_by_name.insert(make_pair("LFE", LFE));
    pgn_by_name.insert(make_pair("AMBC", AMBC));
    pgn_by_name.insert(make_pair("IEC", IEC));
    pgn_by_name.insert(make_pair("VEP", VEP));
    pgn_by_name.insert(make_pair("TF", TF));
    pgn_by_name.insert(make_pair("RF", RF));

    return pgn_by_name;
}


/* Interpretable parameter group number values, used to check if a certain PGN
 * value is valid */
static vector<int> PGNs {
	PDU, TSC1, EXAC, RQST, ERC1, EBC1, ETC1, EEC2, EEC1, ETC2, GFI2, EI, FD,
	EBC2, HRVD, TURBO, EEC3, VD, RCFG, TCFG, ECFG, ETEMP, PTO, CCVS, LFE,
	AMBC, IEC, VEP, TF, RF,
};


/* Names of the interpretable messages */
static vector<string> names {
	"PDU", "TSC1", "EXAC", "RQST", "ERC1", "EBC1", "ETC1", "EEC2", "EEC1",
	"ETC2", "GFI2", "EI", "FD", "EBC2", "HRVD", "TURBO", "EEC3", "VD",
	"RCFG", "TCFG", "ECFG", "ETEMP", "PTO", "CCVS", "LFE", "AMBC", "IEC",
	"VEP", "TF", "RF", "TPDT", "TPCM"
};


static void show_usage(std::string name) {
    cout << "Usage: " << name << " <option(s)> INFILE\n"
    	 << "Options:\n"
         << "\t-o OUTFILE\tSpecify the destination to stored the data.\n"
         << "\t-n\t\tNumeric (less expressive and more compact outfile)\n"
         << "\t-v\t\tVerbose\n"
         << endl;
}


int main(int argc, char* argv[]) {
    bool numeric = false;
    bool verbose = false;
    string outfile = "j1939_output.txt";

    /* we expect 2 arguments: the program name and the input filepath */
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << "INFILE" << endl;
        return 1;
    }
    string infile = argv[1];

    /* process arguments that are inputed during execution */
    for (int i=2; i<argc; ++i) {
        string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        } else if (arg == "-o") {
			if (i + 1 < argc) {
				outfile = argv[++i];
			} else {
				cerr << "-o option requires one argument." << endl;
				return 1;
			}
        } else if (arg == "-n") {
        	numeric = true;
        } else if (arg == "-v") {
        	verbose = true;
        } else {
			cerr << arg << " argument is not known." << endl;
			return 1;
        }
    }

    /* some predefined variables */
    FILE *fp = fopen(outfile.c_str(), "w");
    map <int, J1939Interpreter*> interpreters = get_interpreters();
    map<string, int> pgn_by_name = get_pgn_by_name();
    int pgn_val;
    void *message;

    /* start streaming the text file */
    ifstream in(infile);
    char line[255];
    string pgn_type;

    while (in) {
        /* read data from the text file */
        in.getline(line, 255);

        /* separate the string by its " " delimiter */
        vector<string> tokens;
        char *pch = strtok(line, " ");
        int i = 0;
        bool flag = false;
        while (pch != NULL) {
        	/* check if the message can be deciphered */
        	if (i == 0 && find(names.begin(), names.end(),
        			string(pch)) == names.end()) {
				flag = true;
				break;
        	}
        	/* store the next component of the message in a list of strings */
            tokens.push_back(string(pch));
            pch = strtok(NULL, " ");
        	i++;
        }

        /* skip if the interpreter is cannot be deciphered */
        if (flag) continue;

        /* check for empty line */
        if (tokens.size() == 0) continue;

        /* If the message was not in it's PDU format, convert it. Otherwise,
         * determine the interpreter for printing and publishing from the first
         * token term (i.e. the name of the message) */
        if (tokens[0] =="PDU") {
        	j1939_pdu_typ *pdu = (j1939_pdu_typ*) interpreters[PDU]->import(tokens);

            /* compute the PGN value from the PDU format and specific terms */
            pgn_val = TWOBYTES(pdu->pdu_format, pdu->pdu_specific);

            /* skip if the interpreter is cannot be deciphered */
            if (find(PGNs.begin(), PGNs.end(), pgn_val) == PGNs.end())
                continue;

            /* convert the message to its message-specific format */
            message = interpreters[pgn_val]->convert(pdu);
        } else {
            /* get the PGN value from the name of the message */
            pgn_val = pgn_by_name[tokens[0]];

            /* import the message */
            message = interpreters[pgn_val]->import(tokens);
        }

        /* print the message in it's processed format to the output file */
        interpreters[pgn_val]->print(message, fp, numeric);

        /* if verbose, print to stdout */
        if (verbose)
        	interpreters[pgn_val]->print(message, stdout, numeric);
    }

    return 0;
}
