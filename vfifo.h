#ifndef _VFIFO_
#define _VFIFO_

#include <stdint.h>
#include <stdlib.h>

typedef struct vfifo {
	uint8_t *buf;
	size_t buf_len;
	size_t f_bytes;
	uint16_t head;
	uint16_t tail;
	uint16_t no_of_rec;
} vfifo_t;

/**
 * vfifo_init - Initializes circular FIFO capable of storing variable
 *              sized records
 * @fifo: vfifo object to be initialized
 * @buf: memory array to store records
 * @buf_len: length of @buf
 *
 * Initializes FIFO to store variable size of record. Should be
 * invoked before any other function in this module.
 */
void vfifo_init(vfifo_t *fifo, char *buf, size_t buf_len);

/**
 * vfifo_add - Add new record to FIFO
 * @fifo: pointer to the FIFO object
 * @rec: new record
 * @rec_len: length of @rec
 *
 * Add new record to FIFO. Record will be overwrite, if no spaces to
 * store new record.
 */
void vfifo_add(vfifo_t *fifo, char *rec, size_t rec_len);

/**
 * vfifo_remove - Get first record of the FIFO object
 * @fifo: pointer to the FIFO object
 * @buf: array to store records
 * @buf_len: length of @buf
 *
 * Removes a record from the FIFO, and stores into the provided
 * buffer. If the buffer size is lesser than the record size then the
 * record is truncated. Returns 0 on success, -1, if no data to remove
 * and -2, if data is truncated.
 */
int vfifo_remove(vfifo_t *fifo, char *buf, size_t buf_len);

/**
 * vfifo_len - Returns the no. of records queued in the FIFO
 * @fifo: pointer to FIFO object
 */
int vfifo_len(vfifo_t *fifo);

#endif
