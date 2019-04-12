/**\file
 *
 * buffer.cpp
 *
 *  Created on: Apr 9, 2019
 *      Author: aboudy
 */

#include "buffer.h"
#include <stdlib.h>


void init_circular_buffer(cbuff_typ *pbuff, int max_data, int item_size) {
	pbuff->data_size = max_data;
    pbuff->data_count = 0;
    pbuff->data_start = 0;

    if (max_data > 0)
	    pbuff->data_array = (void*) malloc (item_size *pbuff->data_size);
    else
		pbuff->data_array = NULL;
}


int get_circular_index(cbuff_typ *pbuff) {
	/* index of the most recent element */
	int index;

	if (pbuff->data_size == pbuff->data_count) {
		/* if count at max, insert in previous start location */
		index = pbuff->data_start;
		pbuff->data_start++;
		if (pbuff->data_start == pbuff->data_size)
			pbuff->data_start = 0;
	} else {
		/* insert at next free location, increment count */
		index = pbuff->data_start + pbuff->data_count;
		if (index >= pbuff->data_size)
			index -= pbuff->data_size;
		pbuff->data_count++;
	}

	return index;
}


/* -------------------------------------------------------------------------- */
/* ---------- Buffer-related elements for the CAN Rx/Tx operations ---------- */
/* -------------------------------------------------------------------------- */


void can_cq_add(cbuff_typ *pbuff, can_msg_t *new_msg) {
	int index = get_circular_index(pbuff);
	can_msg_t *pdata = (can_msg_t *) pbuff->data_array;
	pdata[index] = *new_msg;
}


can_msg_t *can_cq_read_first(cbuff_typ *pbuff) {
	can_msg_t *pdata = (can_msg_t *) pbuff->data_array;
	return (&pdata[pbuff->data_start]);
}


can_msg_t *can_cq_pop_first(cbuff_typ *pbuff) {
	can_msg_t *pdata = (can_msg_t *) pbuff->data_array;

	if (pbuff->data_count > 0) {
		can_msg_t *rtnval = &pdata[pbuff->data_start];
		pbuff->data_count--;
		pbuff->data_start++;
		if (pbuff->data_start == pbuff->data_size)
			pbuff->data_start = 0;
		return rtnval;
	}
	return NULL;
}


int can_dev_empty_q(IOFUNC_ATTR_T *pattr) {
	int j = 0;
	can_msg_t *dummy = can_cq_pop_first(&pattr->in_buff);
	while (dummy != NULL) {
		j++;
		dummy = can_cq_pop_first(&pattr->in_buff);
	}
	return j;
}
