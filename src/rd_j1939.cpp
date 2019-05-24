/**\file
 *
 * rd_j1939.cpp
 *
 * This script is responsible for periodically searching for messages that have
 * been sent by the j1939 bus. This process receives a message from the J1939
 * port (which may be an ordinary serial port with the B&B converter or a port
 * to the SSV CAN Board). If any new messages are received, they are published
 * to the pub/sub server for use by other processes.
 *
 * Arguments:
 * 	-f	filename for input
 * 	-t 	puts bytes from every frame received on stdout
 * 	-g 	specifies whether to use "generic" mode. In "generic" mode, write all
 * 		PDUs to database as byte streams, don't translate into specific PDU
 * 		formats or assemble multipacket messages
 * 	-v 	run in verbose mode, messages are printed in stdout
 * 	-n	specifies whether to print messages in numeric or non-numeric mode. Only
 * 		used if the process is running in debug mode.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date January 14, 2019
 */

#include "jbus/jbus.h"
#include "jbus/j1939_utils.h"
#include "jbus/j1939_struct.h"
#include "jbus/j1939_interpreters.h"
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

using namespace std;


int main(int argc, char **argv) {
	JBus jfunc;				/* object responsible to r/w messages */
	int external = 0;		/* external from jbus, internal converter */
	int slot_or_type;		/* external slot, internal type */
	int trace = 0;			/* whether to print the input (raw) message */
	bool generic = false;	/* whether to run in "generic" mode */
	long rcv_errors = 0;	/* number of errors during the receiving process */
	long num_received = 0;	/* number of messages successfully received */
	int j1939_debug = 0;	/* whether to print during the receive process */
	j1939_pdu_typ *pdu = new j1939_pdu_typ();	/* placeholder for messages */
	char *fname = "/dev/ser1";					/* path to serial port */

    /* Collect initial variables for interpreting J1939 messages. */
    map <int, J1939Interpreter*> interpreters = get_interpreters();
    bool numeric = false;
    void *message;

	int ch;
	while ((ch = getopt(argc, argv, "a:cd:f:s:tvg")) != EOF) {
		switch (ch) {
			case 'f': fname = strdup(optarg); break;
			case 't': trace = 1; break;
			case 'g': generic = true; break;  /* save as generic to database */
			case 'v': j1939_debug = 1; break;
			case 'n': numeric = true; break;
			default	: {
				printf("Usage: %s [-a <AVCS timing output>", argv[0]);
				printf("\t -c (CAN card vs serial STB) -d (debug)\n");
				printf("\t -t (trace) -f <CAN port>\n");
				printf("\t -s <db num to save> \n");
				printf("\t-g (generic save to DB)]\n");
				break;
			}
		}
	}

	/* Initialize the device port. */
    printf("Initializing device port: %s\n", fname);
    int fpin = jfunc.init(fname, O_RDONLY, NULL);

    if (fpin == -1) {
		printf("Error opening CAN device %s for input\n", fname);
		exit(EXIT_FAILURE);
	}

	int pgn;
	int rcv_val;
	while (true) {
		/* Receive a new value from the J-bus. */
		rcv_val = jfunc.receive(fpin, pdu, &external, &slot_or_type);

		/* If the retrieved message is an error message, increment the number of
		 * errors received and try again. */
		if (rcv_val == J1939_RECEIVE_MESSAGE_ERROR) {
			rcv_errors++;
			continue;
		}

		/* If a fatal error occurred when trying to receive a message, stop the
		 * message collecting process. */
		if (rcv_val == J1939_RECEIVE_FATAL_ERROR)
			break;

		/* Increment if received valid message. */
		num_received++;

        /* Update the time stamp of the PDU object. */
		get_current_timestamp(&pdu->timestamp);

        if (trace) {
			if (external) {
				interpreters[PDU]->print(pdu, stdout, false);
			} else
				printf("internal 0x%2x\n", slot_or_type);
			fflush(stdout);
		}

        if (!external) {
			printf("Received internal message\n");
			continue;
		}

		/* In "generic" mode, write all PDUs to publish/subscribe database as a
		 * byte streams, don't translate into specific PDU formats. */
		if (generic) {
			pgn = PDU;
			message = (void*)pdu;
		} else {
			/* Compute the PGN value from the PDU format and specific terms. */
	        pgn = (unsigned int) TWOBYTES(pdu->pdu_format, pdu->pdu_specific);

            /* Convert the message to its message-specific format. */
            message = interpreters[pgn]->convert(pdu);
		}

        /* Print the message in it's message-specific format. */
		if (j1939_debug)
			interpreters[pgn]->print(message, stdout, numeric);

		/* TODO: Publish the message to the pub/sub server. */
	}

	/* Close the connection. */
	jfunc.close_conn(&fpin);
	delete pdu;
}
