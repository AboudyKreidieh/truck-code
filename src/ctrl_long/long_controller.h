/**
 * long_controller.h
 *
 *  Created on: Jul 11, 2018
 *      Author: Abdul Rahman Kreidieh
 */

#ifndef SRC_CTRL_LONG_LONG_CONTROLLER_H_
#define SRC_CTRL_LONG_LONG_CONTROLLER_H_

#include "long_utils.h"


class LongController
{
public:
	long_vehicle_state *state;

	/** Updates the state vehicle state object.
	 *
	 * Information is collected from PPS objects whenever they are updated and
	 * placed directly in the referenced object.
	 */
	void update_vehicle_state();

	/** Sends a torque command (blank)
	 *
	 * (blank).
	 */
	void send_cmd_torque(float, bool);

	/** Sends a brake command (blank)
	 *
	 * (blank).
	 */
	void send_cmd_brake(float, bool);

};



#endif /* SRC_CTRL_LONG_LONG_CONTROLLER_H_ */
