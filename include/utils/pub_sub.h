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
#include <fcntl.h>
#include "jbus/j1939_interpreters.h"


class PubSub
{
public:
	int fd = open("/pps/truck", O_WRONLY);										// FIXME: input
	virtual void publish(int type, void *message);
	virtual void subscribe(std::string type);
	virtual void ubsubscribe(std::string type);
	virtual void *get_subscription_results();
    virtual ~PubSub();

private:
    virtual void publish_pdu(j1939_pdu_typ *pdu);
    virtual void publish_tsc1(j1939_tsc1_typ *tsc1);
    virtual void publish_ebc1(j1939_ebc1_typ *ebc1);
    virtual void publish_ebc2(j1939_ebc2_typ *enc2);
    virtual void publish_eec1(j1939_eec1_typ *eec1);
    virtual void publish_eec2(j1939_eec2_typ *eec2);
    virtual void publish_eec3(j1939_eec3_typ *eec3);
    virtual void publish_erc1(j1939_erc1_typ *erc1);
    virtual void publish_etc1(j1939_etc1_typ *etc1);
    virtual void publish_etc2(j1939_etc2_typ *etc2);
    virtual void publish_turbo(j1939_turbo_typ *turbo);
    virtual void publish_vd(j1939_vd_typ *vd);
    virtual void publish_rcfg(j1939_rcfg_typ *rcfg);
    virtual void publish_ecfg(j1939_ecfg_typ *ecfg);
    virtual void publish_etemp(j1939_etemp_typ *etemp);
    virtual void publish_pto(j1939_pto_typ *pto);
    virtual void publish_ccvs(j1939_ccvs_typ *ccvs);
    virtual void publish_lfe(j1939_lfe_typ *lfe);
    virtual void publish_ambc(j1939_ambc_typ *ambc);
    virtual void publish_iec(j1939_iec_typ *iec);
    virtual void publish_vep(j1939_vep_typ *vep);
    virtual void publish_tf(j1939_tf_typ *tf);
    virtual void publish_rf(j1939_rf_typ *rf);
    virtual void publish_hrvd(j1939_hrvd_typ *hrvd);
    virtual void publish_fd(j1939_fd_typ *fd);
    virtual void publish_gfi2(j1939_gfi2_typ *gfi2);
    virtual void publish_ei(j1939_ei_typ *ei);
};


#endif /* SRC_UTILS_PUB_SUB_H_ */
