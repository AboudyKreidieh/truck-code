/**\file
 *
 * can.h
 *
 * This file contains the class needed to read and write messages to the Control
 * Area Network (CAN) card.
 *
 * Messages during the reading and writing methods are processed from or placed
 * in a can_message object, also located in this folder.
 *
 * Finally, this file also contains a method for obtaining a pointer to an
 * active CAN class, or creating a new one. This methods is used by the
 * JBusSender and JBusReceiver classes in order for all other processes to
 * share the same CAN class.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date July 19, 2018
 */

#ifndef SRC_JBUS_CAN_H_
#define SRC_JBUS_CAN_H_

#include <vector>
#include <stdint.h>


/** Object used by the CAN class to process messages that it is supposed to
 * send or receive. */
typedef struct
{
	uint8_t id;
	uint8_t id_type;
	uint8_t length;
	vector <uint8_t> data;
} can_message;


/** Primary class used to communicate with the CAN card.
 *
 * This class is responsible for initializing communication with the CAN card,
 * as well as send and receive messages from the CAN card.
 *
 * A single CAN class is used to send and receive all messages by the individual
 * JBusSender and JBusReceiver classes
 */
class CAN {
	bool open;	/* true is communication with the CAN card is on. */
	// TODO(ak): add other things that may be needed.

	/** Initializes communication with the can card. */
	void init(int);

	/** Sends a message to the jbus via the can card. Messages are provided by
	 * the JBusSender classes. */
	void write(can_message*);

	/** Returns a message available from the jbus. Messages are sent to
	 * JBusReceiver classes. */
	can_message *read();

	/** Closes the connection with the CAN card. If the connection has already
	 * been closed, this operation is skipped. */
	void close();
};


/** Initializes communication with the CAN card and returns a pointer to the
 * CAN class.
 *
 * If communication has already been initialized by another class or
 * process, this message returns the pointer to the existing CAN class.
 */
CAN *can_init_or_get(int port) {
	// check if the port is in available
	if (port == 0) {															// TODO(ak): figure out how to do this
		// if the port is not available, return the
		CAN *can = 0;															// TODO(ak): figure out how to do this
	} else {
		// if no CAN class exists, create one and initialize communication with
		// the CAN card.
		CAN *can = new CAN();
		can->init(port);
	}
	return can;
}


#endif /* SRC_JBUS_CAN_H_ */
