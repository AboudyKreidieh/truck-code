/**\file
 *
 * subscribe.cpp
 *
 * Implements the subscription-related methods in pub_sub.h.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date May 24, 2019
 */
#include "pub_sub.h"
#include "jbus/j1939_interpreters.h"
#include <tuple>
#include <vector>
#include <algorithm>


#define MESSAGE_BUFF_SIZE 1024


vector<int> PubSub::get_subscribed_ids() {
	return this->_subscribed_ids;
}


void PubSub::subscribe(int type) {
	switch (type) {
	    case PDU   : this->_subscribe_pdu();
	                 break;
	    case TSC1  : this->_subscribe_tsc1();
	    			 break;
	    case ERC1  : this->_subscribe_erc1();
	    			 break;
	    case EBC1  : this->_subscribe_ebc1();
	    			 break;
	    case EBC2  : this->_subscribe_ebc2();
	    			 break;
	    case ETC1  : this->_subscribe_etc1();
	    			 break;
	    case ETC2  : this->_subscribe_etc2();
	    			 break;
	    case EEC1  : this->_subscribe_eec1();
	    			 break;
	    case EEC2  : this->_subscribe_eec2();
	    			 break;
	    case EEC3  : this->_subscribe_eec3();
	    			 break;
	    case GFI2  : this->_subscribe_gfi2();
	    			 break;
	    case EI    : this->_subscribe_ei();
	    			 break;
	    case FD    : this->_subscribe_fd();
	    	 		 break;
	    case HRVD  : this->_subscribe_hrvd();
	    		 	 break;
	    case TURBO : this->_subscribe_turbo();
	    			 break;
	    case VD    : this->_subscribe_vd();
	    	 		 break;
	    case RCFG  : this->_subscribe_rcfg();
	    		 	 break;
	    case ECFG  : this->_subscribe_ecfg();
	    		 	 break;
	    case ETEMP : this->_subscribe_etemp();
	    		 	 break;
	    case PTO   : this->_subscribe_pto();
	    		 	 break;
	    case CCVS  : this->_subscribe_ccvs();
	    		 	 break;
	    case LFE   : this->_subscribe_lfe();
	    		 	 break;
	    case AMBC  : this->_subscribe_ambc();
	    		 	 break;
	    case IEC   : this->_subscribe_iec();
	    		 	 break;
	    case VEP   : this->_subscribe_vep();
	    		 	 break;
	    case TF    : this->_subscribe_tf();
	    		 	 break;
	    case RF    : this->_subscribe_rf();
	    		 	 break;
	    default    : printf("Type %d is not valid.", type);
	    			 break;
	}
}

void PubSub::_subscribe_j1939_element(int pgn_num, string pgn_string) {
#ifdef DEBUG_FLAG
	printf("Subscribing %s messages.", pgn_string);
#endif

	/* Create a connection with the J1939 object. */
	char filepath[256];
	sprintf(filepath, "/pps/truck/%s?full", pgn_string.c_str());
	this->_fd_sub[pgn_num] = open(filepath, O_RDONLY);
	this->_subscribed_ids.push_back(pgn_num);
}

void PubSub::_subscribe_pdu() {
	this->_subscribe_j1939_element(PDU, "PDU");
}


void PubSub::_subscribe_tsc1() {
	this->_subscribe_j1939_element(TSC1, "TSC1");
}


void PubSub::_subscribe_ebc1() {
	this->_subscribe_j1939_element(EBC1, "EBC1");
}


void PubSub::_subscribe_ebc2() {
	this->_subscribe_j1939_element(EBC2, "EBC2");
}


void PubSub::_subscribe_eec1() {
	this->_subscribe_j1939_element(EEC1, "EEC1");
}


void PubSub::_subscribe_eec2() {
	this->_subscribe_j1939_element(EEC2, "EEC2");
}


void PubSub::_subscribe_eec3() {
	this->_subscribe_j1939_element(EEC3, "EEC3");
}


void PubSub::_subscribe_erc1() {
	this->_subscribe_j1939_element(ERC1, "ERC1");
}


void PubSub::_subscribe_etc1() {
	this->_subscribe_j1939_element(ETC1, "ETC1");
}


void PubSub::_subscribe_etc2() {
	this->_subscribe_j1939_element(ETC2, "ETC2");
}


void PubSub::_subscribe_turbo() {
	this->_subscribe_j1939_element(TURBO, "TURBO");
}


void PubSub::_subscribe_vd() {
	this->_subscribe_j1939_element(VD, "VD");
}


void PubSub::_subscribe_rcfg() {
	this->_subscribe_j1939_element(RCFG, "RCFG");
}


void PubSub::_subscribe_ecfg() {
	this->_subscribe_j1939_element(ECFG, "ECFG");
}


void PubSub::_subscribe_etemp() {
	this->_subscribe_j1939_element(ETEMP, "ETEMP");
}


void PubSub::_subscribe_pto() {
	this->_subscribe_j1939_element(PTO, "PTO");
}


void PubSub::_subscribe_ccvs() {
	this->_subscribe_j1939_element(CCVS, "CCVS");
}


void PubSub::_subscribe_lfe() {
	this->_subscribe_j1939_element(LFE, "LFE");
}


void PubSub::_subscribe_ambc() {
	this->_subscribe_j1939_element(AMBC, "AMBC");
}


void PubSub::_subscribe_iec() {
	this->_subscribe_j1939_element(IEC, "IEC");
}


void PubSub::_subscribe_vep() {
	this->_subscribe_j1939_element(VEP, "VEP");
}


void PubSub::_subscribe_tf() {
	this->_subscribe_j1939_element(TF, "TF");
}


void PubSub::_subscribe_rf() {
	this->_subscribe_j1939_element(RF, "RF");
}


void PubSub::_subscribe_hrvd() {
	this->_subscribe_j1939_element(HRVD, "HRVD");
}


void PubSub::_subscribe_fd() {
	this->_subscribe_j1939_element(FD, "FD");
}


void PubSub::_subscribe_gfi2() {
	this->_subscribe_j1939_element(GFI2, "GFI2");
}


void PubSub::_subscribe_ei() {
	this->_subscribe_j1939_element(EI, "EI");
}


void PubSub::unsubscribe(int type) {
	switch (type) {
	    case PDU   : this->_unsubscribe_pdu();
	                 break;
	    case TSC1  : this->_unsubscribe_tsc1();
	    			 break;
	    case ERC1  : this->_unsubscribe_erc1();
	    			 break;
	    case EBC1  : this->_unsubscribe_ebc1();
	    			 break;
	    case EBC2  : this->_unsubscribe_ebc2();
	    			 break;
	    case ETC1  : this->_unsubscribe_etc1();
	    			 break;
	    case ETC2  : this->_unsubscribe_etc2();
	    			 break;
	    case EEC1  : this->_unsubscribe_eec1();
	    			 break;
	    case EEC2  : this->_unsubscribe_eec2();
	    			 break;
	    case EEC3  : this->_unsubscribe_eec3();
	    			 break;
	    case GFI2  : this->_unsubscribe_gfi2();
	    			 break;
	    case EI    : this->_unsubscribe_ei();
	    			 break;
	    case FD    : this->_unsubscribe_fd();
	    	 		 break;
	    case HRVD  : this->_unsubscribe_hrvd();
	    		 	 break;
	    case TURBO : this->_unsubscribe_turbo();
	    			 break;
	    case VD    : this->_unsubscribe_vd();
	    	 		 break;
	    case RCFG  : this->_unsubscribe_rcfg();
	    		 	 break;
	    case ECFG  : this->_unsubscribe_ecfg();
	    		 	 break;
	    case ETEMP : this->_unsubscribe_etemp();
	    		 	 break;
	    case PTO   : this->_unsubscribe_pto();
	    		 	 break;
	    case CCVS  : this->_unsubscribe_ccvs();
	    		 	 break;
	    case LFE   : this->_unsubscribe_lfe();
	    		 	 break;
	    case AMBC  : this->_unsubscribe_ambc();
	    		 	 break;
	    case IEC   : this->_unsubscribe_iec();
	    		 	 break;
	    case VEP   : this->_unsubscribe_vep();
	    		 	 break;
	    case TF    : this->_unsubscribe_tf();
	    		 	 break;
	    case RF    : this->_unsubscribe_rf();
	    		 	 break;
	    default    : printf("Type %d is not valid.", type);
	    			 break;
	}
}

void PubSub::_unsubscribe_j1939_element(int pgn_num, string pgn_string) {
#ifdef DEBUG_FLAG
	printf("Unsubscribing %s messages.", pgn_string);
#endif

	/* Remove the connection from all lists, if applicable. */
	this->_fd_sub.erase(pgn_num);
	this->_subscribed_ids.erase(remove(this->_subscribed_ids.begin(),
			                           this->_subscribed_ids.end(),
									   pgn_num),
								this->_subscribed_ids.end());
}


void PubSub::_unsubscribe_pdu() {
	this->_unsubscribe_j1939_element(PDU, "PDU");
}


void PubSub::_unsubscribe_tsc1() {
	this->_unsubscribe_j1939_element(TSC1, "TSC1");
}


void PubSub::_unsubscribe_ebc1() {
	this->_unsubscribe_j1939_element(EBC1, "EBC1");
}


void PubSub::_unsubscribe_ebc2() {
	this->_unsubscribe_j1939_element(EBC2, "EBC2");
}


void PubSub::_unsubscribe_eec1() {
	this->_unsubscribe_j1939_element(EEC1, "EEC1");
}


void PubSub::_unsubscribe_eec2() {
	this->_unsubscribe_j1939_element(EEC2, "EEC2");
}


void PubSub::_unsubscribe_eec3() {
	this->_unsubscribe_j1939_element(EEC3, "EEC3");
}


void PubSub::_unsubscribe_erc1() {
	this->_unsubscribe_j1939_element(ERC1, "ERC1");
}


void PubSub::_unsubscribe_etc1() {
	this->_unsubscribe_j1939_element(ETC1, "ETC1");
}


void PubSub::_unsubscribe_etc2() {
	this->_unsubscribe_j1939_element(ETC2, "ETC2");
}


void PubSub::_unsubscribe_turbo() {
	this->_unsubscribe_j1939_element(TURBO, "TURBO");
}


void PubSub::_unsubscribe_vd() {
	this->_unsubscribe_j1939_element(VD, "VD");
}


void PubSub::_unsubscribe_rcfg() {
	this->_unsubscribe_j1939_element(RCFG, "RCFG");
}


void PubSub::_unsubscribe_ecfg() {
	this->_unsubscribe_j1939_element(ECFG, "ECFG");
}


void PubSub::_unsubscribe_etemp() {
	this->_unsubscribe_j1939_element(ETEMP, "ETEMP");
}


void PubSub::_unsubscribe_pto() {
	this->_unsubscribe_j1939_element(PTO, "PTO");
}


void PubSub::_unsubscribe_ccvs() {
	this->_unsubscribe_j1939_element(CCVS, "CCVS");
}


void PubSub::_unsubscribe_lfe() {
	this->_unsubscribe_j1939_element(LFE, "LFE");
}


void PubSub::_unsubscribe_ambc() {
	this->_unsubscribe_j1939_element(AMBC, "AMBC");
}


void PubSub::_unsubscribe_iec() {
	this->_unsubscribe_j1939_element(IEC, "IEC");
}


void PubSub::_unsubscribe_vep() {
	this->_unsubscribe_j1939_element(VEP, "VEP");
}


void PubSub::_unsubscribe_tf() {
	this->_unsubscribe_j1939_element(TF, "TF");
}


void PubSub::_unsubscribe_rf() {
	this->_unsubscribe_j1939_element(RF, "RF");
}


void PubSub::_unsubscribe_hrvd() {
	this->_unsubscribe_j1939_element(HRVD, "HRVD");
}


void PubSub::_unsubscribe_fd() {
	this->_unsubscribe_j1939_element(FD, "FD");
}


void PubSub::_unsubscribe_gfi2() {
	this->_unsubscribe_j1939_element(GFI2, "GFI2");
}


void PubSub::_unsubscribe_ei() {
	this->_unsubscribe_j1939_element(EI, "EI");
}


map <int, void*> PubSub::get_subscription_results() {
	map <int, void*> res;

	for (unsigned int i=0; i<this->_subscribed_ids.size(); i++) {
		switch (this->_subscribed_ids[i]) {
	    case PDU   : res[PDU] = this->_get_subscribed_pdu();
	                 break;
	    case TSC1  : res[TSC1] = this->_get_subscribed_tsc1();
	    			 break;
	    case ERC1  : res[ERC1] = this->_get_subscribed_erc1();
	    			 break;
	    case EBC1  : res[EBC1] = this->_get_subscribed_ebc1();
	    			 break;
	    case EBC2  : res[EBC2] = this->_get_subscribed_ebc2();
	    			 break;
	    case ETC1  : res[ETC1] = this->_get_subscribed_etc1();
	    			 break;
	    case ETC2  : res[ETC2] = this->_get_subscribed_etc2();
	    			 break;
	    case EEC1  : res[EEC1] = this->_get_subscribed_eec1();
	    			 break;
	    case EEC2  : res[EEC2] = this->_get_subscribed_eec2();
	    			 break;
	    case EEC3  : res[EEC3] = this->_get_subscribed_eec3();
	    			 break;
	    case GFI2  : res[GFI2] = this->_get_subscribed_gfi2();
	    			 break;
	    case EI    : res[EI] = this->_get_subscribed_ei();
	    			 break;
	    case FD    : res[FD] = this->_get_subscribed_fd();
	    	 		 break;
	    case HRVD  : res[HRVD] = this->_get_subscribed_hrvd();
	    		 	 break;
	    case TURBO : res[TURBO] = this->_get_subscribed_turbo();
	    			 break;
	    case VD    : res[VD] = this->_get_subscribed_vd();
	    	 		 break;
	    case RCFG  : res[RCFG] = this->_get_subscribed_rcfg();
	    		 	 break;
	    case ECFG  : res[ECFG] = this->_get_subscribed_ecfg();
	    		 	 break;
	    case ETEMP : res[ETEMP] = this->_get_subscribed_etemp();
	    		 	 break;
	    case PTO   : res[PTO] = this->_get_subscribed_pto();
	    		 	 break;
	    case CCVS  : res[CCVS] = this->_get_subscribed_ccvs();
	    		 	 break;
	    case LFE   : res[LFE] = this->_get_subscribed_lfe();
	    		 	 break;
	    case AMBC  : res[AMBC] = this->_get_subscribed_ambc();
	    		 	 break;
	    case IEC   : res[IEC] = this->_get_subscribed_iec();
	    		 	 break;
	    case VEP   : res[VEP] = this->_get_subscribed_vep();
	    		 	 break;
	    case TF    : res[TF] = this->_get_subscribed_tf();
	    		 	 break;
	    case RF    : res[RF] = this->_get_subscribed_rf();
	    		 	 break;
	    default    : printf("Type %d is not valid.", this->_subscribed_ids[i]);
	    			 break;
		}
	}

	return res;
}


void PubSub::_print_published_j1939(void* pdv, int pgn_num, string pgn_string) {
#ifdef DEBUG_FLAG
	printf("Grabbing published %s message: ", pgn_string);
	this->_interpreters[pgn_num]->print(pdv, stdout, true);
#endif
}


void* PubSub::_get_subscribed_pdu() {
	j1939_pdu_typ *pdu = new j1939_pdu_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[PDU], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &pdu->timestamp);
	pps_decoder_get_int(&decoder, "priority", &(pdu->priority));
	pps_decoder_get_int(&decoder, "pdu_format", &(pdu->pdu_format));
	pps_decoder_get_int(&decoder, "pdu_specific", &(pdu->pdu_specific));
	pps_decoder_get_int(&decoder, "src_address", &(pdu->src_address));
	pps_decoder_get_int(&decoder, "num_bytes", &(pdu->num_bytes));
	pps_decoder_push(&decoder, "data_field");
	for (int i=0; i<pdu->num_bytes; ++i)
		pps_decoder_get_int(&decoder, to_string(i).c_str(), &(pdu->data_field[i]));
	pps_decoder_pop(&decoder);
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)pdu, PDU, "PDU");

	return (void*)pdu;
}


void* PubSub::_get_subscribed_tsc1() {
	j1939_tsc1_typ *tsc1 = new j1939_tsc1_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[TSC1], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &tsc1->timestamp);
	pps_decoder_get_int(&decoder, "destination_address", &(tsc1->destination_address));
	pps_decoder_get_int(&decoder, "src_address", &(tsc1->src_address));
	pps_decoder_get_int(&decoder, "ovrd_ctrl_m_pr", &(tsc1->ovrd_ctrl_m_pr));
	pps_decoder_get_int(&decoder, "req_spd_ctrl", &(tsc1->req_spd_ctrl));
	pps_decoder_get_int(&decoder, "ovrd_ctrl_m", &(tsc1->ovrd_ctrl_m));
	pps_decoder_get_double(&decoder, "req_spd_lim", &(tsc1->req_spd_lim));
	pps_decoder_get_double(&decoder, "req_trq_lim", &(tsc1->req_trq_lim));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)tsc1, TSC1, "TSC1");

	return (void*)tsc1;
}


void* PubSub::_get_subscribed_ebc1() {
	j1939_ebc1_typ *ebc1 = new j1939_ebc1_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[EBC1], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &ebc1->timestamp);
	pps_decoder_get_int(&decoder, "ebs_brk_switch", &(ebc1->ebs_brk_switch));
	pps_decoder_get_int(&decoder, "antilock_brk_active", &(ebc1->antilock_brk_active));
	pps_decoder_get_int(&decoder, "asr_brk_ctrl_active", &(ebc1->asr_brk_ctrl_active));
	pps_decoder_get_int(&decoder, "asr_engine_ctrl_active", &(ebc1->asr_engine_ctrl_active));
	pps_decoder_get_double(&decoder, "brk_pedal_pos", &(ebc1->brk_pedal_pos));
	pps_decoder_get_int(&decoder, "trac_ctrl_override_switch", &(ebc1->trac_ctrl_override_switch));
	pps_decoder_get_int(&decoder, "asr_hillholder_switch", &(ebc1->asr_hillholder_switch));
	pps_decoder_get_int(&decoder, "abs_offroad_switch", &(ebc1->abs_offroad_switch));
	pps_decoder_get_int(&decoder, "asr_offroad_switch", &(ebc1->asr_offroad_switch));
	pps_decoder_get_int(&decoder, "accel_enable_switch", &(ebc1->accel_enable_switch));
	pps_decoder_get_int(&decoder, "aux_eng_shutdown_switch", &(ebc1->aux_eng_shutdown_switch));
	pps_decoder_get_int(&decoder, "eng_derate_switch", &(ebc1->eng_derate_switch));
	pps_decoder_get_int(&decoder, "accel_interlock_switch", &(ebc1->accel_interlock_switch));
	pps_decoder_get_double(&decoder, "eng_retarder_selection", &(ebc1->eng_retarder_selection));
	pps_decoder_get_int(&decoder, "abs_ebs_amber_warning", &(ebc1->abs_ebs_amber_warning));
	pps_decoder_get_int(&decoder, "ebs_red_warning", &(ebc1->ebs_red_warning));
	pps_decoder_get_int(&decoder, "abs_fully_operational", &(ebc1->abs_fully_operational));
	pps_decoder_get_int(&decoder, "src_address_ctrl", &(ebc1->src_address_ctrl));
	pps_decoder_get_double(&decoder, "total_brk_demand", &(ebc1->total_brk_demand));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)ebc1, EBC1, "EBC1");

	return (void*)ebc1;
}


void* PubSub::_get_subscribed_ebc2() {
	j1939_ebc2_typ *ebc2 = new j1939_ebc2_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[EBC2], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &ebc2->timestamp);
	pps_decoder_get_double(&decoder, "front_axle_spd", &(ebc2->front_axle_spd));
	pps_decoder_get_double(&decoder, "rel_spd_front_left", &(ebc2->rel_spd_front_left));
	pps_decoder_get_double(&decoder, "rel_spd_front_right", &(ebc2->rel_spd_front_right));
	pps_decoder_get_double(&decoder, "rel_spd_rear_left_1", &(ebc2->rel_spd_rear_left_1));
	pps_decoder_get_double(&decoder, "rel_spd_rear_right_1", &(ebc2->rel_spd_rear_right_1));
	pps_decoder_get_double(&decoder, "rel_spd_rear_left_2", &(ebc2->rel_spd_rear_left_2));
	pps_decoder_get_double(&decoder, "rel_spd_rear_right_2", &(ebc2->rel_spd_rear_right_2));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)ebc2, EBC2, "EBC2");

	return (void*)ebc2;
}


void* PubSub::_get_subscribed_eec1() {
	j1939_eec1_typ *eec1 = new j1939_eec1_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[EEC1], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &eec1->timestamp);
	pps_decoder_get_int(&decoder, "eng_trq_mode", &(eec1->eng_trq_mode));
	pps_decoder_get_double(&decoder, "drvr_demand_eng_trq", &(eec1->drvr_demand_eng_trq));
	pps_decoder_get_double(&decoder, "actual_eng_trq", &(eec1->actual_eng_trq));
	pps_decoder_get_double(&decoder, "eng_demand_trq", &(eec1->eng_demand_trq));
	pps_decoder_get_double(&decoder, "eng_spd", &(eec1->eng_spd));
	pps_decoder_get_int(&decoder, "src_address", &(eec1->src_address));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)eec1, EEC1, "EEC1");

	return (void*)eec1;
}


void* PubSub::_get_subscribed_eec2() {
	j1939_eec2_typ *eec2 = new j1939_eec2_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[EEC2], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &eec2->timestamp);
	pps_decoder_get_int(&decoder, "spd_limit_status", &(eec2->spd_limit_status));
	pps_decoder_get_int(&decoder, "accel_pedal_kickdown", &(eec2->accel_pedal_kickdown));
	pps_decoder_get_int(&decoder, "accel_pedal1_idle", &(eec2->accel_pedal1_idle));
	pps_decoder_get_int(&decoder, "accel_pedal2_idle", &(eec2->accel_pedal2_idle));
	pps_decoder_get_double(&decoder, "act_max_avail_eng_trq", &(eec2->act_max_avail_eng_trq));
	pps_decoder_get_double(&decoder, "accel_pedal1_pos", &(eec2->accel_pedal1_pos));
	pps_decoder_get_double(&decoder, "accel_pedal2_pos", &(eec2->accel_pedal2_pos));
	pps_decoder_get_double(&decoder, "eng_prcnt_load_curr_spd", &(eec2->eng_prcnt_load_curr_spd));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)eec2, EEC2, "EEC2");

	return (void*)eec2;
}


void* PubSub::_get_subscribed_eec3() {
	j1939_eec3_typ *eec3 = new j1939_eec3_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[EEC3], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &eec3->timestamp);
	pps_decoder_get_double(&decoder, "nominal_friction", &(eec3->nominal_friction));
	pps_decoder_get_double(&decoder, "est_eng_prstic_loss", &(eec3->est_eng_prstic_loss));
	pps_decoder_get_int(&decoder, "operating_spd_adjust", &(eec3->operating_spd_adjust));
	pps_decoder_get_double(&decoder, "desired_operating_spd", &(eec3->desired_operating_spd));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)eec3, EEC3, "EEC3");

	return (void*)eec3;
}


void* PubSub::_get_subscribed_erc1() {
	j1939_erc1_typ *erc1 = new j1939_erc1_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[ERC1], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	pps_decoder_get_int(&decoder, "enable_shift_assist", &(erc1->enable_shift_assist));
	pps_decoder_get_int(&decoder, "enable_brake_assist", &(erc1->enable_brake_assist));
	pps_decoder_get_int(&decoder, "trq_mode", &(erc1->trq_mode));
	pps_decoder_get_double(&decoder, "actual_ret_pcnt_trq", &(erc1->actual_ret_pcnt_trq));
	pps_decoder_get_double(&decoder, "intended_ret_pcnt_trq", &(erc1->intended_ret_pcnt_trq));
	pps_decoder_get_int(&decoder, "rq_brake_light", &(erc1->rq_brake_light));
	pps_decoder_get_int(&decoder, "src_address_ctrl", &(erc1->src_address_ctrl));
	pps_decoder_get_int(&decoder, "drvrs_demand_prcnt_trq", &(erc1->drvrs_demand_prcnt_trq));
	pps_decoder_get_double(&decoder, "selection_nonengine", &(erc1->selection_nonengine));
	pps_decoder_get_int(&decoder, "max_available_prcnt_trq", &(erc1->max_available_prcnt_trq));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)erc1, ERC1, "ERC1");

	return (void*)erc1;
}


void* PubSub::_get_subscribed_etc1() {
	j1939_etc1_typ *etc1 = new j1939_etc1_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[ETC1], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &etc1->timestamp);
	pps_decoder_get_int(&decoder, "trans_shift", &(etc1->trans_shift));
	pps_decoder_get_int(&decoder, "trq_conv_lockup", &(etc1->trq_conv_lockup));
	pps_decoder_get_int(&decoder, "trans_driveline", &(etc1->trans_driveline));
	pps_decoder_get_double(&decoder, "tran_output_shaft_spd", &(etc1->tran_output_shaft_spd));
	pps_decoder_get_double(&decoder, "prcnt_clutch_slip", &(etc1->prcnt_clutch_slip));
	pps_decoder_get_int(&decoder, "prog_shift_disable", &(etc1->prog_shift_disable));
	pps_decoder_get_int(&decoder, "eng_overspd_enable", &(etc1->eng_overspd_enable));
	pps_decoder_get_double(&decoder, "trans_input_shaft_spd", &(etc1->trans_input_shaft_spd));
	pps_decoder_get_int(&decoder, "src_address_ctrl", &(etc1->src_address_ctrl));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)etc1, ETC1, "ETC1");

	return (void*)etc1;
}


void* PubSub::_get_subscribed_etc2() {
	j1939_etc2_typ *etc2 = new j1939_etc2_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[ETC2], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &etc2->timestamp);
	pps_decoder_get_int(&decoder, "trans_selected_gear", &(etc2->trans_selected_gear));
	pps_decoder_get_double(&decoder, "trans_act_gear_ratio", &(etc2->trans_act_gear_ratio));
	pps_decoder_get_int(&decoder, "trans_current_gear", &(etc2->trans_current_gear));
	pps_decoder_get_int(&decoder, "range_selected", &(etc2->range_selected));
	pps_decoder_get_int(&decoder, "range_attained", &(etc2->range_attained));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)etc2, ETC2, "ETC2");

	return (void*)etc2;
}


void* PubSub::_get_subscribed_turbo() {
	j1939_turbo_typ *turbo = new j1939_turbo_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[TURBO], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &turbo->timestamp);
	pps_decoder_get_double(&decoder, "turbo_lube_oil_pressure", &(turbo->turbo_lube_oil_pressure));
	pps_decoder_get_double(&decoder, "turbo_speed", &(turbo->turbo_speed));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)turbo, TURBO, "TURBO");

	return (void*)turbo;
}


void* PubSub::_get_subscribed_vd() {
	j1939_vd_typ *vd = new j1939_vd_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[VD], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &vd->timestamp);
	pps_decoder_get_double(&decoder, "trip_dist", &(vd->trip_dist));
	pps_decoder_get_double(&decoder, "tot_vehicle_dist", &(vd->tot_vehicle_dist));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)vd, VD, "VD");

	return (void*)vd;
}


void* PubSub::_get_subscribed_rcfg() {
	j1939_rcfg_typ *rcfg = new j1939_rcfg_typ();
	pps_decoder_t decoder;
	int i;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[RCFG], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &rcfg->timestamp);
	pps_decoder_get_int(&decoder, "retarder_loc", &(rcfg->retarder_loc));
	pps_decoder_get_int(&decoder, "retarder_type", &(rcfg->retarder_type));
	pps_decoder_get_int(&decoder, "retarder_ctrl_steps", &(rcfg->retarder_ctrl_steps));
	pps_decoder_get_double(&decoder, "reference_retarder_trq", &(rcfg->reference_retarder_trq));
	pps_decoder_push(&decoder, "retarder_speed");
	for (i = 0; i < 5; i++)
		pps_decoder_get_double(&decoder, to_string(i).c_str(), &(rcfg->retarder_speed[i]));
	pps_decoder_pop(&decoder);
	pps_decoder_push(&decoder, "percent_torque");
	for (i = 0; i < 5; i++)
		pps_decoder_get_double(&decoder, to_string(i).c_str(), &(rcfg->percent_torque[i]));
	pps_decoder_pop(&decoder);
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)rcfg, RCFG, "RCFG");

	return (void*)rcfg;
}


void* PubSub::_get_subscribed_ecfg() {
	j1939_ecfg_typ *ecfg = new j1939_ecfg_typ();
	pps_decoder_t decoder;
	int i;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[ECFG], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &ecfg->timestamp);
	pps_decoder_get_int(&decoder, "receive_status", &(ecfg->receive_status));
	pps_decoder_push(&decoder, "engine_spd");
	for (i = 0; i < 7; i++)
		pps_decoder_get_double(&decoder, to_string(i).c_str(), &(ecfg->engine_spd[i]));
	pps_decoder_pop(&decoder);
	pps_decoder_push(&decoder, "percent_trq");
	for (i = 0; i < 5; i++)
		pps_decoder_get_double(&decoder, to_string(i).c_str(), &(ecfg->percent_trq[i]));
	pps_decoder_pop(&decoder);
	pps_decoder_get_double(&decoder, "gain_endspeed_governor", &(ecfg->gain_endspeed_governor));
	pps_decoder_get_double(&decoder, "reference_eng_trq", &(ecfg->reference_eng_trq));
	pps_decoder_get_double(&decoder, "max_momentary_overide_time", &(ecfg->max_momentary_overide_time));
	pps_decoder_get_double(&decoder, "spd_ctrl_lower_lim", &(ecfg->spd_ctrl_lower_lim));
	pps_decoder_get_double(&decoder, "spd_ctrl_upper_lim", &(ecfg->spd_ctrl_upper_lim));
	pps_decoder_get_double(&decoder, "trq_ctrl_lower_lim", &(ecfg->trq_ctrl_lower_lim));
	pps_decoder_get_double(&decoder, "trq_ctrl_upper_lim", &(ecfg->trq_ctrl_upper_lim));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)ecfg, ECFG, "ECFG");

	return (void*)ecfg;
}


void* PubSub::_get_subscribed_etemp() {
	j1939_etemp_typ *etemp = new j1939_etemp_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[ETEMP], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &etemp->timestamp);
	pps_decoder_get_double(&decoder, "eng_coolant_temp", &(etemp->eng_coolant_temp));
	pps_decoder_get_double(&decoder, "fuel_temp", &(etemp->fuel_temp));
	pps_decoder_get_double(&decoder, "eng_oil_temp", &(etemp->eng_oil_temp));
	pps_decoder_get_double(&decoder, "turbo_oil_temp", &(etemp->turbo_oil_temp));
	pps_decoder_get_double(&decoder, "eng_intercooler_temp", &(etemp->eng_intercooler_temp));
	pps_decoder_get_double(&decoder, "eng_intercooler_thermostat_opening", &(etemp->eng_intercooler_thermostat_opening));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)etemp, ETEMP, "ETEMP");

	return (void*)etemp;
}


void* PubSub::_get_subscribed_pto() {
	j1939_pto_typ *pto = new j1939_pto_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[PTO], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &pto->timestamp);
	pps_decoder_get_double(&decoder, "oil_temp", &(pto->oil_temp));
	pps_decoder_get_double(&decoder, "speed", &(pto->speed));
	pps_decoder_get_double(&decoder, "set_speed", &(pto->set_speed));
	pps_decoder_get_int(&decoder, "remote_variable_spd_status", &(pto->remote_variable_spd_status));
	pps_decoder_get_int(&decoder, "remote_preprogramm_status", &(pto->remote_preprogramm_status));
	pps_decoder_get_int(&decoder, "enable_switch", &(pto->enable_switch));
	pps_decoder_get_int(&decoder, "accel_switch", &(pto->accel_switch));
	pps_decoder_get_int(&decoder, "resume_switch", &(pto->resume_switch));
	pps_decoder_get_int(&decoder, "coast_decel_switch", &(pto->coast_decel_switch));
	pps_decoder_get_int(&decoder, "set_switch", &(pto->set_switch));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)pto, PTO, "PTO");

	return (void*)pto;
}


void* PubSub::_get_subscribed_ccvs() {
	j1939_ccvs_typ *ccvs = new j1939_ccvs_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[CCVS], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &(ccvs->timestamp));
	pps_decoder_get_int(&decoder, "park_brk_release", &(ccvs->park_brk_release));
	pps_decoder_get_int(&decoder, "parking_brk_switch", &(ccvs->parking_brk_switch));
	pps_decoder_get_int(&decoder, "two_spd_axle_switch", &(ccvs->two_spd_axle_switch));
	pps_decoder_get_double(&decoder, "vehicle_spd", &(ccvs->vehicle_spd));
	pps_decoder_get_int(&decoder, "clutch_switch", &(ccvs->clutch_switch));
	pps_decoder_get_int(&decoder, "brk_switch", &(ccvs->brk_switch));
	pps_decoder_get_int(&decoder, "cc_pause_switch", &(ccvs->cc_pause_switch));
	pps_decoder_get_int(&decoder, "cc_enable_switch", &(ccvs->cc_enable_switch));
	pps_decoder_get_int(&decoder, "cc_active", &(ccvs->cc_active));
	pps_decoder_get_int(&decoder, "cc_accel_switch", &(ccvs->cc_accel_switch));
	pps_decoder_get_int(&decoder, "cc_resume_switch", &(ccvs->cc_resume_switch));
	pps_decoder_get_int(&decoder, "cc_coast_switch", &(ccvs->cc_coast_switch));
	pps_decoder_get_int(&decoder, "cc_set_switch", &(ccvs->cc_set_switch));
	pps_decoder_get_double(&decoder, "cc_set_speed", &(ccvs->cc_set_speed));
	pps_decoder_get_int(&decoder, "cc_state", &(ccvs->cc_state));
	pps_decoder_get_int(&decoder, "pto_state", &(ccvs->pto_state));
	pps_decoder_get_int(&decoder, "eng_shutdown_override", &(ccvs->eng_shutdown_override));
	pps_decoder_get_int(&decoder, "eng_test_mode_switch", &(ccvs->eng_test_mode_switch));
	pps_decoder_get_int(&decoder, "eng_idle_decr_switch", &(ccvs->eng_idle_decr_switch));
	pps_decoder_get_int(&decoder, "eng_idle_incr_switch", &(ccvs->eng_idle_incr_switch));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)ccvs, CCVS, "CCVS");

	return (void*)ccvs;
}


void* PubSub::_get_subscribed_lfe() {
	j1939_lfe_typ *lfe = new j1939_lfe_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[LFE], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &lfe->timestamp);
	pps_decoder_get_double(&decoder, "eng_fuel_rate", &(lfe->eng_fuel_rate));
	pps_decoder_get_double(&decoder, "eng_inst_fuel_economy", &(lfe->eng_inst_fuel_economy));
	pps_decoder_get_double(&decoder, "eng_avg_fuel_economy", &(lfe->eng_avg_fuel_economy));
	pps_decoder_get_double(&decoder, "eng_throttle1_pos", &(lfe->eng_throttle1_pos));
	pps_decoder_get_double(&decoder, "eng_throttle2_pos", &(lfe->eng_throttle2_pos));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)lfe, LFE, "LFE");

	return (void*)lfe;
}


void* PubSub::_get_subscribed_ambc() {
	j1939_ambc_typ *ambc = new j1939_ambc_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[AMBC], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &ambc->timestamp);
	pps_decoder_get_double(&decoder, "barometric_pressure", &(ambc->barometric_pressure));
	pps_decoder_get_double(&decoder, "cab_interior_temp", &(ambc->cab_interior_temp));
	pps_decoder_get_double(&decoder, "ambient_air_temp", &(ambc->ambient_air_temp));
	pps_decoder_get_double(&decoder, "air_inlet_temp", &(ambc->air_inlet_temp));
	pps_decoder_get_double(&decoder, "road_surface_temp", &(ambc->road_surface_temp));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)ambc, AMBC, "AMBC");

	return (void*)ambc;
}


void* PubSub::_get_subscribed_iec() {
	j1939_iec_typ *iec = new j1939_iec_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[IEC], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &iec->timestamp);
	pps_decoder_get_double(&decoder, "particulate_inlet_pressure", &(iec->particulate_inlet_pressure));
	pps_decoder_get_double(&decoder, "boost_pressure", &(iec->boost_pressure));
	pps_decoder_get_double(&decoder, "intake_manifold_temp", &(iec->intake_manifold_temp));
	pps_decoder_get_double(&decoder, "air_inlet_pressure", &(iec->air_inlet_pressure));
	pps_decoder_get_double(&decoder, "air_filter_diff_pressure", &(iec->air_filter_diff_pressure));
	pps_decoder_get_double(&decoder, "exhaust_gas_temp", &(iec->exhaust_gas_temp));
	pps_decoder_get_double(&decoder, "coolant_filter_diff_pressure", &(iec->coolant_filter_diff_pressure));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)iec, IEC, "IEC");

	return (void*)iec;
}


void* PubSub::_get_subscribed_vep() {
	j1939_vep_typ *vep = new j1939_vep_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[VEP], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &vep->timestamp);
	pps_decoder_get_double(&decoder, "net_battery_current", &(vep->net_battery_current));
	pps_decoder_get_double(&decoder, "alternator_current", &(vep->alternator_current));
	pps_decoder_get_double(&decoder, "alternator_potential", &(vep->alternator_potential));
	pps_decoder_get_double(&decoder, "electrical_potential", &(vep->electrical_potential));
	pps_decoder_get_double(&decoder, "battery_potential", &(vep->battery_potential));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)vep, VEP, "VEP");

	return (void*)vep;
}


void* PubSub::_get_subscribed_tf() {
	j1939_tf_typ *tf = new j1939_tf_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[TF], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &tf->timestamp);
	pps_decoder_get_double(&decoder, "clutch_pressure", &(tf->clutch_pressure));
	pps_decoder_get_double(&decoder, "oil_level", &(tf->oil_level));
	pps_decoder_get_double(&decoder, "diff_pressure", &(tf->diff_pressure));
	pps_decoder_get_double(&decoder, "oil_pressure", &(tf->oil_pressure));
	pps_decoder_get_double(&decoder, "oil_temp", &(tf->oil_temp));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)tf, TF, "TF");

	return (void*)tf;
}


void* PubSub::_get_subscribed_rf() {
	j1939_rf_typ *rf = new j1939_rf_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[RF], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &rf->timestamp);
	pps_decoder_get_double(&decoder, "pressure", &(rf->pressure));
	pps_decoder_get_double(&decoder, "oil_temp", &(rf->oil_temp));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)rf, RF, "RF");

	return (void*)rf;
}


void* PubSub::_get_subscribed_hrvd() {
	j1939_hrvd_typ *hrvd = new j1939_hrvd_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[HRVD], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &hrvd->timestamp);
	pps_decoder_get_double(&decoder, "vehicle_distance", &(hrvd->vehicle_distance));
	pps_decoder_get_double(&decoder, "trip_distance", &(hrvd->trip_distance));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)hrvd, HRVD, "HRVD");

	return (void*)hrvd;
}


void* PubSub::_get_subscribed_fd() {
	j1939_fd_typ *fd = new j1939_fd_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[FD], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &fd->timestamp);
	pps_decoder_get_double(&decoder, "prcnt_fan_spd", &(fd->prcnt_fan_spd));
	pps_decoder_get_int(&decoder, "fan_drive_state", &(fd->fan_drive_state));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)fd, FD, "FD");

	return (void*)fd;
}


void* PubSub::_get_subscribed_gfi2() {
	j1939_gfi2_typ *gfi2 = new j1939_gfi2_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[GFI2], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &gfi2->timestamp);
	pps_decoder_get_double(&decoder, "fuel_flow_rate1", &(gfi2->fuel_flow_rate1));
	pps_decoder_get_double(&decoder, "fuel_flow_rate2", &(gfi2->fuel_flow_rate2));
	pps_decoder_get_double(&decoder, "fuel_valve_pos1", &(gfi2->fuel_valve_pos1));
	pps_decoder_get_double(&decoder, "fuel_valve_pos2", &(gfi2->fuel_valve_pos2));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)gfi2, GFI2, "GFI2");

	return (void*)gfi2;
}


void* PubSub::_get_subscribed_ei() {
	j1939_ei_typ *ei = new j1939_ei_typ();
	pps_decoder_t decoder;

	char buffer[MESSAGE_BUFF_SIZE];
	read(this->_fd_sub[EI], buffer, sizeof(buffer));

	pps_decoder_initialize(&decoder, NULL);
	pps_decoder_parse_pps_str(&decoder, buffer);
	pps_decoder_push(&decoder, NULL);
	decode_timestamp(decoder, &ei->timestamp);
	pps_decoder_get_double(&decoder, "pre_filter_oil_pressure", &(ei->pre_filter_oil_pressure));
	pps_decoder_get_double(&decoder, "exhaust_gas_pressure", &(ei->exhaust_gas_pressure));
	pps_decoder_get_double(&decoder, "rack_position", &(ei->rack_position));
	pps_decoder_get_double(&decoder, "eng_gas_mass_flow", &(ei->eng_gas_mass_flow));
	pps_decoder_get_double(&decoder, "inst_estimated_brake_power", &(ei->inst_estimated_brake_power));
	pps_decoder_pop(&decoder);
	pps_decoder_cleanup(&decoder);

	/* If in debug mode, print the collected method. */
	this->_print_published_j1939((void*)ei, EI, "EI");

	return (void*)ei;
}
