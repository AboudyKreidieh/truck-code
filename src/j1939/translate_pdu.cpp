/** 
 * translate_pdu.cpp
 * 
 * This script translates several pdu-formatted j1939 messages into their 
 * message-specific formats, and once it is done, logs the data into an new text
 * file.
 */

#include <jbus/j1939_utils.h>
#include <jbus/j1939_struct.h>
#include <jbus/j1939_interpreters.h>
#include <vector>
#include <map>

using namespace std;


/** This method initializes a few variables that will help facilitate the 
 * message translation process. */
void initialize() {
    // blank
    map<int, j1939_interpreter*> interpreters;
    interpreters.insert(make_pair(PDU, new TSC1_interpreter()));  // FIXME(ak): add PDU interpreter
    interpreters.insert(make_pair(TSC1, new TSC1_interpreter()));
    interpreters.insert(make_pair(EXAC, new TSC1_interpreter()));  // FIXME(ak): EXAC?
    interpreters.insert(make_pair(RQST, new TSC1_interpreter()));  // FIXME(ak): RQST?
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
    interpreters.insert(make_pair(TCFG, new TCFG_interpreter()));
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
    interpreters.insert(make_pair(EBC_ACC, new RF_interpreter()));  // FIXME(ak): EBC_ACC?
    interpreters.insert(make_pair(TPDT, new RF_interpreter()));  // FIXME(ak): TPDT?
    interpreters.insert(make_pair(TPCM, new RF_interpreter()));  // FIXME(ak): TPDT?

    // list of all interpretable PGN values (used to check if a certain PGN 
    // value is valid)
    vector<int> PGNs {
        PDU, TSC1, EXAC, RQST, ERC1, EBC1, ETC1, EEC2, EEC1, ETC2, GFI2, EI, FD,
        EBC2, HRVD, TURBO, EEC3, VD, RCFG, TCFG, ECFG, ETEMP, PTO, CCVS, LFE,
        AMBC, IEC, VEP, TF, RF, EBC_ACC, TPDT, TPCM,
    };
}


int main() {
    return 0;
}
