/**
 * j1939_interpreters.h
 *
 * This file contains the abstract class for J1939 message processing. The
 * abstract class consists of methods for:
 *  - processing: converting a pdu variable into its message-specific type
 *  - comparing: checking if a pdu message is the same type as the one covered
 *    by any specific subclass of the abstract class
 *  - printing: printing/logging the information in a message
 *  - publishing: sending messages to a pub/sub servers
 *
 * This file contains also interpreters for all used J1939 messages.
 *
 *  Created on: May 31, 2018
 *      Author: Abdul Rahman Kreidieh
 */

#ifndef INCLUDE_JBUS_J1939_INTERPRETERS_H_
#define INCLUDE_JBUS_J1939_INTERPRETERS_H_

#include <stdio.h>
#include <math.h>
#include "j1939_struct.h"
#include "j1939_utils.h"


/** Base interpreter for processing pdu variables into their message-specific
 * format. */
class j1939_interpreter
{
public:
	// converts a message from its pdu format to its data-specific format
	virtual void* convert(j1939_pdu_typ*) = 0;

	// checks whether the incoming message is of the same pdu type as the
	// one covered by this (child) class
	bool is_type(j1939_pdu_typ*);

	// prints and logs the message-specific format of a data point to the
	// file path located in FILE*. Furthermore, if the third element is set
	// to true, the names of the separate data points are further revealed
	virtual void print(void*, FILE*, bool) = 0;

	// publish the message to the QNX publish/subscribe server
	virtual void publish(void*, int) = 0;
//	virtual void ~publish();

	// J1939 PGN number for the data-type
	int pgn;
};


/* -------------------------------------------------------------------------- */
/* ----------------------- In case not interpretable ------------------------ */
/* -------------------------------------------------------------------------- */


/** PDU generic interpreter (in case the PGN number is not interpretable) */
class PDU_interpreter : public j1939_interpreter
{
public:
	int pgn = 0;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
    virtual void publish(void*, int);
};


/* -------------------------------------------------------------------------- */
/* ------------------------ Received from the brake ------------------------- */
/* -------------------------------------------------------------------------- */


/** PDU TSC1 (Torque/Speed Control) doc. in J1939 - 71, p149 */
class TSC1_interpreter : public j1939_interpreter
{
public:
	int pgn = TSC1;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU EBC1 (Electronic Brake Controller #1) doc. in J1939 - 71, p151 */
class EBC1_interpreter : public j1939_interpreter
{
public:
	int pgn = EBC1;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU EBC2 (Electronic Brake Controller 2) doc. in J1939 - 71, p170 */
class EBC2_interpreter : public j1939_interpreter
{
public:
	int pgn = EBC2;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/* -------------------------------------------------------------------------- */
/* ------------------------ Received from the engine ------------------------ */
/* -------------------------------------------------------------------------- */


/** PDU EEC1 (Electronic Engine Controller #1) doc. in J1939 - 71, p152 */
class EEC1_interpreter : public j1939_interpreter
{
public:
	int pgn = EEC1;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU EEC2 (Electronic Engine Controller #2) doc. in J1939 - 71, p152 */
class EEC2_interpreter : public j1939_interpreter
{
public:
	int pgn = EEC2;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU EEC3 (Electronic Engine Controller #3) doc. in J1939 - 71, p154 */
class EEC3_interpreter : public j1939_interpreter
{
public:
	int pgn = EEC3;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU ETC1 (Elec. Transmission Controller #1) doc. in J1939 - 71, p151 */
class ETC1_interpreter : public j1939_interpreter
{
public:
	int pgn = ETC1;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU ETC2 (Electronic Transmission Controller #2) doc. in J1939 - 71, p152 */
class ETC2_interpreter : public j1939_interpreter
{
public:
	int pgn = ETC2;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU ERC1 (Electronic Retarder Controller #1) doc. in J1939 - 71, p150 */
class ERC1_interpreter : public j1939_interpreter
{
public:
	int pgn = ERC1;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU TF (Transmission Fluids) doc. in J1939 - 71, p164 */
class TF_interpreter : public j1939_interpreter
{
public:
	int pgn = TF;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU CCVS (Cruise Control/Vehicle Speed) doc. in J1939 - 71, p162 */
class CCVS_interpreter : public j1939_interpreter
{
public:
	int pgn = CCVS;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU LFE (Fuel Economy) doc. in J1939 - 71, p162 */
class LFE_interpreter : public j1939_interpreter
{
public:
	int pgn = LFE;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/* -------------------------------------------------------------------------- */
/* ------------------ Received from both engine and brake ------------------- */
/* -------------------------------------------------------------------------- */


/** PDU RF (Retarder Fluids) doc. in J1939 - 71, p164 */
class RF_interpreter : public j1939_interpreter
{
public:
	int pgn = RF;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/* -------------------------------------------------------------------------- */
/* ----------------------------- Miscellaneous ------------------------------ */
/* -------------------------------------------------------------------------- */


// FIXME(ak): is this not written anywhere?
/** PDU TC1 (Transmission Control) doc. in J1939 - 71, p149 */
class TC1_interpreter : public j1939_interpreter
{
public:
	// char* name = "Transmission Control (TC1)";
	int pgn = 0;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU TURBO (Turbocharger) doc. in J1939 - 71, p153 */
class TURBO_interpreter : public j1939_interpreter
{
public:
	int pgn = TURBO;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU VD (Vehicle Distance) doc. in J1939 - 71, p154 */
class VD_interpreter : public j1939_interpreter
{
public:
	int pgn = VD;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU RCFG (Retarder Configuration) doc. in J1939 - 71, p155 */
class RCFG_interpreter : public j1939_interpreter
{
public:
	int pgn = RCFG;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


// TODO(ak): do this
/** PDU TCFG (Transmission Configuration) doc. in J1939 - 71, p155 */
//class TCFG_interpreter : public j1939_interpreter
//{
//public:
//	// char* name = "Transmission Configuration (TCFG)";
//	int pgn = TCFG;
//	virtual void *convert(j1939_pdu_typ*);
//	virtual void print(void*, FILE*, bool);
//	virtual void publish(void*);
//};


/** PDU ECFG (Engine Configuration) doc. in J1939 - 71, p156 */
class ECFG_interpreter : public j1939_interpreter
{
public:
	int pgn = ECFG;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU ETEMP (Engine Temperature) doc. in J1939 - 71, p160 */
class ETEMP_interpreter : public j1939_interpreter
{
public:
	int pgn = ETEMP;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU PTO (Power Takeoff Information) doc. in J1939 - 71, p161 */
class PTO_interpreter : public j1939_interpreter
{
public:
	int pgn = PTO;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU AMBC (Ambient Conditions) doc. in J1939 - 71, p163 */
class AMBC_interpreter : public j1939_interpreter
{
public:
	int pgn = AMBC;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU IEC (Inlet/Exhaust Conditions) doc. in J1939 - 71, p164 */
class IEC_interpreter : public j1939_interpreter
{
public:
	int pgn = IEC;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU VEP (Vehicle Electrical Power) doc. in J1939 - 71, p164 */
class VEP_interpreter : public j1939_interpreter
{
public:
	int pgn = VEP;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU HRVD (High Resolution Vehicle Distance) doc. in J1939 - 71, p170 */
class HRVD_interpreter : public j1939_interpreter
{
public:
	int pgn = HRVD;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU FD (Fan Drive) doc. in J1939 - 71, sec. 5.3.58 */
class FD_interpreter : public j1939_interpreter
{
public:
	int pgn = FD;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


// TODO(ak): find out if we need these
/** PDU EXAC (External Acceleration Control), WABCO proprietary */
//class EXAC_interpreter : public j1939_interpreter
//{
//public:
//	int pgn = EXAC;
//	j1939_exac_typ *convert(j1939_pdu_typ*);
//	void print(j1939_exac_typ*, FILE*, bool);
//};


/** PDU EBC_ACC (Electronic Brake Control for ACC), WABCO proprietary */
//class EBC_ACC_interpreter : public j1939_interpreter
//{
//public:
//	int pgn = 0;
//	j1939_ebc_acc_typ *convert(j1939_pdu_typ*);
//	void print(j1939_ebc_acc_typ*, FILE*, bool);
//};


/** PDU GFI2 (Gaseous Fuel Information 2), J1939-71, sec 5.3.123 */
class GFI2_interpreter : public j1939_interpreter
{
public:
	int pgn = GFI2;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


/** PDU EI (Engine Information), J1939-71, sec 5.3.105 */
class EI_interpreter : public j1939_interpreter
{
public:
	int pgn = EI;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*, int);
};


#endif /* INCLUDE_JBUS_J1939_INTERPRETERS_H_ */
