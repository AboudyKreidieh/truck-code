/**
 * translate_pdu.cpp
 *
 * This script translates several pdu-formatted j1939 messages into their
 * message-specific formats, and once it is done, logs the data into an new text
 * file.
 *
 * Arguments:
 *  -v: verbose
 *  -N: numeric (?)
 *  -o: outfile path
 *  -i: infile file
 *
 * Usage:
 *      translate_pdu -i "~/j1939_brake.dbg" -o "~/parsed.txt"
 *
 * TODO(ak): replace text files with published content that this process
 * subscribes to.
 *
 * 	Created on: May 31, 2018
 * 		Author: Abdul Rahman Kreidieh
 */

#include "j1939_utils.h"
#include "j1939_struct.h"
#include "j1939_interpreters.h"
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <math.h>

using namespace std;


/** This method initializes a map used to equate a specific PGN value with an
 * interpreter class that can convert and print the messages within the PDU-
 * formatted variable. */
map<int, j1939_interpreter*> get_interpreters() {
    map<int, j1939_interpreter*> interpreters;
    interpreters.insert(make_pair(PDU, new TSC1_interpreter()));  				// FIXME(ak): add PDU interpreter
    interpreters.insert(make_pair(TSC1, new TSC1_interpreter()));
    interpreters.insert(make_pair(EXAC, new TSC1_interpreter()));  				// FIXME(ak): EXAC?
    interpreters.insert(make_pair(RQST, new TSC1_interpreter()));  				// FIXME(ak): RQST?
    interpreters.insert(make_pair(ERC1, new ERC1_interpreter()));
    interpreters.insert(make_pair(EBC1, new EBC1_interpreter()));
    interpreters.insert(make_pair(EBC2, new EBC2_interpreter()));
    interpreters.insert(make_pair(ETC1, new ETC1_interpreter()));
    interpreters.insert(make_pair(ETC2, new ETC2_interpreter()));
    interpreters.insert(make_pair(EEC1, new EEC1_interpreter()));
    interpreters.insert(make_pair(EEC2, new EEC2_interpreter()));
    interpreters.insert(make_pair(EEC3, new EEC3_interpreter()));
    interpreters.insert(make_pair(GFI2, new GFI2_interpreter()));
    interpreters.insert(make_pair(EI, new EI_interpreter()));
    interpreters.insert(make_pair(FD, new FD_interpreter()));
    interpreters.insert(make_pair(HRVD, new HRVD_interpreter()));
    interpreters.insert(make_pair(TURBO, new TURBO_interpreter()));
    interpreters.insert(make_pair(VD, new VD_interpreter()));
    interpreters.insert(make_pair(RCFG, new RCFG_interpreter()));
    interpreters.insert(make_pair(TCFG, new TSC1_interpreter()));				// FIXME(ak): TCFG?
    interpreters.insert(make_pair(ECFG, new ECFG_interpreter()));
    interpreters.insert(make_pair(ETEMP, new ETEMP_interpreter()));
    interpreters.insert(make_pair(PTO, new PTO_interpreter()));
    interpreters.insert(make_pair(CCVS, new CCVS_interpreter()));
    interpreters.insert(make_pair(LFE, new LFE_interpreter()));
    interpreters.insert(make_pair(AMBC, new AMBC_interpreter()));
    interpreters.insert(make_pair(IEC, new IEC_interpreter()));
    interpreters.insert(make_pair(VEP, new VEP_interpreter()));
    interpreters.insert(make_pair(TF, new TF_interpreter()));
    interpreters.insert(make_pair(RF, new RF_interpreter()));
    interpreters.insert(make_pair(EBC_ACC, new RF_interpreter()));  			// FIXME(ak): EBC_ACC?
    interpreters.insert(make_pair(TPDT, new RF_interpreter()));  				// FIXME(ak): TPDT?
    interpreters.insert(make_pair(TPCM, new RF_interpreter()));  				// FIXME(ak): TPDT?

    return interpreters;
}


/** The method initializes the list of all interpretable PGN values (used to
 * check if a certain PGN value is valid) */
vector<int> get_PGNs() {
    vector<int> PGNs {
        PDU, TSC1, EXAC, RQST, ERC1, EBC1, ETC1, EEC2, EEC1, ETC2, GFI2, EI, FD,
        EBC2, HRVD, TURBO, EEC3, VD, RCFG, TCFG, ECFG, ETEMP, PTO, CCVS, LFE,
        AMBC, IEC, VEP, TF, RF, EBC_ACC, TPDT, TPCM,
    };

    return PGNs;
}


/** Utility method to convert a string of representing PDU messages into a
 * j1939_pdu_typ object */
j1939_pdu_typ *convert_text_to_pdu(char s[]) {
    vector<string> tokens;
    j1939_pdu_typ *pdu = new j1939_pdu_typ();

    // separate the string by its " " delimiter
    char *pch = strtok(s, " ");
    while (pch != NULL) {
        tokens.push_back(string(pch));
        pch = strtok(NULL, " ");
    }

     for (int i=0; i<tokens.size(); ++i) {
         cout << tokens[i] << " ";
     }
     cout << endl;

    // compute the timestamp object
    string sub_s;
    timestamp_t t;
    t.hour = stoi(tokens[1].substr(0,2));
    t.minute = stoi(tokens[1].substr(3,5));
    t.second = stoi(tokens[1].substr(6,8));
    t.millisecond = stoi(tokens[1].substr(9,12));
    pdu->timestamp = t;

    // rest of the pdu variables
    pdu->priority = stoi(tokens[2]);
	pdu->pdu_format = stoi(tokens[3]);
	pdu->pdu_specific = stoi(tokens[4]);
	pdu->src_address = stoi(tokens[5]);
	pdu->num_bytes = stoi(tokens[6]);
	for (int i=0; i<pdu->num_bytes; i++)
        pdu->data_field[i] = stoi(tokens[7+i]);

    return pdu;
}

#include <cstdlib>
#include <fstream>
#include <iostream>

int main() {
    // process arguments that are inputed during execution
    FILE *fp = fopen("/tmp/truck-code/myfile.txt" , "w");
    bool numeric = true;
    int pgn_val;
    string infile = "/home/qnxuser/j1939_brake.dbg";

    // some predefined variables
    void *message;
    j1939_interpreter *interpreter;

    // collect initial variables
    map <int, j1939_interpreter*> interpreters = get_interpreters();
    vector<int> PGNs = get_PGNs();

    // for (int i=0; i<PGNs.size(); ++i) printf("%d %d %d\n", PGNs[i], PGNs[i] / (int)pow(2.0,8.0), PGNs[i] % (int)pow(2.0,8.0));
    // return 0;

    // start streaming the text file
    ifstream in(infile);
    char line[255];

    // ignore first two (header) lines
    for (int i=0; i<2; i++){
        in.getline(line, 255);
    }
	cout << "Hello world!" << endl;

    while (in) {
    	cout << "Hello world" << endl;
        // Read data from the text file
        in.getline(line, 255);

        // FIXME: check for end of text file
        if (string(&line[0], &line[7]) == "rdj1939") break;

        // convert the data to it's PDU format
        j1939_pdu_typ *pdu = convert_text_to_pdu(line);

        // get the interpreter that can process this message
        pgn_val = (int)pow(2.0,8.0) * pdu->pdu_format + pdu->pdu_specific;

        if (find(PGNs.begin(), PGNs.end(), pgn_val) == PGNs.end()) {
            // skip if the interpreter is cannot be deciphered
            continue;
        }

        interpreter = interpreters[pgn_val];

        // convert the message from its PDU format to its message-specific
        // format
        message = interpreter -> convert(pdu);

        // print the message in it's message-specific format
        interpreter->print(message, fp, numeric);

        // publish the message onto the pub/sub server
        interpreter->publish(message);
    }

	cout << "Hello world!" << endl;

    return 0;
}
