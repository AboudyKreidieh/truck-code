/**\file
 *
 * j1939_interpreters.h
 *
 * This file contains the abstract class for J1939 message processing. The
 * abstract class consists of methods for:
 *  - processing: converting a pdu variable into its message-specific type
 *  - comparing: checking if a pdu message is the same type as the one covered
 *    by any specific subclass of the abstract class
 *  - printing: printing/logging the information in a message
 *  - importing: collecting data from a file of numeric formatted data and
 *    placing it in a message-specific object
 *
 * This file contains also interpreters for all used J1939 messages, and a
 * method for importing a hash table containing all these interpreters, where
 * the PGN value of the interpreter is used to access it.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date May 31, 2018
 */

#ifndef INCLUDE_JBUS_J1939_INTERPRETERS_H_
#define INCLUDE_JBUS_J1939_INTERPRETERS_H_

#include <stdio.h>
#include <math.h>
#include <vector>
#include <map>
#include <string>
#include "j1939_struct.h"
#include "j1939_utils.h"

using namespace std;


/** Base interpreter for processing pdu variables into their message-specific
 * format. */
class J1939Interpreter
{
public:
	/** J1939 PGN number for the data-type */
	virtual int pgn();

	/** Convert a message from its pdu format to its data-specific format.
	 *
	 * @param pdu generic format of the message
	 * @return data-specific format of the message, depending on the child class
	 * used
	 */
	virtual void *convert(j1939_pdu_typ *pdu) = 0;

	/** Check whether the incoming message is of the same pdu type as the one
	 * covered by this (child) class.
	 *
	 * @param pdu generic format of the message
	 * @return True if the parameter group number (PGN) of the message matches
	 * the PGN of the child class; False otherwise
	 */
	bool is_type(j1939_pdu_typ* pdu);

	/** Print and log the message-specific format of a data point.
	 *
	 * @param pdv the message to print
	 * @param fp location of the file to print to
	 * @param numeric If set to true, the names of the separate data points are
	 * further revealed.
	 */
	virtual void print(void *pdv, FILE *fp, bool numeric) = 0;

	/** Import data from a printed file into a message-specific object.
	 *
	 * This is used primarily for processing preprinted messages from a file.
	 *
	 * @param tokens list of string values from each data element in the message
	 * @return the data-specific format of the message
	 */
	virtual void *import(vector<string> &tokens) = 0;

	/** destructor */
	virtual ~J1939Interpreter() = 0;
};


/* -------------------------------------------------------------------------- */
/* ----------------------- In case not interpretable ------------------------ */
/* -------------------------------------------------------------------------- */


/** PDU generic interpreter (in case the PGN number is not interpretable) */
class PDUInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/* -------------------------------------------------------------------------- */
/* ------------------------ Received from the brake ------------------------- */
/* -------------------------------------------------------------------------- */


/** PDU TSC1 (Torque/Speed Control) doc. in J1939 - 71, p149 */
class TSC1Interpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU EBC1 (Electronic Brake Controller #1) doc. in J1939 - 71, p151 */
class EBC1Interpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU EBC2 (Electronic Brake Controller #2) doc. in J1939 - 71, p170 */
class EBC2Interpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/* -------------------------------------------------------------------------- */
/* ------------------------ Received from the engine ------------------------ */
/* -------------------------------------------------------------------------- */


/** PDU EEC1 (Electronic Engine Controller #1) doc. in J1939 - 71, p152 */
class EEC1Interpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU EEC2 (Electronic Engine Controller #2) doc. in J1939 - 71, p152 */
class EEC2Interpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU EEC3 (Electronic Engine Controller #3) doc. in J1939 - 71, p154 */
class EEC3Interpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU ETC1 (Electronic Transmission Controller #1) doc. in J1939 - 71, p151 */
class ETC1Interpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU ETC2 (Electronic Transmission Controller #2) doc. in J1939 - 71, p152 */
class ETC2Interpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU ERC1 (Electronic Retarder Controller #1) doc. in J1939 - 71, p150 */
class ERC1Interpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU TF (Transmission Fluids) doc. in J1939 - 71, p164 */
class TFInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU CCVS (Cruise Control/Vehicle Speed) doc. in J1939 - 71, p162 */
class CCVSInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU LFE (Fuel Economy) doc. in J1939 - 71, p162 */
class LFEInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/* -------------------------------------------------------------------------- */
/* ------------------ Received from both engine and brake ------------------- */
/* -------------------------------------------------------------------------- */


/** PDU RF (Retarder Fluids) doc. in J1939 - 71, p164 */
class RFInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/* -------------------------------------------------------------------------- */
/* ----------------------------- Miscellaneous ------------------------------ */
/* -------------------------------------------------------------------------- */


// FIXME(ak): is this not written anywhere?
/** PDU TC1 (Transmission Control) doc. in J1939 - 71, p149 */
class TC1Interpreter : public J1939Interpreter
{
public:
	// char* name = "Transmission Control (TC1)";
	int pgn = 0;
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU TURBO (Turbocharger) doc. in J1939 - 71, p153 */
class TURBOInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU VD (Vehicle Distance) doc. in J1939 - 71, p154 */
class VDInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU RCFG (Retarder Configuration) doc. in J1939 - 71, p155 */
class RCFGInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


// TODO(ak): do this
/** PDU TCFG (Transmission Configuration) doc. in J1939 - 71, p155 */
//class TCFG_interpreter : public J1939Interpreter
//{
//public:
//	// char* name = "Transmission Configuration (TCFG)";
//	int pgn = TCFG;
//	virtual void *convert(j1939_pdu_typ *pdu);
//	virtual void print(void *pdv, FILE *fp, bool numeric);
//	virtual void publish(void*);
//};


/** PDU ECFG (Engine Configuration) doc. in J1939 - 71, p156 */
class ECFGInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU ETEMP (Engine Temperature) doc. in J1939 - 71, p160 */
class ETEMPInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU PTO (Power Takeoff Information) doc. in J1939 - 71, p161 */
class PTOInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU AMBC (Ambient Conditions) doc. in J1939 - 71, p163 */
class AMBCInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU IEC (Inlet/Exhaust Conditions) doc. in J1939 - 71, p164 */
class IECInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU VEP (Vehicle Electrical Power) doc. in J1939 - 71, p164 */
class VEPInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU HRVD (High Resolution Vehicle Distance) doc. in J1939 - 71, p170 */
class HRVDInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU FD (Fan Drive) doc. in J1939 - 71, sec. 5.3.58 */
class FDInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU GFI2 (Gaseous Fuel Information 2), J1939-71, sec 5.3.123 */
class GFI2Interpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/** PDU EI (Engine Information), J1939-71, sec 5.3.105 */
class EIInterpreter : public J1939Interpreter
{
public:
	virtual int pgn();
	virtual void *convert(j1939_pdu_typ *pdu);
	virtual void print(void *pdv, FILE *fp, bool numeric);
    virtual void *import(vector<string> &tokens);
};


/* This method initializes a map used to equate a specific PGN value with an
 * interpreter class that can convert and print the messages within the PDU-
 * formatted variable. */
extern map<int, J1939Interpreter*> get_interpreters();


#endif /* INCLUDE_JBUS_J1939_INTERPRETERS_H_ */
