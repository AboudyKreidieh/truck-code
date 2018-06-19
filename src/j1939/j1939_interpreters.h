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
	virtual void publish(void*) = 0;

	// J1939 PGN number for the data-type
	int pgn;
};


bool j1939_interpreter::is_type(j1939_pdu_typ *pdu) {
	int target_pgn = (int)pow(2.0,8.0) * pdu->pdu_format + pdu->pdu_specific;
    return (pdu -> pdu_specific == pgn);
}


/** PDU generic interpreter (in case the PGN number is not interpretable) */
class PDU_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = 0;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
    virtual void publish(void*);
};


/** PDU TSC1 (Torque/Speed Control) doc. in J1939 - 71, p149 */
class TSC1_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = TSC1;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


// FIXME(ak): is this not written anywhere?
/** PDU TC1 (Transmission Control) doc. in J1939 - 71, p149 */
class TC1_interpreter : public j1939_interpreter
{
public:
	// char* name = "Transmission Control (TC1)";
	int pdu_specific = 0;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU ERC1 (Electronic Retarder Controller #1) doc. in J1939 - 71, p150 */
class ERC1_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = ERC1;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU EBC1 (Electronic Brake Controller #1) doc. in J1939 - 71, p151 */
class EBC1_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = EBC1;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU ETC1 (Elec. Transmission Controller #1) doc. in J1939 - 71, p151 */
class ETC1_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = ETC1;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU EEC1 (Electronic Engine Controller #1) doc. in J1939 - 71, p152 */
class EEC1_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = EEC1;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU EEC2 (Electronic Engine Controller #2) doc. in J1939 - 71, p152 */
class EEC2_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = EEC2;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU ETC2 (Electronic Transmission Controller #2) doc. in J1939 - 71, p152 */
class ETC2_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = ETC2;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU TURBO (Turbocharger) doc. in J1939 - 71, p153 */
class TURBO_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = TURBO;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU EEC3 (Electronic Engine Controller #3) doc. in J1939 - 71, p154 */
class EEC3_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = EEC3;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU VD (Vehicle Distance) doc. in J1939 - 71, p154 */
class VD_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = VD;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU RCFG (Retarder Configuration) doc. in J1939 - 71, p155 */
class RCFG_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = RCFG;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


// TODO(ak): do this
/** PDU TCFG (Transmission Configuration) doc. in J1939 - 71, p155 */
class TCFG_interpreter : public j1939_interpreter
{
public:
	// char* name = "Transmission Configuration (TCFG)";
	int pdu_specific = TCFG;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU ECFG (Engine Configuration) doc. in J1939 - 71, p156 */
class ECFG_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = ECFG;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU ETEMP (Engine Temperature) doc. in J1939 - 71, p160 */
class ETEMP_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = ETEMP;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU PTO (Power Takeoff Information) doc. in J1939 - 71, p161 */
class PTO_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = PTO;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU CCVS (Cruise Control/Vehicle Speed) doc. in J1939 - 71, p162 */
class CCVS_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = CCVS;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU LFE (Fuel Economy) doc. in J1939 - 71, p162 */
class LFE_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = LFE;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU AMBC (Ambient Conditions) doc. in J1939 - 71, p163 */
class AMBC_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = AMBC;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU IEC (Inlet/Exhaust Conditions) doc. in J1939 - 71, p164 */
class IEC_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = IEC;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU VEP (Vehicle Electrical Power) doc. in J1939 - 71, p164 */
class VEP_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = VEP;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU TF (Transmission Fluids) doc. in J1939 - 71, p164 */
class TF_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = TF;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU RF (Retarder Fluids) doc. in J1939 - 71, p164 */
class RF_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = RF;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU HRVD (High Resolution Vehicle Distance) doc. in J1939 - 71, p170 */
class HRVD_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = HRVD;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU EBC2 (Electronic Brake Controller 2) doc. in J1939 - 71, p170 */
class EBC2_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = EBC2;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU FD (Fan Drive) doc. in J1939 - 71, sec. 5.3.58 */
class FD_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = FD;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


// TODO(ak): find out if we need these
/** PDU EXAC (External Acceleration Control), WABCO proprietary
class EXAC_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = EXAC;
	j1939_exac_typ *convert(j1939_pdu_typ*);
	void print(j1939_exac_typ*, FILE*, bool);
};


/** PDU EBC_ACC (Electronic Brake Control for ACC), WABCO proprietary
class EBC_ACC_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = 0;
	j1939_ebc_acc_typ *convert(j1939_pdu_typ*);
	void print(j1939_ebc_acc_typ*, FILE*, bool);
};
*/


/** PDU GFI2 (Gaseous Fuel Information 2), J1939-71, sec 5.3.123 */
class GFI2_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = GFI2;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};


/** PDU EI (Engine Information), J1939-71, sec 5.3.105 */
class EI_interpreter : public j1939_interpreter
{
public:
	int pdu_specific = EI;
	virtual void *convert(j1939_pdu_typ*);
	virtual void print(void*, FILE*, bool);
	virtual void publish(void*);
};

#endif /* INCLUDE_JBUS_J1939_INTERPRETERS_H_ */
