/**\file
 *
 * buffer.h
 *
 * This file contains utility methods to handle the circular queue of output
 * messages from the CAN card. The actual buffer open can be found in
 * can_struct.h.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date April 9, 2019
 */

#ifndef INCLUDE_UTILS_BUFFER_H_
#define INCLUDE_UTILS_BUFFER_H_


/** Circular buffer type used with data item saving.
 *
 * Data from last "data_size" samples can be saved in this structure.
 */
class CircularBuffer {
public:
	/** Initialize the circular buffer.
	 *
	 * This method is used to set up buffer of items of a type defined by call
	 * program. sizeof(the_type_name) is used for the item_size parameter. The
	 * most recent max_data items will be saved.
	 *
	 * @param max_data
	 * 		maximum number of elements that can be stored in the buffer
	 * @param item_size
	 * 		size of a single element in data_array
	 */
	void init(int max_data, int item_size);

	/** Return the index of the most recent element to be added the buffer.
	 *
	 * This method also increments the value of data_start in the buffer. This
	 * is used to get the index of insertion into the circular buffer's data
	 * array.
	 *
	 * @return
	 *		index of the most recent element to be added the buffer.
	 */
	int get_circular_index();

	/** Remove all elements from the circular buffer.
	 *
	 * Used to throw out buffered input when a receiver first connects to the
	 * driver.
	 *
	 * @return
	 * 		number of elements that were removed from the queue
	 */
	int empty();

	/** Add a new message to the message buffer.
	 *
	 * @param new_mg
	 * 		the new message that should be added to the buffer
	 */
	void add(void *new_msg);

	/** Read the first element from the the message buffer.
	 *
	 * @return
	 * 		the first message
	 */
	void *read_first();

	/** Pop the first element from the the message buffer.
	 *
	 * @return
	 * 		the first message
	 */
	void *pop_first();

	/** Return the number of elements in the buffer. */
	int get_count();

private:
	void *_data_array;  /**< data elements */
	int _size;  		/**< number of structures malloced */
	int _count; 		/**< control intervals saved (<=data_size) */
	int _start; 		/**< index of oldest item in the circular buffer */
};


#endif /* INCLUDE_UTILS_BUFFER_H_ */
