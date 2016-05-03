#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "vfifo.h"

void vfifo_init(vfifo_t *fifo, char *buf, size_t buf_len)
{
	fifo->buf = buf;
	fifo->buf_len = buf_len;
	fifo->no_of_rec = 0;
	fifo->f_bytes = buf_len;
	fifo->head = 0;
	fifo->tail = 0;
}

void vfifo_add(vfifo_t *fifo, char *rec, size_t rec_len)
{
	size_t len1;
	size_t len2;
	uint8_t tmp[1];

	while (fifo->f_bytes < (rec_len + 1))
                vfifo_remove(fifo, tmp, sizeof(tmp));

	if ((fifo->tail + rec_len) > fifo->buf_len) {
		len1 = fifo->buf_len - fifo->tail - 1;
                len2 = rec_len - len1;
		fifo->buf[fifo->tail] = rec_len;

                memcpy(&fifo->buf[fifo->tail + 1], rec, len1);
                memcpy(&fifo->buf[0], rec + len1, len2);
                fifo->tail = len2;
	} else {
		fifo->buf[fifo->tail] = rec_len;
		memcpy(&fifo->buf[fifo->tail + 1], rec, rec_len);
		fifo->tail = (fifo->tail + rec_len + 1) % fifo->buf_len;
	}
	fifo->no_of_rec++;
	fifo->f_bytes -= rec_len + 1;
}

int vfifo_remove(vfifo_t *fifo, char *buf, size_t buf_len)
{
	size_t rec_len;
	int ret;

	if (fifo->no_of_rec == 0)
		return -1;

	if (fifo->buf[fifo->head] > buf_len) {
		rec_len = buf_len;
		ret = -2;
	} else {
		rec_len = fifo->buf[fifo->head];
		ret = rec_len;
	}

	if (fifo->head + rec_len > fifo->buf_len) {
		size_t len1;
		size_t len2;

		len1 = (fifo->buf_len - fifo->head - 1) % rec_len;
		len2 = rec_len - len1;

		memcpy(buf, &fifo->buf[fifo->head + 1], len1);
		memcpy(buf + len1, &fifo->buf[0], len2);
	} else {
		memcpy(buf, &fifo->buf[fifo->head + 1], rec_len);
	}

	fifo->no_of_rec--;
	fifo->f_bytes += fifo->buf[fifo->head] + 1;
	fifo->head = (fifo->head + fifo->buf[fifo->head] + 1) % fifo->buf_len;
	return ret;
}

int vfifo_len(vfifo_t *fifo)
{
	return fifo->no_of_rec;
}

static print_buf(char *buf, int buf_len)
{
	int i;

	for (i = 0; i < 20; i++) {
		if(isalpha(buf[i]))
			printf("%c", buf[i]);
		else
			printf("[%d]", buf[i]);
	}
	printf("\r\n");
}

int vfifo_bulk_read(vfifo_t *fifo, char *buf, int buf_len, int no_rec)
{
	#define EMPTY_REC ("[]")
        int ret;
        int i;
        int pos = 0;
	
        for (i = 0; i < no_rec; i++) {
                ret = vfifo_remove(fifo, &buf[pos + 1], buf_len - pos - 1);
		/* print_buf(buf, buf_len); */
		/* printf("%s %d: [%d] = [%s]\r\n", __FILE__, __LINE__, ret, &buf[pos + 1]); */
                if (ret == -1) {
                        if (pos == 0) {
                                memcpy(buf, EMPTY_REC, sizeof(EMPTY_REC));
                                pos = sizeof(EMPTY_REC);
                        }
                        printf("%s %d: empty rec, pos = %d\r\n", __FILE__, __LINE__, pos);
                        return pos;
                } else if (ret == -2) {
                        printf("%s %d: rec truncated, pos = %d\r\n", __FILE__, __LINE__, pos);
                        return pos;
                } else {
			printf("%s %d: [%d] = [%.*s]\r\n", __FILE__, __LINE__, ret, ret, &buf[pos + 1]);
                        buf[pos++] = ret;
                        pos += ret;
                }
        }
        printf("%s %d: %d rec readed, pos = %d\r\n", __FILE__, __LINE__, i, pos);
        return pos;
}
