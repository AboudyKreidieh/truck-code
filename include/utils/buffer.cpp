/**\file
 *
 * buffer.cpp
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date April 9, 2019
 */

#include <cstdlib>
#include "buffer.h"


void CircularBuffer::init(int max_data, int item_size) {
	this->_size = max_data;
    this->_count = 0;
    this->_start = 0;

    if (max_data > 0)
	    this->_data_array = (void*) malloc (item_size * this->_size);
    else
		this->_data_array = NULL;
}


int CircularBuffer::get_circular_index() {
	/* index of the most recent element */
	int index;

	if (this->_size == this->_count) {
		/* if count at max, insert in previous start location */
		index = this->_start;
		this->_start++;
		if (this->_start == this->_size)
			this->_start = 0;
	} else {
		/* insert at next free location, increment count */
		index = this->_start + this->_count;
		if (index >= this->_size)
			index -= this->_size;
		this->_count++;
	}

	return index;
}


void CircularBuffer::add(void *new_msg) {
	int index = this->get_circular_index();
	void *pdata = this->_data_array;
	(&pdata)[index] = new_msg;
}


void *CircularBuffer::read_first() {
	void *pdata = this->_data_array;
	return (&pdata)[this->_start];
}


void *CircularBuffer::pop_first() {
	void *pdata = this->_data_array;

	if (this->_count > 0) {
		void *rtnval = (&pdata)[this->_start];
		this->_count--;
		this->_start++;
		if (this->_start == this->_size)
			this->_start = 0;
		return rtnval;
	}
	else
		return NULL;
}


int CircularBuffer::empty() {
	int j = 0;
	while (this->pop_first() != NULL)
		j++;
	return j;
}


int CircularBuffer::get_count() {
	return this->_count;
}
