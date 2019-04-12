/**\file
 *
 * buffer.h
 *
 * This file contains utility methods to handle the circular queue of output
 * messages from the CAN card. The actual buffer open can be found in
 * can_struct.h.
 *
 *  Created on: Apr 9, 2019
 *      Author: aboudy
 */

#ifndef INCLUDE_CAN_BUFFER_H_
#define INCLUDE_CAN_BUFFER_H_

#include "can_struct.h"


/* -------------------------------------------------------------------------- */
/* ----------- Generic objects and method for the circular buffer ----------- */
/* -------------------------------------------------------------------------- */


/** Initialize the circular buffer.
 *
 * This method is used to set up buffer of items of a type defined by call
 * program. sizeof(the_type_name) is used for the item_size parameter. The most
 * recent max_data items will be saved.
 *
 * @param pbuff
 * 		pointer to the circular buffer that is to be initialized
 * @parma max_data
 * 		maximum number of elements that can be stored in the buffer
 * @param item_size
 * 		size of a single element in data_array
 */
extern void init_circular_buffer(cbuff_typ *pbuff, int max_data, int item_size);


/** Return the index of the most recent element to be added the buffer.
 *
 * This method also increments the value of data_start in the buffer. This is
 * used to get the index of insertion into the circular buffer's data array.
 *
 * @param pbuff
 * 		pointer to the circular buffer
 * @return
 *		index of the most recent element to be added the buffer.
 */
extern int get_circular_index(cbuff_typ *pbuff);


/* -------------------------------------------------------------------------- */
/* ---------- Buffer-related elements for the CAN Rx/Tx operations ---------- */
/* -------------------------------------------------------------------------- */


/** Remove all elements from the circular buffer.
 *
 * Used to throw out buffered input when a receiver first connects to the
 * driver.
 *
 * @param pattr
 * 		pointer to information per device manager. The in_buffer element is the
 * 		buffer that is being emptied.
 * @return
 * 		number of elements that were removed from the queue
 */
extern int can_dev_empty_q(IOFUNC_ATTR_T *pattr);


/** Add a new CAN message to the message buffer.
 *
 * @param pbuff
 * 		pointer to the CAN message buffer
 * @param new_mg
 * 		the new CAN message that should be added to the buffer
 */
extern void can_cq_add(cbuff_typ *pbuff, can_msg_t *new_msg);


/** Read the first element from the the message buffer.
 *
 * @param pbuff
 * 		pointer to the CAN message buffer
 * @return
 * 		the first message
 */
extern can_msg_t *can_cq_read_first(cbuff_typ *pbuff);


/** Pop the first element from the the message buffer.
 *
 * @param pbuff
 * 		pointer to the CAN message buffer
 * @return
 * 		the first message
 */
extern can_msg_t *can_cq_pop_first(cbuff_typ *pbuff);


#endif /* INCLUDE_CAN_BUFFER_H_ */
