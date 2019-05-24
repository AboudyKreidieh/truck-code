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
	int fd = open("/pps/truck", O_WRONLY);										// FIXME: input
	virtual void publish(int type, void *message);
	virtual void subscribe(int type);
	virtual void ubsubscribe(int type);
	virtual std::vector <std::tuple<int, void*>> get_subscription_results();
    virtual ~PubSub();

private:
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
};


#endif /* SRC_UTILS_PUB_SUB_H_ */
