#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "vfifo.h"

#define ARRAY_LEN(arr) (sizeof(arr)/sizeof(arr[0]))

/**
 * Setup function for each test cases
 */
int setup(void **fifo)
{
	static char buf[20];
	static vfifo_t emon;

	*fifo = &emon;
	vfifo_init(&emon, buf, sizeof(buf));

	return 0;
}

/**
 * Write 1 record and read 1 record.
 */
void test_case_1(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char buf[4];
	char str[] = "make";
	int ret;

	vfifo_add(e_fifo, str, strlen(str));
	ret = vfifo_remove(e_fifo, buf, sizeof(buf));
	assert_int_equal(ret, 0);
	assert_memory_equal(str, buf, strlen(str));
}

/**
 * Write 1 record and read 1 record with small buffer size
 */
void test_case_2(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char buf[4];
	char str[] = "volt";
	int ret;

	vfifo_add(e_fifo, str, strlen(str));
	ret = vfifo_remove(e_fifo, buf, 3);
	assert_int_equal(ret, -2);
	assert_memory_equal(str, buf, 3);
}

/**
 * Try to remove record from empty FIFO, check return value.
 */
void test_case_3(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char buf[4];
	int ret;

	ret = vfifo_remove(e_fifo, buf, sizeof(buf));
	assert_int_equal(ret, -1);
}


/**
 * Write same length of records to FIFO till it get full,
 * remove and compare all records.
 */
void test_case_4(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char buf[4];
	char * const str[] = {"laze", "flux", "jobs", "jeep"};
	int ret;
	int i;

	for (i = 0; i < ARRAY_LEN(str); i++)
		vfifo_add(e_fifo, str[i], strlen(str[i]));

	for (i = 0; i < ARRAY_LEN(str); i++) {
		ret = vfifo_remove(e_fifo, buf, sizeof(buf));
		assert_int_equal(ret, 0);
		assert_memory_equal(str[i], buf, strlen(str[i]));
	}
}

/**
 * Write same length of records to FIFO till it get full, remove all
 * records and check FIFO's length for each vfifo_remove call.
 */
void test_case_5(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char buf[4];
	char * const str[] = {"laze", "flux", "jobs", "jeep"};
	int ret;
	int i;

	for (i = 0; i < ARRAY_LEN(str); i++) {
		ret = vfifo_len(e_fifo);
		assert_int_equal(ret, i);
		vfifo_add(e_fifo, str[i], strlen(str[i]));
	}

	for (i = ARRAY_LEN(str); i >= 0; i--) {
		ret = vfifo_len(e_fifo);
		assert_int_equal(ret, i);
		vfifo_remove(e_fifo, buf, sizeof(buf));
	}
}

/**
 * Add same length of records to FIFO till first records will get
 * overwrite, remove and compare all records, except the first
 * records.
 */
void test_case_6(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char buf[4];
	char * const str[] = {"laze", "flux", "jobs", "jeep", "boot"};
	int ret;
	int i;

	for (i = 0; i < ARRAY_LEN(str); i++)
		vfifo_add(e_fifo, str[i], strlen(str[i]));

	for (i = 1; i < ARRAY_LEN(str); i++) {
		ret = vfifo_remove(e_fifo, buf, sizeof(buf));
		assert_int_equal(ret, 0);
		assert_memory_equal(str[i], buf, strlen(str[i]));
	}
}

/**
 * Add same length of records to FIFO till the first records will get
 * overwrite, remove all records and check FIFO's length for each
 * vfifo_remove call.
 */
void test_case_7(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char buf[4];
	char * const str[] = {"laze", "flux", "jobs", "jeep", "boot"};
	int ret;
	int i;

	for (i = 0; i < ARRAY_LEN(str); i++) {
		ret = vfifo_len(e_fifo);
		assert_int_equal(ret, i);
		vfifo_add(e_fifo, str[i], strlen(str[i]));
	}

	for (i = ARRAY_LEN(str) - 1; i >= 0; i--) {
		ret = vfifo_len(e_fifo);
		assert_int_equal(ret, i);
		vfifo_remove(e_fifo, buf, sizeof(buf));
	}
}

/**
 * Add same length of records to FIFO till the first two records will
 * get overwrite, remove and compare all records, except first two
 * records.
 */
void test_case_8(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char buf[4];
	char * const str[] = {"laze", "flux", "jobs", "jeep", "boot", "good"};
	int ret;
	int i;

	for (i = 0; i < ARRAY_LEN(str); i++)
		vfifo_add(e_fifo, str[i], strlen(str[i]));

	for (i = 2; i < ARRAY_LEN(str); i++) {
		ret = vfifo_remove(e_fifo, buf, sizeof(buf));
		assert_int_equal(ret, 0);
		assert_memory_equal(str[i], buf, strlen(str[i]));
	}
}

/**
 * Add same length of records to FIFO till the first two records will
 * get overwrite, remove all records and check FIFO's length for each
 * vfifo_remove call.
 */
void test_case_9(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char buf[4];
	char * const str[] = {"laze", "flux", "jobs", "jeep", "boot", "good"};
	int ret;
	int i;

	for (i = 0; i < ARRAY_LEN(str); i++) {
		ret = vfifo_len(e_fifo);
		if (i < 4)
			assert_int_equal(ret, i);
		else
			assert_int_equal(ret, 4);
		vfifo_add(e_fifo, str[i], strlen(str[i]));
	}

	for (i = ARRAY_LEN(str) - 2; i >= 0; i--) {
		ret = vfifo_len(e_fifo);
		assert_int_equal(ret, i);
		vfifo_remove(e_fifo, buf, sizeof(buf));
	}
}

/**
 * Write different length of records to FIFO till first record get
 * overwrite, last record should be present in both edges of the
 * FIFO. remove and compare all records, and check FIFO's length.
 */
void test_case_10(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char * const str[] = {"Brazil", "Cuba", "Egypt", "Iran"};
	char buf[10];
	int ret;
	int i;
	int len;

	for (i = 0; i < ARRAY_LEN(str); i++) {
		vfifo_add(e_fifo, str[i], strlen(str[i]));
		ret = vfifo_len(e_fifo);
		if (i < 3)
			assert_int_equal(ret, i + 1);
		else
			assert_int_equal(ret, 3);
	}
	
	
	for (len = 2, i = 1; i <= 3; i++, len--) {
		ret = vfifo_remove(e_fifo, buf, sizeof(buf));
		assert_int_equal(ret, 0);
		assert_memory_equal(str[i], buf, strlen(str[i]));
		ret = vfifo_len(e_fifo);
		assert_int_equal(ret, len);
	}
}

/**
 * Write different length of records to FIFO till first record get
 * overwrite, last record should be present in both edges of the
 * FIFO. remove records with small buffer size, check return value,
 * truncated records and FIFO's length.
 */
void test_case_11(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char * const str[] = {"Brazil", "Cuba", "Egypt", "Iran"};
	char buf[3];
	int ret;
	int i;
	int len;

	for (i = 0; i < ARRAY_LEN(str); i++) {
		vfifo_add(e_fifo, str[i], strlen(str[i]));
		ret = vfifo_len(e_fifo);
		if (i < 3)
			assert_int_equal(ret, i + 1);
		else
			assert_int_equal(ret, 3);
	}
	
	
	for (len = 2, i = 1; i <= 3; i++, len--) {
		ret = vfifo_remove(e_fifo, buf, sizeof(buf));
		assert_int_equal(ret, -2);
		assert_memory_equal(str[i], buf, sizeof(buf));
		ret = vfifo_len(e_fifo);
		assert_int_equal(ret, len);
	}
}

/**
 * Write different length of records to FIFO till first record get
 * overwrite, but last record's head byte at end of the FIFO and rest
 * of them at beginning of the FIFO. remove and compare all records,
 * and check FIFO's length.
 */
void test_case_12(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char * const str[] = {"Brazil", "Cuba", "Egypt", "Iran"};
	char buf[10];
	int ret;
	int i;
	int len;

	for (i = 0; i < ARRAY_LEN(str); i++) {
		vfifo_add(e_fifo, str[i], strlen(str[i]));
		ret = vfifo_len(e_fifo);
		if (i < 3)
			assert_int_equal(ret, i + 1);
		else
			assert_int_equal(ret, 3);
	}
	
	
	for (len = 2, i = 1; i <= 3; i++, len--) {
		ret = vfifo_remove(e_fifo, buf, sizeof(buf));
		assert_int_equal(ret, 0);
		assert_memory_equal(str[i], buf, strlen(str[i]));
		ret = vfifo_len(e_fifo);
		assert_int_equal(ret, len);
	}
}

/**
 * Move FIFO head and tail at `n`th byte by write and read action.
 * Add different length of records to FIFO up to `n`th byte. remove
 * and compare all records and check FIFO's length.
 */
void test_case_13(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char * const str[] = {"Brazil", "Cuba", "Russia", "Iran"};
	char buf[3];
	int ret;
	int i;
	int len;

	for (i = 0; i < ARRAY_LEN(str); i++) {
		vfifo_add(e_fifo, str[i], strlen(str[i]));
		ret = vfifo_len(e_fifo);
		if (i < 3)
			assert_int_equal(ret, i + 1);
		else
			assert_int_equal(ret, 3);
	}
	
	
	for (len = 2, i = 1; i <= 3; i++, len--) {
		ret = vfifo_remove(e_fifo, buf, sizeof(buf));
		assert_int_equal(ret, -2);
		assert_memory_equal(str[i], buf, sizeof(buf));
		ret = vfifo_len(e_fifo);
		assert_int_equal(ret, len);
	}
}

/**
 * Move FIFO head and tail at `n`th byte by write and read action.
 * Add different length of records to FIFO till `n`th byte get
 * overwrite. remove and compare all records and check FIFO's length.
 */
void test_case_14(void **fifo)
{
	vfifo_t *e_fifo = *fifo;
	char * const str[] = {"Brazil", "Cuba", "Russia", "Iran"};
	char buf[10];
	int ret;
	int len;
	int i;

	/* Change FIFO's head and tail position to 'n'th byte */
	for (i = 0; i < 2; i++)
		vfifo_add(e_fifo, str[i], strlen(str[i]));

	for (i = 0; i < 2; i++)
		vfifo_remove(e_fifo, buf, sizeof(buf));


	/* Write and read from 'n'th byte */
	for (i = 0; i < ARRAY_LEN(str); i++) {
		vfifo_add(e_fifo, str[i], strlen(str[i]));
		ret = vfifo_len(e_fifo);
		if (i < 3)
			assert_int_equal(ret, i + 1);
		else
			assert_int_equal(ret, 3);
	}
	
	for (len = 2, i = 1; i <= 3; i++, len--) {
		ret = vfifo_remove(e_fifo, buf, strlen(str[i]));
		assert_int_equal(ret, 0);
		assert_memory_equal(str[i], buf, strlen(str[i]));
		ret = vfifo_len(e_fifo);
		assert_int_equal(ret, len);
	}
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_case_1, setup),
		cmocka_unit_test_setup(test_case_2, setup),
		cmocka_unit_test_setup(test_case_3, setup),
		cmocka_unit_test_setup(test_case_4, setup),
		cmocka_unit_test_setup(test_case_5, setup),
		cmocka_unit_test_setup(test_case_6, setup),
		cmocka_unit_test_setup(test_case_8, setup),
		cmocka_unit_test_setup(test_case_7, setup),
		cmocka_unit_test_setup(test_case_9, setup),
		cmocka_unit_test_setup(test_case_10, setup),
		cmocka_unit_test_setup(test_case_11, setup),
		cmocka_unit_test_setup(test_case_12, setup),
		cmocka_unit_test_setup(test_case_13, setup),
		cmocka_unit_test_setup(test_case_14, setup),
	};

	cmocka_run_group_tests(tests, NULL, NULL);
	return 0;
}

