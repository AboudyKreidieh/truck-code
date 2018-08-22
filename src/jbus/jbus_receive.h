/**\file
 *
 * jbus_receive.h
 *
 * This file contains all methods needed to collect data from the jbus.
 *
 * PDU data is collected and printed as single line strings of the format:
 * 	PDU HH:MM:SS.mmm priority format specific src_address num_bytes byte[0] ...
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date July 11, 2018
 */

#ifndef SRC_JBUS_JBUS_RECEIVE_H_
#define SRC_JBUS_JBUS_RECEIVE_H_


/** blank
 *
 * blank.
 */
class JBusReceiver{
public:
	bool receive_status = 0;	/* true if a new message is received */
	void *message;				/* most recent message received */

	/* Collects a new message and places it in the pdu or (blank) objects. If a
	 * new message is received, the receive status is set to the status of the
	/* received message. */
	virtual void collect();

	/* Publishes the most recently acquired object, assuming a new message has
	 * been transmitted */
	virtual void publish();
};


/** blank */
class PDUReceiver : JBusReceiver {
	virtual void collect();
	virtual void publish();
};


#endif /* SRC_JBUS_JBUS_RECEIVE_H_ */
