/**\file
 *
 * receive_messages.cpp
 *
 * This script is responsible for periodically searching for messages that have
 * been sent by the jbus. If any new messages are received, they are published
 * to the pub/sub server for use by other processes.
 *
 * Currently, this is only used to collect PDU-formatted messages provided by
 * the truck.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date July 19, 2018
 */

#include "jbus_receive.h"


