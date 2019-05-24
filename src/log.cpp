/**\file
 *
 * log.cpp
 *
 * TODO: fill in
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


int main() {
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
	string db_path = "/dev/qdb/" + str;

	/* Initialize some variables. */
	vector < tuple<int, void*> > results;
	int message_type;
	unsigned int i;
	void *message;

	/* Initialize the logger object. */
	DBManager *db_manager = new DBManager();
	db_manager->init(db_path, subscribed, db_append);

	/* Initialize the PubSub object, and subscribe to all relevant messages. */
	PubSub *ps = new PubSub();
	for (i=0; i < subscribed.size(); i++)
		ps->subscribe(subscribed[i]);

	while (true) {
		/* Collect all new subscription results from the PubSub server. */
		results = ps->get_subscription_results();

		/* If no new messages were collected, skip the logging procedure. */
		if (get<0>(results[0]) == PS_EMTPY)
			continue;

		/* Loop through all messages that were subscribed, and store them. */
		for (i=0; i<results.size(); i++) {
			message_type = get<0>(results[i]);
			message = get<1>(results[i]);
			db_manager->store(message, message_type);
		}
	}

	/* Free up memory. */
	delete db_manager;
	delete ps;

	return 0;
}
