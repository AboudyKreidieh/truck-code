/**\file
 *
 * log.cpp
 *
 * This process is responsible for storing all messages in a QDB (SQL-style)
 * server for later analysis.
 *
 * Arguments:
 *
 * - --logdir: TODO
 * - --append: TODO
 *
 * Usage:
 *
 * 	log --logdir "/dev/qdb" --append
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date May 22, 2019
 */
#include "jbus/j1939_utils.h"
#include "logger/logger.h"
#include "utils/pub_sub.h"
#include <tuple>
#include <vector>
#include <string>
#include <ctime>
#include <iostream>


static void show_usage(std::string name) {
    cout << "Usage: " << name << "\n"
    	 << "Options:\n"
         << "\t--logdir OUTFILE\tPath to the log file.\n"
         << "\t--append\t\tSpecify whether to append to an existing log directory.\n"
         << endl;
}


int main(int argc, char* argv[]) {
	/* Get the current date-time as a string. */
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];
	time (&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%d-%m-%Y-%H-%M-%S", timeinfo);
	string str(buffer);

	/* Messages that should be subscribed from the PubSub server for the logging
	 * procedure.
	 *
	 * Note: Modify this element to match the elements you would like to store
	 * in the SQL server.
	 */
	vector <int> subscribed = {
		PDU, TSC1, ERC1, EBC1, EBC2, ETC1, ETC2, EEC1, EEC2, EEC3, GFI2, EI, FD,
		HRVD, TURBO, VD, RCFG, TCFG, ECFG, ETEMP, PTO, CCVS, LFE, AMBC, IEC,
		VEP, TF, RF
	};

	/* Specifies whether to clear the QDB server once this process is
	 * initialized.
	 *
	 * By default, the server is cleared. However, the name of the server (see
	 * the next element) is always modified to match the current date, so the
	 * databases by default are already empty.
	 */
	bool db_append = false;

	/* Path to the file that should contain all SQL tables that will be used by
	 * this object.
	 *
	 * Note: Modify this element if you would like to store the messages in a
	 * single (unified) file.
	 */
	string db_path = "/dev/qdb_" + str;


	/* Initialize some variables. */
	map <int, void*> results;
	int message_type;
	void *message;

	/* Process arguments that are inputed during execution. */
    for (int i=1; i<argc; ++i) {
        string arg = argv[i];
        if ((arg == "-h") || (arg == "--help")) {
            show_usage(argv[0]);
            return 0;
        } else if (arg == "--logdir") {
        	db_path = argv[++i];
        } else if (arg == "--append") {
        	db_append = true;
        } else {
			cerr << arg << " argument is not known." << endl;
			return 1;
        }
    }

	/* Initialize the logger object. */
	DBManager *db_manager = new DBManager();
	db_manager->init(db_path, subscribed, db_append);

	/* Initialize the PubSub object, and subscribe to all relevant messages. */
	PubSub *ps = new PubSub();
	for (unsigned int i=0; i < subscribed.size(); i++)
		ps->subscribe(subscribed[i]);

	while (true) {
		/* Collect all new subscription results from the PubSub server. */
		results = ps->get_subscription_results();

		/* Loop through all messages that were subscribed, and store them. */
		for (unsigned int i=0; i<subscribed.size(); i++) {
			message_type = subscribed[i];
			message = results[message_type];
			db_manager->store(message_type, message);
		}
	}

	/* Free up memory. */
	delete db_manager;
	delete ps;

	return 0;
}
