/**\file
 *
 * logger.cpp
 *
 * Implements methods in logger.h.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date May 12, 2019
 */

#include <string>
#include <vector>
#include <errno.h>
#include <qdb/qdb.h>
#include "logger.h"
#include "jbus/j1939_utils.h"
#include "jbus/j1939_struct.h"

using namespace std;


vector<int> DBManager::get_types() {
	return this->_types;
}


void DBManager::init(string db_path, vector<int> types, bool append) {
	this->_types = types;
	this->_fp = db_path;

	/* Connect to the QDB database. */
	this->_hdl = qdb_connect(db_path.c_str(), 0);
	if (this->_hdl == NULL)
	  fprintf(stderr, "Error connecting to database: %s\n", strerror(errno));

	/* Initialize the tables for all requested types to be stored. */
	for (unsigned int i=0; i<types.size(); i++) {
		switch (types[i]) {
		    case PDU   : this->_init_pdu(this->_hdl, append);
		                 break;
		    case TSC1  : this->_init_tsc1(this->_hdl, append);
		    			 break;
		    case ERC1  : this->_init_erc1(this->_hdl, append);
		    			 break;
		    case EBC1  : this->_init_ebc1(this->_hdl, append);
		    			 break;
		    case EBC2  : this->_init_ebc2(this->_hdl, append);
		    			 break;
		    case ETC1  : this->_init_etc1(this->_hdl, append);
		    			 break;
		    case ETC2  : this->_init_etc2(this->_hdl, append);
		    			 break;
		    case EEC1  : this->_init_eec1(this->_hdl, append);
		    			 break;
		    case EEC2  : this->_init_eec2(this->_hdl, append);
		    			 break;
		    case EEC3  : this->_init_eec3(this->_hdl, append);
		    			 break;
		    case GFI2  : this->_init_gfi2(this->_hdl, append);
		    			 break;
		    case EI    : this->_init_ei(this->_hdl, append);
		    			 break;
		    case FD    : this->_init_fd(this->_hdl, append);
		    	 		 break;
		    case HRVD  : this->_init_hrvd(this->_hdl, append);
		    		 	 break;
		    case TURBO : this->_init_turbo(this->_hdl, append);
		    			 break;
		    case VD    : this->_init_vd(this->_hdl, append);
		    	 		 break;
		    case RCFG  : this->_init_rcfg(this->_hdl, append);
		    		 	 break;
		    case TCFG  : this->_init_tcfg(this->_hdl, append);
		    		 	 break;
		    case ECFG  : this->_init_ecfg(this->_hdl, append);
		    		 	 break;
		    case ETEMP : this->_init_etemp(this->_hdl, append);
		    		 	 break;
		    case PTO   : this->_init_pto(this->_hdl, append);
		    		 	 break;
		    case CCVS  : this->_init_ccvs(this->_hdl, append);
		    		 	 break;
		    case LFE   : this->_init_lfe(this->_hdl, append);
		    		 	 break;
		    case AMBC  : this->_init_ambc(this->_hdl, append);
		    		 	 break;
		    case IEC   : this->_init_iec(this->_hdl, append);
		    		 	 break;
		    case VEP   : this->_init_vep(this->_hdl, append);
		    		 	 break;
		    case TF    : this->_init_tf(this->_hdl, append);
		    		 	 break;
		    case RF    : this->_init_rf(this->_hdl, append);
		    		 	 break;
		    default    : printf("Type %d is not valid.", types[i]);
		    			 break;
		}
	}
}


void DBManager::store(int type, void *data) {
	switch (type) {
		case PDU   : this->_store_pdu(data);
					 break;
		case TSC1  : this->_store_tsc1(data);
					 break;
		case ERC1  : this->_store_erc1(data);
					 break;
		case EBC1  : this->_store_ebc1(data);
					 break;
		case EBC2  : this->_store_ebc2(data);
					 break;
		case ETC1  : this->_store_etc1(data);
					 break;
		case ETC2  : this->_store_etc2(data);
					 break;
		case EEC1  : this->_store_eec1(data);
					 break;
		case EEC2  : this->_store_eec2(data);
					 break;
		case EEC3  : this->_store_eec3(data);
					 break;
		case GFI2  : this->_store_gfi2(data);
					 break;
		case EI    : this->_store_ei(data);
					 break;
		case FD    : this->_store_fd(data);
					 break;
		case HRVD  : this->_store_hrvd(data);
					 break;
		case TURBO : this->_store_turbo(data);
					 break;
		case VD    : this->_store_vd(data);
					 break;
		case RCFG  : this->_store_rcfg(data);
					 break;
		case TCFG  : this->_store_tcfg(data);
					 break;
		case ECFG  : this->_store_ecfg(data);
					 break;
		case ETEMP : this->_store_etemp(data);
					 break;
		case PTO   : this->_store_pto(data);
					 break;
		case CCVS  : this->_store_ccvs(data);
					 break;
		case LFE   : this->_store_lfe(data);
					 break;
		case AMBC  : this->_store_ambc(data);
					 break;
		case IEC   : this->_store_iec(data);
					 break;
		case VEP   : this->_store_vep(data);
					 break;
		case TF    : this->_store_tf(data);
					 break;
		case RF    : this->_store_rf(data);
					 break;
		default    : printf("Type %d is not valid.", type);
					 break;
	}
}


int DBManager::_init_table(qdb_hdl_t* hdl, string name, string schema) {
	char command[1000];

	sprintf(command,
			"IF EXISTS (SELECT * FROM INFORMATION_SCHEMA.TABLESWHERE "
			"TABLE_NAME = N'%s')\n BEGIN\n %s\n END",
			name.c_str(), schema.c_str());

	return qdb_statement(hdl, command);
}


int DBManager::_maybe_clear(qdb_hdl_t* hdl, string name, bool append) {
	int rc = 0;  /* FIXME: default if no append is required */
	char command[256];

	if (!append) {
		sprintf(command, "DELETE FROM %s;", name.c_str());
		rc = qdb_statement(hdl, command);
	}

	return rc;
}


int DBManager::_init_pdu(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE PDU("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" reserved INTEGER,"
			" data_page INTEGER,"
			" priority INTEGER,"
			" pdu_format INTEGER,"
			" pdu_specific INTEGER,"
			" src_address INTEGER,"
			" data_field INTEGER,"
			" num_bytes INTEGER"
			");";
	rc = this->_init_table(hdl, "PDU", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "PDU", append);

	return rc;
}


int DBManager::_store_pdu(void *data) {
	char command[256];
	j1939_pdu_typ *pdu = (j1939_pdu_typ*) data;

	sprintf(command,
			"INSERT INTO PDU(hour, minute, second, millisecond,"
			" reserved,"
			" data_page,"
			" priority,"
			" pdu_format,"
			" pdu_specific,"
			" src_address,"
			" data_field,"
			" num_bytes) "
			"VALUES(%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d);",
			pdu->timestamp.hour,
			pdu->timestamp.minute,
			pdu->timestamp.second,
			pdu->timestamp.millisecond,
			pdu->reserved,
			pdu->data_page,
			pdu->priority,
			pdu->pdu_format,
			pdu->pdu_specific,
			pdu->src_address,
			pdu->data_field,
			pdu->num_bytes);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_tsc1(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE TSC1("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" ovrd_ctrl_m INTEGER,"
			" req_spd_ctrl INTEGER,"
			" ovrd_ctrl_m_pr INTEGER,"
			" req_spd_lim REAL,"
			" req_trq_lim REAL,"
			" destination_address INTEGER,"
			" src_address INTEGER"
			");";
	rc = this->_init_table(hdl, "TSC1", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "TSC1", append);

	return rc;
}


int DBManager::_store_tsc1(void *data) {
	char command[256];
	j1939_tsc1_typ *tsc1 = (j1939_tsc1_typ*) data;

	sprintf(command,
			"INSERT INTO TSC1(hour, minute, second, millisecond,"
			" ovrd_ctrl_m,"
			" req_spd_ctrl,"
			" ovrd_ctrl_m_pr,"
			" req_spd_lim,"
			" req_trq_lim,"
			" destination_address,"
			" src_address) "
			"VALUES (%d, %d, %d, %d, %d, %d, %d, %.3f, %.3f, %d, %d);",
			tsc1->timestamp.hour,
			tsc1->timestamp.minute,
			tsc1->timestamp.second,
			tsc1->timestamp.millisecond,
			tsc1->ovrd_ctrl_m,
			tsc1->req_spd_ctrl,
			tsc1->ovrd_ctrl_m_pr,
			tsc1->req_spd_lim,
			tsc1->req_trq_lim,
			tsc1->destination_address,
			tsc1->src_address);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_erc1(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE ERC1("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" trq_mode INTEGER,"
			" enable_brake_assist INTEGER,"
			" enable_shift_assist INTEGER,"
			" actual_ret_pcnt_trq REAL,"
			" intended_ret_pcnt_trq REAL,"
			" rq_brake_light INTEGER,"
			" src_address_ctrl INTEGER,"
			" drvrs_demand_prcnt_trq INTEGER,"
			" selection_nonengine REAL,"
			" max_available_prcnt_trq INTEGER"
			");";
	rc = this->_init_table(hdl, "ERC1", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "ERC1", append);

	return rc;
}


int DBManager::_store_erc1(void *data) {
	char command[256];
	j1939_erc1_typ *erc1 = (j1939_erc1_typ*) data;

	sprintf(command,
			"INSERT INTO ERC1(hour, minute, second, millisecond,"
			" trq_mode,"
			" enable_brake_assist,"
			" enable_shift_assist,"
			" actual_ret_pcnt_trq,"
			" intended_ret_pcnt_trq,"
			" rq_brake_light,"
			" src_address_ctrl,"
			" drvrs_demand_prcnt_trq,"
			" selection_nonengine,"
			" max_available_prcnt_trq) "
			"VALUES(%d, %d, %d, %d, %d, %d, %d, %.3f, %.3f, %d, %d, %d, %.3f, "
			"%d)",
			erc1->timestamp.hour,
			erc1->timestamp.minute,
			erc1->timestamp.second,
			erc1->timestamp.millisecond,
			erc1->trq_mode,
			erc1->enable_brake_assist,
			erc1->enable_shift_assist,
			erc1->actual_ret_pcnt_trq,
			erc1->intended_ret_pcnt_trq,
			erc1->rq_brake_light,
			erc1->src_address_ctrl,
			erc1->drvrs_demand_prcnt_trq,
			erc1->selection_nonengine,
			erc1->max_available_prcnt_trq);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_ebc1(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE EBC1("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" asr_engine_ctrl_active INTEGER,"
			" asr_brk_ctrl_active INTEGER,"
			" antilock_brk_active INTEGER,"
			" ebs_brk_switch INTEGER,"
			" brk_pedal_pos REAL,"
			" abs_offroad_switch INTEGER,"
			" asr_offroad_switch INTEGER,"
			" asr_hillholder_switch INTEGER,"
			" trac_ctrl_override_switch INTEGER,"
			" accel_interlock_switch INTEGER,"
			" eng_derate_switch INTEGER,"
			" aux_eng_shutdown_switch INTEGER,"
			" accel_enable_switch INTEGER,"
			" eng_retarder_selection REAL,"
			" abs_fully_operational INTEGER,"
			" ebs_red_warning INTEGER,"
			" abs_ebs_amber_warning INTEGER,"
			" src_address_ctrl INTEGER,"
			" total_brk_demand REAL"
			");";
	rc = this->_init_table(hdl, "EBC1", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "EBC1", append);

	return rc;
}


int DBManager::_store_ebc1(void *data) {
	char command[256];
	j1939_ebc1_typ *ebc1 = (j1939_ebc1_typ*) data;

	sprintf(command,
			"INSERT INTO EBC1(hour, minute, second, millisecond,"
			" asr_engine_ctrl_active,"
			" asr_brk_ctrl_active,"
			" antilock_brk_active,"
			" ebs_brk_switch,"
			" brk_pedal_pos,"
			" abs_offroad_switch,"
			" asr_offroad_switch,"
			" asr_hillholder_switch,"
			" trac_ctrl_override_switch,"
			" accel_interlock_switch,"
			" eng_derate_switch,"
			" aux_eng_shutdown_switch,"
			" accel_enable_switch,"
			" eng_retarder_selection,"
			" abs_fully_operational,"
			" ebs_red_warning,"
			" abs_ebs_amber_warning,"
			" src_address_ctrl,"
			" total_brk_demand) "
			"VALUES(%d, %d, %d, %d, %d, %d, %d, %d, %.3f, %d, %d, %d, %d, %d, "
			"%d, %d, %d, %.3f, %d, %d, %d, %d, %.3f);",
			ebc1->timestamp.hour,
			ebc1->timestamp.minute,
			ebc1->timestamp.second,
			ebc1->timestamp.millisecond,
			ebc1->asr_engine_ctrl_active,
			ebc1->asr_brk_ctrl_active,
			ebc1->antilock_brk_active,
			ebc1->ebs_brk_switch,
			ebc1->brk_pedal_pos,
			ebc1->abs_offroad_switch,
			ebc1->asr_offroad_switch,
			ebc1->asr_hillholder_switch,
			ebc1->trac_ctrl_override_switch,
			ebc1->accel_interlock_switch,
			ebc1->eng_derate_switch,
			ebc1->aux_eng_shutdown_switch,
			ebc1->accel_enable_switch,
			ebc1->eng_retarder_selection,
			ebc1->abs_fully_operational,
			ebc1->ebs_red_warning,
			ebc1->abs_ebs_amber_warning,
			ebc1->src_address_ctrl,
			ebc1->total_brk_demand);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_ebc2(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE EBC2("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" front_axle_spd REAL,"
			" rel_spd_front_left REAL,"
			" rel_spd_front_right REAL,"
			" rel_spd_rear_left_1 REAL,"
			" rel_spd_rear_right_1 REAL,"
			" rel_spd_rear_left_2 REAL,"
			" rel_spd_rear_right_2 REAL"
			");";
	rc = this->_init_table(hdl, "EBC2", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "EBC2", append);

	return rc;
}


int DBManager::_store_ebc2(void *data) {
	char command[256];
	j1939_ebc2_typ *ebc2 = (j1939_ebc2_typ*) data;

	sprintf(command,
			"INSERT INTO EBC2(hour, minute, second, millisecond,"
			" front_axle_spd,"
			" rel_spd_front_left,"
			" rel_spd_front_right,"
			" rel_spd_rear_left_1,"
			" rel_spd_rear_right_1,"
			" rel_spd_rear_left_2,"
			" rel_spd_rear_right_2) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f);",
			ebc2->timestamp.hour,
			ebc2->timestamp.minute,
			ebc2->timestamp.second,
			ebc2->timestamp.millisecond,
			ebc2->front_axle_spd,
			ebc2->rel_spd_front_left,
			ebc2->rel_spd_front_right,
			ebc2->rel_spd_rear_left_1,
			ebc2->rel_spd_rear_right_1,
			ebc2->rel_spd_rear_left_2,
			ebc2->rel_spd_rear_right_2);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_etc1(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE ETC1("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" trans_driveline INTEGER,"
			" trq_conv_lockup INTEGER,"
			" trans_shift INTEGER,"
			" tran_output_shaft_spd REAL,"
			" prcnt_clutch_slip REAL,"
			" eng_overspd_enable INTEGER,"
			" prog_shift_disable INTEGER,"
			" trans_input_shaft_spd REAL,"
			" src_address_ctrl INTEGER"
			");";
	rc = this->_init_table(hdl, "ETC1", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "ETC1", append);

	return rc;
}


int DBManager::_store_etc1(void *data) {
	char command[256];
	j1939_etc1_typ *etc1 = (j1939_etc1_typ*) data;

	sprintf(command,
			"INSERT INTO ETC1(hour, minute, second, millisecond,"
			" trans_driveline,"
			" trq_conv_lockup,"
			" trans_shift,"
			" tran_output_shaft_spd,"
			" prcnt_clutch_slip,"
			" eng_overspd_enable,"
			" prog_shift_disable,"
			" trans_input_shaft_spd,"
			" src_address_ctrl) "
			"VALUES(%d, %d, %d, %d, %d, %d, %d, %.3f, %.3f, %d, %d, %.3f, %d);",
			etc1->timestamp.hour,
			etc1->timestamp.minute,
			etc1->timestamp.second,
			etc1->timestamp.millisecond,
			etc1->trans_driveline,
			etc1->trq_conv_lockup,
			etc1->trans_shift,
			etc1->tran_output_shaft_spd,
			etc1->prcnt_clutch_slip,
			etc1->eng_overspd_enable,
			etc1->prog_shift_disable,
			etc1->trans_input_shaft_spd,
			etc1->src_address_ctrl);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_etc2(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE ETC2("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" trans_selected_gear INTEGER,"
			" trans_act_gear_ratio REAL,"
			" trans_current_gear INTEGER,"
			" range_selected INTEGER,"
			" range_attained INTEGER"
			");";
	rc = this->_init_table(hdl, "ETC2", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "ETC2", append);

	return rc;
}


int DBManager::_store_etc2(void *data) {
	char command[256];
	j1939_etc2_typ *etc2 = (j1939_etc2_typ*) data;

	sprintf(command,
			"INSERT INTO ETC2(hour, minute, second, millisecond,"
			" trans_selected_gear,"
			" trans_act_gear_ratio,"
			" trans_current_gear,"
			" range_selected,"
			" range_attained) "
			"VALUES(%d, %d, %d, %d, %d, %.3f, %d, %d, %d)",
			etc2->timestamp.hour,
			etc2->timestamp.minute,
			etc2->timestamp.second,
			etc2->timestamp.millisecond,
			etc2->trans_selected_gear,
			etc2->trans_act_gear_ratio,
			etc2->trans_current_gear,
			etc2->range_selected,
			etc2->range_attained);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_eec1(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE EEC1("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" eng_trq_mode INTEGER,"
			" drvr_demand_eng_trq REAL,"
			" actual_eng_trq REAL,"
			" eng_spd REAL,"
			" eng_demand_trq REAL,"
			" src_address INTEGER"
			");";
	rc = this->_init_table(hdl, "EEC1", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "EEC1", append);

	return rc;
}


int DBManager::_store_eec1(void *data) {
	char command[256];
	j1939_eec1_typ *eec1 = (j1939_eec1_typ*) data;

	sprintf(command,
			"INSERT INTO EEC1(hour, minute, second, millisecond"
			" eng_trq_mode,"
			" drvr_demand_eng_trq,"
			" actual_eng_trq,"
			" eng_spd,"
			" eng_demand_trq,"
			" src_address) "
			"VALUES(%d, %d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %d);",
			eec1->timestamp.hour,
			eec1->timestamp.minute,
			eec1->timestamp.second,
			eec1->timestamp.millisecond,
			eec1->eng_trq_mode,
			eec1->drvr_demand_eng_trq,
			eec1->actual_eng_trq,
			eec1->eng_spd,
			eec1->eng_demand_trq,
			eec1->src_address);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_eec2(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE EEC2("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" accel_pedal1_idle INTEGER,"
			" accel_pedal_kickdown INTEGER,"
			" spd_limit_status INTEGER,"
			" accel_pedal2_idle INTEGER,"
			" accel_pedal1_pos REAL,"
			" eng_prcnt_load_curr_spd REAL,"
			" accel_pedal2_pos REAL,"
			" act_max_avail_eng_trq REAL"
			");";
	rc = this->_init_table(hdl, "EEC2", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "EEC2", append);

	return rc;
}


int DBManager::_store_eec2(void *data) {
	char command[256];
	j1939_eec2_typ *eec2 = (j1939_eec2_typ*) data;

	sprintf(command,
			"INSERT INTO EEC2(hour, minute, second, millisecond,"
			" accel_pedal1_idle,"
			" accel_pedal_kickdown,"
			" spd_limit_status,"
			" accel_pedal2_idle,"
			" accel_pedal1_pos,"
			" eng_prcnt_load_curr_spd,"
			" accel_pedal2_pos,"
			" act_max_avail_eng_trq) "
			"VALUES(%d, %d, %d, %d, %d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f);",
			eec2->timestamp.hour,
			eec2->timestamp.minute,
			eec2->timestamp.second,
			eec2->timestamp.millisecond,
			eec2->accel_pedal1_idle,
			eec2->accel_pedal_kickdown,
			eec2->spd_limit_status,
			eec2->accel_pedal2_idle,
			eec2->accel_pedal1_pos,
			eec2->eng_prcnt_load_curr_spd,
			eec2->accel_pedal2_pos,
			eec2->act_max_avail_eng_trq);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_eec3(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE EEC3("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" nominal_friction REAL,"
			" desired_operating_spd REAL,"
			" operating_spd_adjust INTEGER,"
			" est_eng_prstic_loss REAL"
			");";
	rc = this->_init_table(hdl, "EEC3", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "EEC3", append);

	return rc;
}


int DBManager::_store_eec3(void *data) {
	char command[256];
	j1939_eec3_typ *eec3 = (j1939_eec3_typ*) data;

	sprintf(command,
			"INSERT INTO EEC3(hour, minute, second, millisecond"
			" nominal_friction,"
			" desired_operating_spd,"
			" operating_spd_adjust,"
			" est_eng_prstic_loss) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f, %d, %.3f);",
			eec3->timestamp.hour,
			eec3->timestamp.minute,
			eec3->timestamp.second,
			eec3->timestamp.millisecond,
			eec3->nominal_friction,
			eec3->desired_operating_spd,
			eec3->operating_spd_adjust,
			eec3->est_eng_prstic_loss);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_gfi2(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE GFI2("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" fuel_flow_rate1 REAL,"
			" fuel_flow_rate2 REAL,"
			" fuel_valve_pos1 REAL,"
			" fuel_valve_pos2 REAL"
			");";
	rc = this->_init_table(hdl, "GFI2", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "GFI2", append);

	return rc;
}


int DBManager::_store_gfi2(void *data) {
	char command[256];
	j1939_gfi2_typ *gfi2 = (j1939_gfi2_typ*) data;

	sprintf(command,
			"INSERT INTO GFI2(hour, minute, second, millisecond,"
			" fuel_flow_rate1,"
			" fuel_flow_rate2,"
			" fuel_valve_pos1,"
			" fuel_valve_pos2) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f);",
			gfi2->timestamp.hour,
			gfi2->timestamp.minute,
			gfi2->timestamp.second,
			gfi2->timestamp.millisecond,
			gfi2->fuel_flow_rate1,
			gfi2->fuel_flow_rate2,
			gfi2->fuel_valve_pos1,
			gfi2->fuel_valve_pos2);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_ei(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE EI("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" pre_filter_oil_pressure REAL,"
			" exhaust_gas_pressure REAL,"
			" rack_position REAL,"
			" eng_gas_mass_flow REAL,"
			" inst_estimated_brake_power REAL"
			");";
	rc = this->_init_table(hdl, "EI", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "EI", append);

	return rc;
}


int DBManager::_store_ei(void *data) {
	char command[256];
	j1939_ei_typ *ei = (j1939_ei_typ*) data;

	sprintf(command,
			"INSERT INTO EI(hour, minute, second, millisecond,"
			" pre_filter_oil_pressure,"
			" exhaust_gas_pressure,"
			" rack_position,"
			" eng_gas_mass_flow,"
			" inst_estimated_brake_power) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %.3f);",
			ei->timestamp.hour,
			ei->timestamp.minute,
			ei->timestamp.second,
			ei->timestamp.millisecond,
			ei->pre_filter_oil_pressure,
			ei->exhaust_gas_pressure,
			ei->rack_position,
			ei->eng_gas_mass_flow,
			ei->inst_estimated_brake_power);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_fd(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE FD("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" prcnt_fan_spd REAL,"
			" fan_drive_state INTEGER"
			");";
	rc = this->_init_table(hdl, "FD", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "FD", append);

	return rc;
}


int DBManager::_store_fd(void *data) {
	char command[256];
	j1939_fd_typ *fd = (j1939_fd_typ*) data;

	sprintf(command,
			"INSERT INTO FD(hour, minute, second, millisecond,"
			" prcnt_fan_spd,"
			" fan_drive_state) "
			"VALUES(%d, %d, %d, %d, %.3f, %d);",
			fd->timestamp.hour,
			fd->timestamp.minute,
			fd->timestamp.second,
			fd->timestamp.millisecond,
			fd->prcnt_fan_spd,
			fd->fan_drive_state);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_hrvd(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE HRVD("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" vehicle_distance REAL,"
			" trip_distance REAL"
			");";
	rc = this->_init_table(hdl, "HRVD", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "HRVD", append);

	return rc;
}


int DBManager::_store_hrvd(void *data) {
	char command[256];
	j1939_hrvd_typ *hrvd = (j1939_hrvd_typ*) data;

	sprintf(command,
			"INSERT INTO HRVD("
			" hour,"
			" minute,"
			" second,"
			" millisecond,"
			" vehicle_distance,"
			" trip_distance) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f);",
			hrvd->timestamp.hour,
			hrvd->timestamp.minute,
			hrvd->timestamp.second,
			hrvd->timestamp.millisecond,
			hrvd->vehicle_distance,
			hrvd->trip_distance);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_turbo(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE TURBO("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" turbo_lube_oil_pressure REAL,"
			" turbo_speed REAL"
			");";
	rc = this->_init_table(hdl, "TURBO", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "TURBO", append);

	return rc;
}


int DBManager::_store_turbo(void *data) {
	char command[256];
	j1939_turbo_typ *turbo = (j1939_turbo_typ*) data;

	sprintf(command,
			"INSERT INTO TURBO(hour, minute, second, millisecond,"
			" turbo_lube_oil_pressure,"
			" turbo_speed) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f);",
			turbo->timestamp.hour,
			turbo->timestamp.minute,
			turbo->timestamp.second,
			turbo->timestamp.millisecond,
			turbo->turbo_lube_oil_pressure,
			turbo->turbo_speed);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_vd(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE VD("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" trip_dist REAL,"
			" tot_vehicle_dist REAL"
			");";
	rc = this->_init_table(hdl, "VD", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "VD", append);

	return rc;
}


int DBManager::_store_vd(void *data) {
	char command[256];
	j1939_vd_typ *vd = (j1939_vd_typ*) data;

	sprintf(command,
			"INSERT INTO VD(hour, minute, second, millisecond,"
			" trip_dist,"
			" tot_vehicle_dist) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f);",
			vd->timestamp.hour,
			vd->timestamp.minute,
			vd->timestamp.second,
			vd->timestamp.millisecond,
			vd->trip_dist,
			vd->tot_vehicle_dist);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_rcfg(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE RCFG("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" retarder_type INTEGER,"
			" retarder_loc INTEGER,"
			" retarder_ctrl_steps INTEGER,"
			" retarder_speed_0 REAL,"
			" retarder_speed_1 REAL,"
			" retarder_speed_2 REAL,"
			" retarder_speed_3 REAL,"
			" retarder_speed_4 REAL,"
			" percent_torque_0 REAL,"
			" percent_torque_1 REAL,"
			" percent_torque_2 REAL,"
			" percent_torque_3 REAL,"
			" percent_torque_4 REAL,"
			" reference_retarder_trq REAL"
			");";
	rc = this->_init_table(hdl, "RCFG", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "RCFG", append);

	return rc;
}


int DBManager::_store_rcfg(void *data) {
	char command[256];
	j1939_rcfg_typ *rcfg = (j1939_rcfg_typ*) data;

	sprintf(command,
			"INSERT INTO RCFG(hour, minute, second, millisecond,"
			" retarder_type,"
			" retarder_loc,"
			" retarder_ctrl_steps,"
			" retarder_speed_0,"
			" retarder_speed_1,"
			" retarder_speed_2,"
			" retarder_speed_3,"
			" retarder_speed_4,"
			" percent_torque_0,"
			" percent_torque_1,"
			" percent_torque_2,"
			" percent_torque_3,"
			" percent_torque_4,"
			" reference_retarder_trq) "
			"VALUES(%d, %d, %d, %d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %.3f,"
			" %.3f, %.3f, %.3f, %.3f, %.3f, %.3f);",
			rcfg->timestamp.hour,
			rcfg->timestamp.minute,
			rcfg->timestamp.second,
			rcfg->timestamp.millisecond,
			rcfg->retarder_type,
			rcfg->retarder_loc,
			rcfg->retarder_ctrl_steps,
			rcfg->retarder_speed[0],
			rcfg->retarder_speed[1],
			rcfg->retarder_speed[2],
			rcfg->retarder_speed[3],
			rcfg->retarder_speed[4],
			rcfg->percent_torque[0],
			rcfg->percent_torque[1],
			rcfg->percent_torque[2],
			rcfg->percent_torque[3],
			rcfg->percent_torque[4],
			rcfg->reference_retarder_trq
			);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_tcfg(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE TCFG("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" num_rev_gear_ratios INTEGER,"
			" num_fwd_gear_ratios INTEGER,"
			" rev_gear_ratios_0 REAL,"
			" rev_gear_ratios_1 REAL,"
			" rev_gear_ratios_2 REAL,"
			" rev_gear_ratios_3 REAL,"
			" rev_gear_ratios_4 REAL,"
			" rev_gear_ratios_5 REAL,"
			" rev_gear_ratios_6 REAL,"
			" rev_gear_ratios_7 REAL,"
			" fwd_gear_rations_0 REAL,"
			" fwd_gear_rations_1 REAL,"
			" fwd_gear_rations_2 REAL,"
			" fwd_gear_rations_3 REAL,"
			" fwd_gear_rations_4 REAL,"
			" fwd_gear_rations_5 REAL,"
			" fwd_gear_rations_6 REAL,"
			" fwd_gear_rations_7 REAL,"
			" fwd_gear_rations_8 REAL,"
			" fwd_gear_rations_9 REAL,"
			" fwd_gear_rations_10 REAL,"
			" fwd_gear_rations_11 REAL,"
			" fwd_gear_rations_12 REAL,"
			" fwd_gear_rations_13 REAL,"
			" fwd_gear_rations_14 REAL,"
			" fwd_gear_rations_15 REAL"
			");";
	rc = this->_init_table(hdl, "TCFG", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "TCFG", append);

	return rc;
}


int DBManager::_store_tcfg(void *data) {
	char command[256];
	j1939_tcfg_typ *tcfg = (j1939_tcfg_typ*) data;

	sprintf(command,
			"INSERT INTO TCFG(hour, minute, second, millisecond,"
			" num_rev_gear_ratios,"
			" num_fwd_gear_ratios,"
			" rev_gear_ratios_0,"
			" rev_gear_ratios_1,"
			" rev_gear_ratios_2,"
			" rev_gear_ratios_3,"
			" rev_gear_ratios_4,"
			" rev_gear_ratios_5,"
			" rev_gear_ratios_6,"
			" rev_gear_ratios_7,"
			" fwd_gear_rations_0,"
			" fwd_gear_rations_1,"
			" fwd_gear_rations_2,"
			" fwd_gear_rations_3,"
			" fwd_gear_rations_4,"
			" fwd_gear_rations_5,"
			" fwd_gear_rations_6,"
			" fwd_gear_rations_7,"
			" fwd_gear_rations_8,"
			" fwd_gear_rations_9,"
			" fwd_gear_rations_10,"
			" fwd_gear_rations_11,"
			" fwd_gear_rations_12,"
			" fwd_gear_rations_13,"
			" fwd_gear_rations_14,"
			" fwd_gear_rations_15) "
			"VALUES(%d, %d, %d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f,"
			" %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f,"
			" %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f);",
			tcfg->timestamp.hour,
			tcfg->timestamp.minute,
			tcfg->timestamp.second,
			tcfg->timestamp.millisecond,
			tcfg->num_rev_gear_ratios,
			tcfg->num_fwd_gear_ratios,
			tcfg->rev_gear_ratios[0],
			tcfg->rev_gear_ratios[1],
			tcfg->rev_gear_ratios[2],
			tcfg->rev_gear_ratios[3],
			tcfg->rev_gear_ratios[4],
			tcfg->rev_gear_ratios[5],
			tcfg->rev_gear_ratios[6],
			tcfg->rev_gear_ratios[7],
			tcfg->fwd_gear_rations[0],
			tcfg->fwd_gear_rations[1],
			tcfg->fwd_gear_rations[2],
			tcfg->fwd_gear_rations[3],
			tcfg->fwd_gear_rations[4],
			tcfg->fwd_gear_rations[5],
			tcfg->fwd_gear_rations[6],
			tcfg->fwd_gear_rations[7],
			tcfg->fwd_gear_rations[8],
			tcfg->fwd_gear_rations[9],
			tcfg->fwd_gear_rations[10],
			tcfg->fwd_gear_rations[11],
			tcfg->fwd_gear_rations[12],
			tcfg->fwd_gear_rations[13],
			tcfg->fwd_gear_rations[14],
			tcfg->fwd_gear_rations[15]);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_ecfg(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE ECFG("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" engine_spd_0 REAL,"
			" engine_spd_1 REAL,"
			" engine_spd_2 REAL,"
			" engine_spd_3 REAL,"
			" engine_spd_4 REAL,"
			" engine_spd_5 REAL,"
			" engine_spd_6 REAL,"
			" percent_trq_0 REAL,"
			" percent_trq_1 REAL,"
			" percent_trq_2 REAL,"
			" percent_trq_3 REAL,"
			" percent_trq_4 REAL,"
			" gain_endspeed_governor REAL,"
			" reference_eng_trq REAL,"
			" max_momentary_overide_time REAL,"
			" spd_ctrl_lower_lim REAL,"
			" spd_ctrl_upper_lim REAL,"
			" trq_ctrl_lower_lim REAL,"
			" trq_ctrl_upper_lim REAL,"
			" receive_status INTEGER"
			");";
	rc = this->_init_table(hdl, "ECFG", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "ECFG", append);

	return rc;
}


int DBManager::_store_ecfg(void *data) {
	char command[256];
	j1939_ecfg_typ *ecfg = (j1939_ecfg_typ*) data;

	sprintf(command,
			"INSERT INTO ECFG(hour, minute, second, millisecond,"
			" engine_spd_0,"
			" engine_spd_1,"
			" engine_spd_2,"
			" engine_spd_3,"
			" engine_spd_4,"
			" engine_spd_5,"
			" engine_spd_6,"
			" percent_trq_0,"
			" percent_trq_1,"
			" percent_trq_2,"
			" percent_trq_3,"
			" percent_trq_4,"
			" gain_endspeed_governor,"
			" reference_eng_trq,"
			" max_momentary_overide_time,"
			" spd_ctrl_lower_lim,"
			" spd_ctrl_upper_lim,"
			" trq_ctrl_lower_lim,"
			" trq_ctrl_upper_lim,"
			" receive_status) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f,"
			" %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f,"
			" %.3f, %d);",
			ecfg->timestamp.hour,
			ecfg->timestamp.minute,
			ecfg->timestamp.second,
			ecfg->timestamp.millisecond,
			ecfg->engine_spd[0],
			ecfg->engine_spd[1],
			ecfg->engine_spd[2],
			ecfg->engine_spd[3],
			ecfg->engine_spd[4],
			ecfg->engine_spd[5],
			ecfg->engine_spd[6],
			ecfg->percent_trq[0],
			ecfg->percent_trq[1],
			ecfg->percent_trq[2],
			ecfg->percent_trq[3],
			ecfg->percent_trq[4],
			ecfg->gain_endspeed_governor,
			ecfg->reference_eng_trq,
			ecfg->max_momentary_overide_time,
			ecfg->spd_ctrl_lower_lim,
			ecfg->spd_ctrl_upper_lim,
			ecfg->trq_ctrl_lower_lim,
			ecfg->trq_ctrl_upper_lim,
			ecfg->receive_status);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_etemp(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE ETEMP("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" eng_coolant_temp REAL,"
			" fuel_temp REAL,"
			" eng_oil_temp REAL,"
			" turbo_oil_temp REAL,"
			" eng_intercooler_temp REAL,"
			" eng_intercooler_thermostat_opening REAL"
			");";
	rc = this->_init_table(hdl, "ETEMP", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "ETEMP", append);

	return rc;
}


int DBManager::_store_etemp(void *data) {
	char command[256];
	j1939_etemp_typ *etemp = (j1939_etemp_typ*) data;

	sprintf(command,
			"INSERT INTO ETEMP(hour, minute, second, millisecond,"
			" eng_coolant_temp,"
			" fuel_temp,"
			" eng_oil_temp,"
			" turbo_oil_temp,"
			" eng_intercooler_temp,"
			" eng_intercooler_thermostat_opening) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f);",
			etemp->timestamp.hour,
			etemp->timestamp.minute,
			etemp->timestamp.second,
			etemp->timestamp.millisecond,
			etemp->eng_coolant_temp,
			etemp->fuel_temp,
			etemp->eng_oil_temp,
			etemp->turbo_oil_temp,
			etemp->eng_intercooler_temp,
			etemp->eng_intercooler_thermostat_opening);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_pto(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE PTO("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" oil_temp REAL,"
			" speed REAL,"
			" set_speed REAL,"
			" enable_switch INTEGER,"
			" remote_preprogramm_status INTEGER,"
			" remote_variable_spd_status INTEGER,"
			" set_switch INTEGER,"
			" coast_decel_switch INTEGER,"
			" resume_switch INTEGER,"
			" accel_switch INTEGER"
			");";
	rc = this->_init_table(hdl, "PTO", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "PTO", append);

	return rc;
}


int DBManager::_store_pto(void *data) {
	char command[256];
	j1939_pto_typ *pto = (j1939_pto_typ*) data;

	sprintf(command,
			"INSERT INTO PTO(hour, minute, second, millisecond,"
			" oil_temp,"
			" speed,"
			" set_speed,"
			" enable_switch,"
			" remote_preprogramm_status,"
			" remote_variable_spd_status,"
			" set_switch,"
			" coast_decel_switch,"
			" resume_switch,"
			" accel_switch) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f, %.3f, %d, %d, %d, %d, %d, %d,"
			" %d);",
			pto->timestamp.hour,
			pto->timestamp.minute,
			pto->timestamp.second,
			pto->timestamp.millisecond,
			pto->oil_temp,
			pto->speed,
			pto->set_speed,
			pto->enable_switch,
			pto->remote_preprogramm_status,
			pto->remote_variable_spd_status,
			pto->set_switch,
			pto->coast_decel_switch,
			pto->resume_switch,
			pto->accel_switch);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_ccvs(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE CCVS("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" two_spd_axle_switch INTEGER,"
			" parking_brk_switch INTEGER,"
			" cc_pause_switch INTEGER,"
			" park_brk_release INTEGER,"
			" vehicle_spd REAL,"
			" cc_active INTEGER,"
			" cc_enable_switch INTEGER,"
			" brk_switch INTEGER,"
			" clutch_switch INTEGER,"
			" cc_set_switch INTEGER,"
			" cc_coast_switch INTEGER,"
			" cc_resume_switch INTEGER,"
			" cc_accel_switch INTEGER,"
			" cc_set_speed REAL,"
			" pto_state INTEGER,"
			" cc_state INTEGER,"
			" eng_idle_incr_switch INTEGER,"
			" eng_idle_decr_switch INTEGER,"
			" eng_test_mode_switch INTEGER,"
			" eng_shutdown_override INTEGER"
			");";
	rc = this->_init_table(hdl, "CCVS", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "CCVS", append);

	return rc;
}


int DBManager::_store_ccvs(void *data) {
	char command[256];
	j1939_ccvs_typ *ccvs = (j1939_ccvs_typ*) data;

	sprintf(command,
			"INSERT INTO CCVS(hour, minute, second, millisecond,"
			" two_spd_axle_switch,"
			" parking_brk_switch,"
			" cc_pause_switch,"
			" park_brk_release,"
			" vehicle_spd,"
			" cc_active,"
			" cc_enable_switch,"
			" brk_switch,"
			" clutch_switch,"
			" cc_set_switch,"
			" cc_coast_switch,"
			" cc_resume_switch,"
			" cc_accel_switch,"
			" cc_set_speed,"
			" pto_state,"
			" cc_state,"
			" eng_idle_incr_switch,"
			" eng_idle_decr_switch,"
			" eng_test_mode_switch,"
			" eng_shutdown_override) "
			"VALUES(%d, %d, %d, %d, %d, %d, %d, %d, %.3f, %d, %d, %d, %d, %d,"
			" %d, %d, %d, %.3f, %d, %d, %d, %d, %d, %d);",
			ccvs->timestamp.hour,
			ccvs->timestamp.minute,
			ccvs->timestamp.second,
			ccvs->timestamp.millisecond,
			ccvs->two_spd_axle_switch,
			ccvs->parking_brk_switch,
			ccvs->cc_pause_switch,
			ccvs->park_brk_release,
			ccvs->vehicle_spd,
			ccvs->cc_active,
			ccvs->cc_enable_switch,
			ccvs->brk_switch,
			ccvs->clutch_switch,
			ccvs->cc_set_switch,
			ccvs->cc_coast_switch,
			ccvs->cc_resume_switch,
			ccvs->cc_accel_switch,
			ccvs->cc_set_speed,
			ccvs->pto_state,
			ccvs->cc_state,
			ccvs->eng_idle_incr_switch,
			ccvs->eng_idle_decr_switch,
			ccvs->eng_test_mode_switch,
			ccvs->eng_shutdown_override);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_lfe(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE LFE("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" eng_fuel_rate REAL,"
			" eng_inst_fuel_economy REAL,"
			" eng_avg_fuel_economy REAL,"
			" eng_throttle1_pos REAL,"
			" eng_throttle2_pos REAL"
			");";
	rc = this->_init_table(hdl, "LFE", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "LFE", append);

	return rc;
}


int DBManager::_store_lfe(void *data) {
	char command[256];
	j1939_lfe_typ *lfe = (j1939_lfe_typ*) data;

	sprintf(command,
			"INSERT INTO LFE(hour, minute, second, millisecond,"
			" eng_fuel_rate,"
			" eng_inst_fuel_economy,"
			" eng_avg_fuel_economy,"
			" eng_throttle1_pos,"
			" eng_throttle2_pos) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %.3f);",
			lfe->timestamp.hour,
			lfe->timestamp.minute,
			lfe->timestamp.second,
			lfe->timestamp.millisecond,
			lfe->eng_fuel_rate,
			lfe->eng_inst_fuel_economy,
			lfe->eng_avg_fuel_economy,
			lfe->eng_throttle1_pos,
			lfe->eng_throttle2_pos);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_ambc(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE AMBC("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" barometric_pressure REAL,"
			" cab_interior_temp REAL,"
			" ambient_air_temp REAL,"
			" air_inlet_temp REAL,"
			" road_surface_temp REAL"
			");";
	rc = this->_init_table(hdl, "AMBC", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "AMBC", append);

	return rc;
}


int DBManager::_store_ambc(void *data) {
	char command[256];
	j1939_ambc_typ *ambc = (j1939_ambc_typ*) data;

	sprintf(command,
			"INSERT INTO AMBC(hour, minute, second, millisecond,"
			" barometric_pressure,"
			" cab_interior_temp,"
			" ambient_air_temp,"
			" air_inlet_temp,"
			" road_surface_temp) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %.3f);",
			ambc->timestamp.hour,
			ambc->timestamp.minute,
			ambc->timestamp.second,
			ambc->timestamp.millisecond,
			ambc->barometric_pressure,
			ambc->cab_interior_temp,
			ambc->ambient_air_temp,
			ambc->air_inlet_temp,
			ambc->road_surface_temp);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_iec(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE IEC("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" particulate_inlet_pressure REAL,"
			" boost_pressure REAL,"
			" intake_manifold_temp REAL,"
			" air_inlet_pressure REAL,"
			" air_filter_diff_pressure REAL,"
			" exhaust_gas_temp REAL,"
			" coolant_filter_diff_pressure REAL"
			");";
	rc = this->_init_table(hdl, "IEC", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "IEC", append);

	return rc;
}


int DBManager::_store_iec(void *data) {
	char command[256];
	j1939_iec_typ *iec = (j1939_iec_typ*) data;

	sprintf(command,
			"INSERT INTO IEC(hour, minute, second, millisecond,"
			" particulate_inlet_pressure,"
			" boost_pressure,"
			" intake_manifold_temp,"
			" air_inlet_pressure,"
			" air_filter_diff_pressure,"
			" exhaust_gas_temp,"
			" coolant_filter_diff_pressure) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f, %.3f);",
			iec->timestamp.hour,
			iec->timestamp.minute,
			iec->timestamp.second,
			iec->timestamp.millisecond,
			iec->particulate_inlet_pressure,
			iec->boost_pressure,
			iec->intake_manifold_temp,
			iec->air_inlet_pressure,
			iec->air_filter_diff_pressure,
			iec->exhaust_gas_temp,
			iec->coolant_filter_diff_pressure);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_vep(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE VEP("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" net_battery_current REAL,"
			" alternator_current REAL,"
			" alternator_potential REAL,"
			" electrical_potential REAL,"
			" battery_potential REAL"
			");";
	rc = this->_init_table(hdl, "VEP", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "VEP", append);

	return rc;
}


int DBManager::_store_vep(void *data) {
	char command[256];
	j1939_vep_typ *vep = (j1939_vep_typ*) data;

	sprintf(command,
			"INSERT INTO VEP(hour, minute, second, millisecond,"
			" net_battery_current,"
			" alternator_current,"
			" alternator_potential,"
			" electrical_potential,"
			" battery_potential) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %.3f);",
			vep->timestamp.hour,
			vep->timestamp.minute,
			vep->timestamp.second,
			vep->timestamp.millisecond,
			vep->net_battery_current,
			vep->alternator_current,
			vep->alternator_potential,
			vep->electrical_potential,
			vep->battery_potential);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_tf(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE TF("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" clutch_pressure REAL,"
			" oil_level REAL,"
			" diff_pressure REAL,"
			" oil_pressure REAL,"
			" oil_temp REAL"
			");";
	rc = this->_init_table(hdl, "TF", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "TF", append);

	return rc;
}


int DBManager::_store_tf(void *data) {
	char command[256];
	j1939_tf_typ *tf = (j1939_tf_typ*) data;

	sprintf(command,
			"INSERT INTO TF(hour, minute, second, millisecond,"
			" clutch_pressure,"
			" oil_level,"
			" diff_pressure,"
			" oil_pressure,"
			" oil_temp) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f, %.3f, %.3f, %.3f);",
			tf->timestamp.hour,
			tf->timestamp.minute,
			tf->timestamp.second,
			tf->timestamp.millisecond,
			tf->clutch_pressure,
			tf->oil_level,
			tf->diff_pressure,
			tf->oil_pressure,
			tf->oil_temp);

	return qdb_statement(this->_hdl, command);
}


int DBManager::_init_rf(qdb_hdl_t* hdl, bool append) {
	int rc;

	/* If the database table does not exist, create it. */
	string schema =
			"CREATE TABLE RF("
			" id INTEGER PRIMARY KEY AUTOINCREMENT,"
			" hour INTEGER,"
			" minute INTEGER,"
			" second INTEGER,"
			" millisecond INTEGER,"
			" pressure REAL,"
			" oil_temp REAL"
			");";
	rc = this->_init_table(hdl, "RF", schema);

	/* If the new data elements should not be appended onto an existing
	 * database, clear any elements from the existing database. */
	rc = this->_maybe_clear(hdl, "RF", append);

	return rc;
}


int DBManager::_store_rf(void *data) {
	char command[256];
	j1939_rf_typ *rf = (j1939_rf_typ*) data;

	sprintf(command,
			"INSERT INTO RF(hour, minute, second, millisecond,"
			" pressure,"
			" oil_temp) "
			"VALUES(%d, %d, %d, %d, %.3f, %.3f)"
			");",
			rf->timestamp.hour,
			rf->timestamp.minute,
			rf->timestamp.second,
			rf->timestamp.millisecond,
			rf->pressure,
			rf->oil_temp);

	return qdb_statement(this->_hdl, command);
}
