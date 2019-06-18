/**\file
 *
 * pub_sub.h
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date July 16, 2019
 */

#ifndef SRC_UTILS_PUB_SUB_H_
#define SRC_UTILS_PUB_SUB_H_

#include <string>
#include <vector>
#include <tuple>
#include <fcntl.h>
#include "jbus/j1939_interpreters.h"


#define PS_EMTPY -1001


class PubSub
{
public:
	/**Publish a message of a specific type to the PPS server.
	 *
	 * @param type
	 * 		the message type
	 * @param message
	 * 		the contents of the message
	 */
	virtual void publish(int type, void *message);

	/**Subscribe to a message.
	 *
	 * These messages can then be collected from `get_subscription_results`.
	 *
	 * @param type
	 * 		the message type
	 */
	virtual void subscribe(int type);

	/**Unsubscribe from a message.
	 *
	 * @param type
	 * 		the message type
	 */
	virtual void unsubscribe(int type);

	/**Collect the variables of all subscribed message.
	 *
	 * @return
	 * 		a map whose keys are the message type and whose values are the
	 * 		message that was collected from the PPS server
	 */
	virtual std::map <int, void*> get_subscription_results();

	/**Return the IDs of all subscribed messages. */
	virtual vector<int> get_subscribed_ids();

	virtual ~PubSub();

private:
	int _fd_pub = open("/pps/truck", O_WRONLY);
	map <int, int> _fd_sub;
	vector <int> _subscribed_ids;
#if DEBUG_FLAG
	map <int, J1939Interpreter*> _interpreters = get_interpreters();
#endif

	/**Subscribe any J1939 message.
	 *
	 * @param pgn_num
	 * 		The J1939 message's parameter group number.
	 * @param pgn_string
	 * 		The name of the J1939 message as it is stored in the PPS system
	 * 		(e.g. "TSC1").
	 */
	virtual void _subscribe_j1939_element(int pgn_num, string pgn_string);

	/**Unsubscribe any J1939 message.
	 *
	 * @param pgn_num
	 * 		The J1939 message's parameter group number.
	 * @param pgn_string
	 * 		The name of the J1939 message as it is stored in the PPS system
	 * 		(e.g. "TSC1").
	 */
	virtual void _unsubscribe_j1939_element(int pgn_num, string pgn_string);

	/**Print the content of a published J1939 message that was collected.
	 *
	 * This is only done in debug mode.
	 *
	 * @param pdv
	 * 		the J1939 message
	 * @param pgn_num
	 * 		The J1939 message's parameter group number.
	 * @param pgn_string
	 * 		The name of the J1939 message as it is stored in the PPS system
	 * 		(e.g. "TSC1").
	 */
	virtual void _print_published_j1939(void* pdv, int pgn_num, string pgn_string);

	virtual void _publish_pdu(void *data);
    virtual void _publish_tsc1(void *data);
    virtual void _publish_ebc1(void *data);
    virtual void _publish_ebc2(void *data);
    virtual void _publish_eec1(void *data);
    virtual void _publish_eec2(void *data);
    virtual void _publish_eec3(void *data);
    virtual void _publish_erc1(void *data);
    virtual void _publish_etc1(void *data);
    virtual void _publish_etc2(void *data);
    virtual void _publish_turbo(void *data);
    virtual void _publish_vd(void *data);
    virtual void _publish_rcfg(void *data);
    virtual void _publish_ecfg(void *data);
    virtual void _publish_etemp(void *data);
    virtual void _publish_pto(void *data);
    virtual void _publish_ccvs(void *data);
    virtual void _publish_lfe(void *data);
    virtual void _publish_ambc(void *data);
    virtual void _publish_iec(void *data);
    virtual void _publish_vep(void *data);
    virtual void _publish_tf(void *data);
    virtual void _publish_rf(void *data);
    virtual void _publish_hrvd(void *data);
    virtual void _publish_fd(void *data);
    virtual void _publish_gfi2(void *data);
    virtual void _publish_ei(void *data);

    virtual void _subscribe_pdu();
    virtual void _subscribe_tsc1();
    virtual void _subscribe_ebc1();
    virtual void _subscribe_ebc2();
    virtual void _subscribe_eec1();
    virtual void _subscribe_eec2();
    virtual void _subscribe_eec3();
    virtual void _subscribe_erc1();
    virtual void _subscribe_etc1();
    virtual void _subscribe_etc2();
    virtual void _subscribe_turbo();
    virtual void _subscribe_vd();
    virtual void _subscribe_rcfg();
    virtual void _subscribe_ecfg();
    virtual void _subscribe_etemp();
    virtual void _subscribe_pto();
    virtual void _subscribe_ccvs();
    virtual void _subscribe_lfe();
    virtual void _subscribe_ambc();
    virtual void _subscribe_iec();
    virtual void _subscribe_vep();
    virtual void _subscribe_tf();
    virtual void _subscribe_rf();
    virtual void _subscribe_hrvd();
    virtual void _subscribe_fd();
    virtual void _subscribe_gfi2();
    virtual void _subscribe_ei();

    virtual void _unsubscribe_pdu();
    virtual void _unsubscribe_tsc1();
    virtual void _unsubscribe_ebc1();
    virtual void _unsubscribe_ebc2();
    virtual void _unsubscribe_eec1();
    virtual void _unsubscribe_eec2();
    virtual void _unsubscribe_eec3();
    virtual void _unsubscribe_erc1();
    virtual void _unsubscribe_etc1();
    virtual void _unsubscribe_etc2();
    virtual void _unsubscribe_turbo();
    virtual void _unsubscribe_vd();
    virtual void _unsubscribe_rcfg();
    virtual void _unsubscribe_ecfg();
    virtual void _unsubscribe_etemp();
    virtual void _unsubscribe_pto();
    virtual void _unsubscribe_ccvs();
    virtual void _unsubscribe_lfe();
    virtual void _unsubscribe_ambc();
    virtual void _unsubscribe_iec();
    virtual void _unsubscribe_vep();
    virtual void _unsubscribe_tf();
    virtual void _unsubscribe_rf();
    virtual void _unsubscribe_hrvd();
    virtual void _unsubscribe_fd();
    virtual void _unsubscribe_gfi2();
    virtual void _unsubscribe_ei();

    virtual void* _get_subscribed_pdu();
    virtual void* _get_subscribed_tsc1();
    virtual void* _get_subscribed_ebc1();
    virtual void* _get_subscribed_ebc2();
    virtual void* _get_subscribed_eec1();
    virtual void* _get_subscribed_eec2();
    virtual void* _get_subscribed_eec3();
    virtual void* _get_subscribed_erc1();
    virtual void* _get_subscribed_etc1();
    virtual void* _get_subscribed_etc2();
    virtual void* _get_subscribed_turbo();
    virtual void* _get_subscribed_vd();
    virtual void* _get_subscribed_rcfg();
    virtual void* _get_subscribed_ecfg();
    virtual void* _get_subscribed_etemp();
    virtual void* _get_subscribed_pto();
    virtual void* _get_subscribed_ccvs();
    virtual void* _get_subscribed_lfe();
    virtual void* _get_subscribed_ambc();
    virtual void* _get_subscribed_iec();
    virtual void* _get_subscribed_vep();
    virtual void* _get_subscribed_tf();
    virtual void* _get_subscribed_rf();
    virtual void* _get_subscribed_hrvd();
    virtual void* _get_subscribed_fd();
    virtual void* _get_subscribed_gfi2();
    virtual void* _get_subscribed_ei();
};


#endif /* SRC_UTILS_PUB_SUB_H_ */
