/**
 * j1939_base_interpreter.h
 *
 * This file contains the abstract class for J1939 message processing. The
 * abstract class consists of methods for:
 *  - processing: converting a pdu variable into its message-specific type
 *  - comparing: checking if a pdu message is the same type as the one covered
 *    by any specific subclass of the abstract class
 *  - printing: printing/logging the information in a message
 *  - publishing: sending messages to a pub/sub servers
 *
 * 	Created on: May 31, 2018
 * 		Author: Abdul Rahman Kreidieh
 */

#ifndef INCLUDE_JBUS_J1939_BASE_INTERPRETER_H_
#define INCLUDE_JBUS_J1939_BASE_INTERPRETER_H_

#include <string>
#include <math.h>
#include "j1939_struct.h"

using namespace std;

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

#endif /* INCLUDE_JBUS_J1939_BASE_INTERPRETER_H_ */
