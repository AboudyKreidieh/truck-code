///**\file
// *
// * translate_pdu.cpp
// *
// * This script translates several pdu-formatted j1939 messages into their
// * message-specific formats, and once it is done, logs the data into an new text
// * file.
// *
// * Arguments:
// *  -v: verbose
// *  -n: numeric (?)
// *  -o: outfile path
// *  -i: infile file
// *
// * Usage:
// *      translate_pdu -i "~/j1939_brake.dbg" -o "~/parsed.txt"
// *
// * TODO(ak): replace text files with published content that this process
// * subscribes to.
// *
// * @author Abdul Rahman Kreidieh
// * @version 1.0.0
// * @date May 31, 2018
// */
//
//#include "j1939_utils.h"
//#include "j1939_struct.h"
//#include "j1939_interpreters.h"
//#include <vector>
//#include <map>
//#include <fstream>
//#include <string>
//#include <string.h>
//#include <iostream>
//#include <algorithm>
//#include <math.h>
//#include <fcntl.h>
//#include <cstdlib>
//#include <iostream>
//
//using namespace std;
//
//
///** This method initializes a map used to equate a specific PGN value with an
// * interpreter class that can convert and print the messages within the PDU-
// * formatted variable. */
//map<int, J1939Interpreter*> get_interpreters() {
//    map<int, J1939Interpreter*> interpreters;
//    interpreters.insert(make_pair(PDU, new PDUInterpreter()));
//    interpreters.insert(make_pair(TSC1, new TSC1Interpreter()));
//    interpreters.insert(make_pair(EXAC, new PDUInterpreter()));  				// FIXME(ak): EXAC?
//    interpreters.insert(make_pair(RQST, new PDUInterpreter()));  				// FIXME(ak): RQST?
//    interpreters.insert(make_pair(ERC1, new ERC1Interpreter()));
//    interpreters.insert(make_pair(EBC1, new EBC1Interpreter()));
//    interpreters.insert(make_pair(EBC2, new EBC2Interpreter()));
//    interpreters.insert(make_pair(ETC1, new ETC1Interpreter()));
//    interpreters.insert(make_pair(ETC2, new ETC2Interpreter()));
//    interpreters.insert(make_pair(EEC1, new EEC1Interpreter()));
//    interpreters.insert(make_pair(EEC2, new EEC2Interpreter()));
//    interpreters.insert(make_pair(EEC3, new EEC3Interpreter()));
//    interpreters.insert(make_pair(GFI2, new GFI2Interpreter()));
//    interpreters.insert(make_pair(EI, new EIInterpreter()));
//    interpreters.insert(make_pair(FD, new FDInterpreter()));
//    interpreters.insert(make_pair(HRVD, new HRVDInterpreter()));
//    interpreters.insert(make_pair(TURBO, new TURBOInterpreter()));
//    interpreters.insert(make_pair(VD, new VDInterpreter()));
//    interpreters.insert(make_pair(RCFG, new RCFGInterpreter()));
//    interpreters.insert(make_pair(TCFG, new PDUInterpreter()));					// FIXME(ak): TCFG?
//    interpreters.insert(make_pair(ECFG, new ECFGInterpreter()));
//    interpreters.insert(make_pair(ETEMP, new ETEMPInterpreter()));
//    interpreters.insert(make_pair(PTO, new PTOInterpreter()));
//    interpreters.insert(make_pair(CCVS, new CCVSInterpreter()));
//    interpreters.insert(make_pair(LFE, new LFEInterpreter()));
//    interpreters.insert(make_pair(AMBC, new AMBCInterpreter()));
//    interpreters.insert(make_pair(IEC, new IECInterpreter()));
//    interpreters.insert(make_pair(VEP, new VEPInterpreter()));
//    interpreters.insert(make_pair(TF, new TFInterpreter()));
//    interpreters.insert(make_pair(RF, new RFInterpreter()));
//
//    return interpreters;
//}
//
//
///** This method initializes a map used to equate a specific PGN value with an
// * interpreter class that can convert and print the messages within the PDU-
// * formatted variable. */
//map<string, int> get_pgn_by_name() {
//    map<string, int> pgn_by_name;
//    pgn_by_name.insert(make_pair("PDU", PDU));
//    pgn_by_name.insert(make_pair("TSC1", TSC1));
//    pgn_by_name.insert(make_pair("EXAC", EXAC));
//    pgn_by_name.insert(make_pair("RQST", RQST));
//    pgn_by_name.insert(make_pair("ERC1", ERC1));
//    pgn_by_name.insert(make_pair("EBC1", EBC1));
//    pgn_by_name.insert(make_pair("EBC2", EBC2));
//    pgn_by_name.insert(make_pair("ETC1", ETC1));
//    pgn_by_name.insert(make_pair("ETC2", ETC2));
//    pgn_by_name.insert(make_pair("EEC1", EEC1));
//    pgn_by_name.insert(make_pair("EEC2", EEC2));
//    pgn_by_name.insert(make_pair("EEC3", EEC3));
//    pgn_by_name.insert(make_pair("GFI2", GFI2));
//    pgn_by_name.insert(make_pair("EI", EI));
//    pgn_by_name.insert(make_pair("FD", FD));
//    pgn_by_name.insert(make_pair("HRVD", HRVD));
//    pgn_by_name.insert(make_pair("TURBO", TURBO));
//    pgn_by_name.insert(make_pair("VD", VD));
//    pgn_by_name.insert(make_pair("RCFG", RCFG));
//    pgn_by_name.insert(make_pair("TCFG", TCFG));
//    pgn_by_name.insert(make_pair("ECFG", ECFG));
//    pgn_by_name.insert(make_pair("ETEMP", ETEMP));
//    pgn_by_name.insert(make_pair("PTO", PTO));
//    pgn_by_name.insert(make_pair("CCVS", CCVS));
//    pgn_by_name.insert(make_pair("LFE", LFE));
//    pgn_by_name.insert(make_pair("AMBC", AMBC));
//    pgn_by_name.insert(make_pair("IEC", IEC));
//    pgn_by_name.insert(make_pair("VEP", VEP));
//    pgn_by_name.insert(make_pair("TF", TF));
//    pgn_by_name.insert(make_pair("RF", RF));
//    pgn_by_name.insert(make_pair("EBC_ACC", EBC_ACC));
//
//    return pgn_by_name;
//}
//
//
///** The method initializes the list of all interpretable PGN values (used to
// * check if a certain PGN value is valid) */
//vector<int> get_PGNs() {
//    vector<int> PGNs {
//        PDU, TSC1, EXAC, RQST, ERC1, EBC1, ETC1, EEC2, EEC1, ETC2, GFI2, EI, FD,
//        EBC2, HRVD, TURBO, EEC3, VD, RCFG, TCFG, ECFG, ETEMP, PTO, CCVS, LFE,
//        AMBC, IEC, VEP, TF, RF, EBC_ACC,
//    };
//
//    return PGNs;
//}
//
//vector<string> get_message_names() {
//	vector<string> message_names {
//        "PDU", "TSC1", "EXAC", "RQST", "ERC1", "EBC1", "ETC1", "EEC2", "EEC1",
//		"ETC2", "GFI2", "EI", "FD", "EBC2", "HRVD", "TURBO", "EEC3", "VD",
//		"RCFG", "TCFG", "ECFG", "ETEMP", "PTO", "CCVS", "LFE", "AMBC", "IEC",
//		"VEP", "TF", "RF", "TPDT", "TPCM"
//	};
//
//	return message_names;
//}
//
////#include <cstdio>
////#include <iostream>
////#include <memory>
////#include <stdexcept>
////#include <string>
////#include <array>
////
////std::string exec(const char* cmd) {
////    std::array<char, 128> buffer;
////    std::string result;
////    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
////    if (!pipe) throw std::runtime_error("popen() failed!");
////    while (!feof(pipe.get())) {
////        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
////            result += buffer.data();
////    }
////    return result;
////}
//
//
//int main() {
//    // process arguments that are inputed during execution
//    FILE *fp = fopen("/usr/aboudy/j1939_eng_output.txt" , "w");
//    bool numeric = false;
//    int pgn_val;
//    string infile = "/usr/aboudy/j1939_eng.dbg";
//    int fd = open("/pps/truck", O_WRONLY);
//
//    // some predefined variables
//    void *message;
//
//    // collect initial variables
//    map <int, J1939Interpreter*> interpreters = get_interpreters();
//    map<string, int> pgn_by_name = get_pgn_by_name();
//    vector<int> PGNs = get_PGNs();
//    vector<string> names = get_message_names();
//
//    // start streaming the text file
//    ifstream in(infile);
//    char line[255];
//
//    // ignore first two (header) lines
//    for (int i=0; i<2; i++){
//        in.getline(line, 255);
//    }
//
//    while (in) {
//        // Read data from the text file
//        in.getline(line, 255);
//
//        // separate the string by its " " delimiter
//        vector<string> tokens;
//        char *pch = strtok(line, " ");
//        while (pch != NULL) {
//            tokens.push_back(string(pch));
//            pch = strtok(NULL, " ");
//        }
//
//        // check for empty line
//        if (tokens.size() == 0) continue;
//
//        // FIXME: check for end of text file
//        if (tokens[0] == "rdj1939") break;
//
//        // if prompted, print the collected message
//        if (true) {
//            for (int i=0; i<tokens.size(); ++i)
//                cout << tokens[i] << " ";
//            cout << endl;
//        }
//
//        // if the message was not in it's PDU format, convert it. Otherwise,
//        // determine the interpreter for printing and publishing from the first
//        // token term (i.e. the name of the message)
//        if (tokens[0] =="PDU") {
//        	j1939_pdu_typ *pdu = (j1939_pdu_typ*) interpreters[PDU]->import(tokens);
//
//            // compute the PGN value from the PDU format and specific terms
//            pgn_val = TWOBYTES(pdu->pdu_format, pdu->pdu_specific);
//
//            // skip if the interpreter is cannot be deciphered
//            if (find(PGNs.begin(), PGNs.end(), pgn_val) == PGNs.end()) {
//                continue;
//            }
//
//            // convert the message to its message-specific format
//            message = interpreters[pgn_val]->convert(pdu);
//        } else {
//            // skip if the interpreter is cannot be deciphered
//            if (find(names.begin(), names.end(), tokens[0]) == names.end()) {
//                continue;
//            }
//
//            // get the PGN value from the name of the message
//            pgn_val = pgn_by_name[tokens[0]];
//
//            // import the message
//            message = interpreters[pgn_val]->import(tokens);
//
//        }
//
//        // print the message in it's message-specific format
//        interpreters[pgn_val]->print(message, fp, numeric);
//
//        // publish the message onto the pub/sub server
//        interpreters[pgn_val]->publish(message, fd);
//
////        string res = exec("ls /usr/aboudy");
////        cout << res << endl;
//
////        FILE *f = fopen("/pps/truck/TSC1", "r");
////        int BUFFER_SIZE = 16;
////        char buffer[BUFFER_SIZE];
////        if (f != NULL)
////        {
////            while (fgets(buffer, BUFFER_SIZE, f) != NULL)
////                printf("%s", buffer);
////            pclose(f);
////        }
//    }
//
//    return 0;
//}
