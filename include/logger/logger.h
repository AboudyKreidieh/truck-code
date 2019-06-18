/**\file
 *
 * logger.h
 *
 * This script contains the DBManager object that is responsible for storing
 * state information from the truck onto an SQL server. This is done via QNX's
 * QDB interface.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date May 12, 2019
 */

#ifndef INCLUDE_LOGGER_LOGGER_H_
#define INCLUDE_LOGGER_LOGGER_H_

#include <string>
#include <vector>
#include <map>
#include <qdb/qdb.h>

using namespace std;


/** Database Manager class.
 *
 * This class is responsible for storing state information from the truck onto
 * an SQL server. This is done via QNX's QDB interface.
 */
class DBManager
{
public:
	/** Initialize the logger object.
	 *
	 * This method is responsible for initializing the QNX database server and
	 * create all tables that will be used to store state and communication
	 * information. This is used primarily as a data storage device, and
	 * otherwise is not interacted with during realtime operations.
	 *
	 * @param db_path
	 * 		path to the file that should contain all SQL tables that will be
	 * 		used by this object
	 * @param types
	 * 		a vector of the types of elements to be stored in the database
	 * 		object. For J1939 messages, this is the parameter group numbers
	 * 		(PGNs) of the individual elements.
	 * @param append
	 * 		specifies whether the new elements should be appended to any
	 * 		existing database. If set to false, any elements in existing
	 * 		databases will be discarded.
	 */
	void init(std::string db_path, std::vector<int> types, bool append);

	/** Store an element in its respective database object.
	 *
	 * @param type
	 * 		the type of element. For J1939 messages, this is their parameter
	 * 		group numbers (PGNs).
	 * @param data
	 * 		the data element
	 */
	void store(int type, void *data);

	/** Return the types of variables that can be stored by this object.
	 *
	 * This corresponds to the methods specified upon initialization.
	 */
	vector<int> get_types();

private:
	vector <int> _types;	/**< a vector of the types of elements to be stored */
							/**< in the database object. For J1939 messages, */
							/**< this is the parameter group numbers (PGNs) of */
							/**< the individual elements. */
	string _fp;				/**< path to the directory that should contain all */
							/**< SQL files that will be used by this object */
	qdb_hdl_t* _hdl;  		/**< The QDB database handle */

	/** TODO */
	int _init_table(qdb_hdl_t* hdl, string name, string schema);

	/** TODO */
	int _maybe_clear(qdb_hdl_t* hdl, string name, bool append);

	int _init_pdu(qdb_hdl_t* hdl, bool append);
	int _store_pdu(void *data);
	int _init_tsc1(qdb_hdl_t* hdl, bool append);
	int _store_tsc1(void *data);
	int _init_erc1(qdb_hdl_t* hdl, bool append);
	int _store_erc1(void *data);
	int _init_ebc1(qdb_hdl_t* hdl, bool append);
	int _store_ebc1(void *data);
	int _init_ebc2(qdb_hdl_t* hdl, bool append);
	int _store_ebc2(void *data);
	int _init_etc1(qdb_hdl_t* hdl, bool append);
	int _store_etc1(void *data);
	int _init_etc2(qdb_hdl_t* hdl, bool append);
	int _store_etc2(void *data);
	int _init_eec1(qdb_hdl_t* hdl, bool append);
	int _store_eec1(void *data);
	int _init_eec2(qdb_hdl_t* hdl, bool append);
	int _store_eec2(void *data);
	int _init_eec3(qdb_hdl_t* hdl, bool append);
	int _store_eec3(void *data);
	int _init_gfi2(qdb_hdl_t* hdl, bool append);
	int _store_gfi2(void *data);
	int _init_ei(qdb_hdl_t* hdl, bool append);
	int _store_ei(void *data);
	int _init_fd(qdb_hdl_t* hdl, bool append);
	int _store_fd(void *data);
	int _init_hrvd(qdb_hdl_t* hdl, bool append);
	int _store_hrvd(void *data);
	int _init_turbo(qdb_hdl_t* hdl, bool append);
	int _store_turbo(void *data);
	int _init_vd(qdb_hdl_t* hdl, bool append);
	int _store_vd(void *data);
	int _init_rcfg(qdb_hdl_t* hdl, bool append);
	int _store_rcfg(void *data);
	int _init_tcfg(qdb_hdl_t* hdl, bool append);
	int _store_tcfg(void *data);
	int _init_ecfg(qdb_hdl_t* hdl, bool append);
	int _store_ecfg(void *data);
	int _init_etemp(qdb_hdl_t* hdl, bool append);
	int _store_etemp(void *data);
	int _init_pto(qdb_hdl_t* hdl, bool append);
	int _store_pto(void *data);
	int _init_ccvs(qdb_hdl_t* hdl, bool append);
	int _store_ccvs(void *data);
	int _init_lfe(qdb_hdl_t* hdl, bool append);
	int _store_lfe(void *data);
	int _init_ambc(qdb_hdl_t* hdl, bool append);
	int _store_ambc(void *data);
	int _init_iec(qdb_hdl_t* hdl, bool append);
	int _store_iec(void *data);
	int _init_vep(qdb_hdl_t* hdl, bool append);
	int _store_vep(void *data);
	int _init_tf(qdb_hdl_t* hdl, bool append);
	int _store_tf(void *data);
	int _init_rf(qdb_hdl_t* hdl, bool append);
	int _store_rf(void *data);
};


#endif /* INCLUDE_LOGGER_LOGGER_H_ */
