#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "vfifo.h"

void print_fifo(vfifo_t *fifo)
{
	printf("\r\nFIFO Data:\r\n");
	printf("Head: %d\n", fifo->head);
	printf("Tail: %d\n", fifo->tail);
	printf("LEN : %d\n", fifo->buf_len);
	printf("Free: %d\n", fifo->f_bytes);
	printf("NRec: %d\n", fifo->no_of_rec);
}

void print_rec(vfifo_t *fifo)
{
	int i;

	for (i = 0; i < fifo->buf_len; i++) {
		if (fifo->head == i)
			printf("{");
		if (fifo->tail == i)
			printf("[");

		if (!isalpha(fifo->buf[i]))
			printf("%d", fifo->buf[i]);
		else
			printf("%c", fifo->buf[i]);

		if (fifo->tail == i)
			printf("]");
		if (fifo->head == i)
			printf("}");
	}
	printf("\r\n");
}

void get_rec(vfifo_t *fifo)
{
	int i;
	int ret;
	char str[20] = {0};

	for (i = 0; i < 1; i++) {
		memset(str, 0, 20);
		ret = vfifo_remove(fifo, str, 5);
		if (ret == 0)
			printf("Str %d: %s\n", i, str);
		else if (ret == -2)
			printf("Str Trun %d: %s\n", i, str);
		else
			printf("Remove %d\n", ret);
	}
}

int main(void)
{
	vfifo_t emon;
	int ch;
	char str[20] = {0};
	uint8_t buf[100] = {0};
	int size;

	printf("Enter FIFO size: ");
	scanf("%d", &size);

	vfifo_init(&emon, buf, size);

	while (1) {
		puts("\n1. Add string");
		puts("2. Get string");
		puts("3. Get no of Records");
		puts("4. Print BUF ");
		puts("5. print FIFO");
		puts("6. Bulk Read");
		puts("Enter your option: ");
		scanf("%d", &ch);


		if (1 == ch) {
			printf("Enter a string: ");
			scanf("%s", str);
			vfifo_add(&emon, str, strlen(str));
		} else if (2 == ch) {
			get_rec(&emon);
		} else if (3 == ch) {
			printf("No of Rec is %d\n", vfifo_len(&emon));
		} else if (4 == ch) {
			print_rec(&emon);
		} else if (5 == ch) {
			print_fifo(&emon);
		} else if (6 == ch) {
			int i;
			char buf[100];
			int ret;

			ret = vfifo_bulk_read(&emon, str, sizeof(str), 5);
			printf("%s %d: ret = %d", __FILE__, __LINE__, ret);
			for (i = 0; i < ret; i++) {
				if(isalpha(str[i]))
					printf("%c", str[i]);
				else
					printf("[%d]", str[i]);
			}
		}
	}

	return 0;
}
