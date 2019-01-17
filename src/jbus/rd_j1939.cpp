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
 * Options:
 * 	-d 	prints debug messages
 * 	-f 	filename for input
 * 	-t 	puts bytes from every frame received on stdout
 * 	-g 	specifies whether to use "generic" mode. In "generic" mode, write all
 * 		PDUs to database as byte streams, don't translate into specific PDU
 * 		formats or assemble multipacket messages
 * 	-s 	**blank**
 * 	-v 	run in verbose mode, messages are printed in stdout
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date January 14, 2019
 */

#include "jbus.h"
#include "j1939_utils.h"
#include "j1939_struct.h"
#include "j1939_interpreters.h"
#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

using namespace std;


/* This method initializes a map used to equate a specific PGN value with an
 * interpreter class that can convert and print the messages within the PDU-
 * formatted variable. */
map<int, J1939Interpreter*> get_interpreters() {
    map<int, J1939Interpreter*> interpreters;
    interpreters.insert(make_pair(PDU, new PDUInterpreter()));
    interpreters.insert(make_pair(TSC1, new TSC1Interpreter()));
    interpreters.insert(make_pair(EXAC, new PDUInterpreter()));  				// FIXME(ak): EXAC? -- replace with XBR
    interpreters.insert(make_pair(RQST, new PDUInterpreter()));  				// FIXME(ak): RQST?
    interpreters.insert(make_pair(ERC1, new ERC1Interpreter()));
    interpreters.insert(make_pair(EBC1, new EBC1Interpreter()));
    interpreters.insert(make_pair(EBC2, new EBC2Interpreter()));
    interpreters.insert(make_pair(ETC1, new ETC1Interpreter()));
    interpreters.insert(make_pair(ETC2, new ETC2Interpreter()));
    interpreters.insert(make_pair(EEC1, new EEC1Interpreter()));
    interpreters.insert(make_pair(EEC2, new EEC2Interpreter()));
    interpreters.insert(make_pair(EEC3, new EEC3Interpreter()));
    interpreters.insert(make_pair(GFI2, new GFI2Interpreter()));
    interpreters.insert(make_pair(EI, new EIInterpreter()));
    interpreters.insert(make_pair(FD, new FDInterpreter()));
    interpreters.insert(make_pair(HRVD, new HRVDInterpreter()));
    interpreters.insert(make_pair(TURBO, new TURBOInterpreter()));
    interpreters.insert(make_pair(VD, new VDInterpreter()));
    interpreters.insert(make_pair(RCFG, new RCFGInterpreter()));
    interpreters.insert(make_pair(TCFG, new PDUInterpreter()));					// FIXME(ak): TCFG?
    interpreters.insert(make_pair(ECFG, new ECFGInterpreter()));
    interpreters.insert(make_pair(ETEMP, new ETEMPInterpreter()));
    interpreters.insert(make_pair(PTO, new PTOInterpreter()));
    interpreters.insert(make_pair(CCVS, new CCVSInterpreter()));
    interpreters.insert(make_pair(LFE, new LFEInterpreter()));
    interpreters.insert(make_pair(AMBC, new AMBCInterpreter()));
    interpreters.insert(make_pair(IEC, new IECInterpreter()));
    interpreters.insert(make_pair(VEP, new VEPInterpreter()));
    interpreters.insert(make_pair(TF, new TFInterpreter()));
    interpreters.insert(make_pair(RF, new RFInterpreter()));

    return interpreters;
}


int main(int argc, char **argv) {
	JBus jfunc;				/* */
	int external = 0;		/* external from jbus, internal converter */
	int slot_or_type;		/* external slot, internal type */
	int trace = 0;			/* */
	bool generic = false;	/* whether to run in "generic" mode */
	long rcv_errors = 0;	/* number of errors during the receiving process */
	long num_received = 0;	/* number of messages successfully received */
	int j1939_debug = 0;	/* whether to print during the receive process */
	int use_can = 0;		/* */
	j1939_pdu_typ *pdu = new j1939_pdu_typ();	/* */
	string fname = J1939STB_SERIAL_DEVICE_NAME;	/* */

	/* True if only storing a single Parameter Group in database. */
	int single_pgn = 0;

    /* collect initial variables for interpreting J1939 messages. */
    map <int, J1939Interpreter*> interpreters = get_interpreters();
    bool numeric = false;
    void *message;

	int ch;
	while ((ch = getopt(argc, argv, "a:cd:f:s:tvg")) != EOF) {
		switch (ch) {
			case 'f': fname = strdup(optarg); break;
			case 't': trace = 1; break;
			case 'g': generic = true; break;  /* save as generic to database */
			case 's': single_pgn = 1; break;
			case 'v': j1939_debug = 1; break;
			case 'n': numeric = true; break;
			default: {
				printf("Usage: %s [-a <AVCS timing output>", argv[0]);
				printf("\t -c (CAN card vs serial STB) -d (debug)\n");
				printf("\t -t (trace) -f <CAN port>\n");
				printf("\t -s <db num to save> \n");
				printf("\t-g (generic save to DB)]\n");
				break;
			}
		}
	}

	/* Initialize device port. */
	int fpin = jfunc.init(fname, O_RDONLY, NULL);
	if (fpin == -1) {
		printf("Error opening %s device %s for input\n",
				use_can?"CAN":"STB", fname);
		exit(EXIT_FAILURE);
	}

	int pgn;
	int rcv_val;
	while (true) {
		rcv_val = jfunc.receive(fpin, pdu, &external, &slot_or_type);

		/* If **blank**, increment the number of errors received and try
		 * again. */
		if (rcv_val == J1939_RECEIVE_MESSAGE_ERROR) {
			rcv_errors++;
			continue;
		}

		/* If **blank**, stop the message receiving process. */
		if (rcv_val == J1939_RECEIVE_FATAL_ERROR)
			break;

		/* received valid message */
		num_received++;

        /* update the time stamp of the PDU object */
		get_current_timestamp(&pdu->timestamp);

        if (trace) {
			if (external) {
				interpreters[PDU]->print(pdu, stdout, false);
			} else
				printf("internal 0x%2x\n", slot_or_type);
			fflush(stdout);
		}

        if (!use_can && !external) {
			printf("Received internal message\n");
			continue;
		}

		/* In "generic" mode, write all PDUs to publish/subscribe database as a
		 * byte streams, don't translate into specific PDU formats. */
		if (generic) {
			pgn = PDU;
			message = (void*)pdu;
		} else {
			/* compute the PGN value from the PDU format and specific terms */
	        pgn = (unsigned int) TWOBYTES(pdu->pdu_format, pdu->pdu_specific);

            /* convert the message to its message-specific format */
            message = interpreters[pgn]->convert(pdu);
		}

        /* print the message in it's message-specific format */
		if (j1939_debug)
			interpreters[pgn]->print(message, stdout, numeric);

		/* TODO: publish the message */
	}

	/* close the connection */
	jfunc.close_conn(&fpin);
}
