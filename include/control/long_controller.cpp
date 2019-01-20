///**\file
// *
// * long_controller.cpp
// *
// * @author Abdul Rahman Kreidieh
// * @version 1.0.0
// * @date July 11, 2018
// */
//
//#include "long_controller.h"
//#include "long_utils.h"
//#include "jbus/j1939_struct.h"
//
//
///** Initializer */
//LongController::LongController() {
//	state = new long_vehicle_state();
//}
//
//
///** Updates the state vehicle state object.
// *
// * Information is collected from PPS objects whenever they are updated and
// * placed directly in the referenced object.
// */
//void LongController::update_vehicle_state() {
//
//}
//
//
///** Sends a brake command (blank)
// *
// * (blank).
// */
//void LongController::send_cmd_torque(float torque, bool verbose) {
//	// prepare a TSC1 message with current vehicle data
//	j1939_tsc1_typ *tsc1 = new j1939_tsc1_typ();
//	tsc1->timestamp = 0;														// TODO(ak): method to get current time
//	tsc1->destination_address = 0;  // destination matches (blank)
//
//	tsc1->ovrd_ctrl_m = 0;
//	tsc1->ovrd_ctrl_m_pr = 0;
//	tsc1->req_spd_ctrl = 0;
//	tsc1->req_spd_lim = 0;
//	tsc1->req_trq_lim = 0;
//	tsc1->src_address = 0;
//
//	// add the torque command to the message
//
//	// send the command to (blank)
//
//	// print that the command was sent, if requested
//	if (verbose)
//		printf("Torque command of %.3f (blank) sent.", torque);
//}
//
//
///** Sends a torque command (blank)
// *
// * (blank).
// */
//void LongController::send_cmd_brake(float brake, bool verbose) {
//	// prepare a (blank) message
//
//	// add the brake command to the message
//
//	// send the command to (blank)
//
//	// print that the command was sent, if requested
//	if (verbose)
//		printf("Brake command of %.3f (blank) sent.", brake);
//}
