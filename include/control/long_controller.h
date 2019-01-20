///**\file
// *
// * long_controller.h
// *
// * This file contains the controller class used for longitudonal control. This
// * class includes methods for:
// *  - storing updated vehicle state information
// *  - issuing commands for both the torque and the break
// *
// * @author Abdul Rahman Kreidieh
// * @version 1.0.0
// * @date July 11, 2018
// */
//
//#ifndef SRC_CONTROL_LONG_CONTROLLER_H_
//#define SRC_CONTROL_LONG_CONTROLLER_H_
//
//#include "long_utils.h"
//
//
//class LongController
//{
//public:
//	long_vehicle_state *state;
//
//	/** Update the state vehicle state object.
//	 *
//	 * Information is collected from PPS objects whenever they are updated and
//	 * placed directly in the referenced object.
//	 */
//	void update_vehicle_state();
//
//	/** Apply an action to the automated truck.
//	 *
//	 * This is the primary method responsible for performing the control
//	 * process, and the component that should be modified when trying to test
//	 * new control algorithms. This method has access to the vehicle state
//	 * information (stored under state), and can send torque and break commands
//	 * using the auxiliary methods within the class.
//	 */
//	void apply_action();
//
//	/** Sends a torque command (blank)
//	 *
//	 * (blank).
//	 */
//	void send_cmd_torque(float, bool);
//
//	/** Sends a brake command (blank).
//	 *
//	 * (blank).
//	 */
//	void send_cmd_brake(float, bool);
//};
//
//
//
//#endif /* SRC_CONTROL_LONG_CONTROLLER_H_ */
