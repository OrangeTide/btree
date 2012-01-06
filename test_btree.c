/*
 * Copyright (c) 2012 Jon Mayo <jon@cobra-kai.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "btree.h"

static int passes, failures;

/* a copy of a very old rand() */
#define ANS_RAND_MAX 0x7fffffff

static unsigned long ans_seed;

static void
ans_srand(unsigned long seed)
{
	ans_seed = seed;
}

static unsigned
ans_rand(void)
{
	ans_seed = (1103515245 * ans_seed + 12345) & ANS_RAND_MAX;
	return ans_seed;
}

static const char *
randname(unsigned len)
{
	static char buf[64];
	const char set[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLM"
		"NOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	unsigned i;

	assert(len > 0);
	if (len >= sizeof(buf) )
		len = sizeof(buf) - 1;

	if (len <= 0) {
		fprintf(stderr, "ERROR:requested length is zero\n");
		return NULL;
	}
	fprintf(stderr, "INFO:len=%d\n", len);

	for (i = 0; i < len; i++) {
		buf[i] = set[ans_rand() % (sizeof(set) - 1)];
	}
	buf[i] = 0;
	return buf;
}

static int
do_writes(struct btree *bt, int count, int min, int max)
{
	char value[64];
	int i;

	ans_srand(0);
	for (i = 0; i < count; i++) {
		struct btval key, data;
		int rc;

		key.data = (void*)randname(ans_rand() % (max - min + 1) + min);
		key.size = strlen(key.data);
		if (key.size <= 0) {
			fprintf(stderr, "ERROR:key is zero length\n");
			return 0;
		}
		data.data = value;
		data.size = snprintf(value, sizeof(value), "%X", i);
		fprintf(stderr, "INFO:put key '%s'\n", (char*)key.data);
		rc = btree_put(bt, &key, &data, 0);
		if (rc != BT_SUCCESS) {
			fprintf(stderr, "ERROR:failed to write key '%s'\n",
				(char*)key.data);
			return 0;
		}
	}

	return 1;
}

static int
do_reads(struct btree *bt, int count, int min, int max)
{
	char value[64];
	size_t value_len;
	int i;

	ans_srand(0);
	for (i = 0; i < count; i++) {
		struct btval key, data;
		int rc;

		key.data = (void*)randname(ans_rand() % (max - min + 1) + min);
		key.size = strlen(key.data);
		if (key.size <= 0) {
			fprintf(stderr, "ERROR:key is zero length\n");
			return 0;
		}
		data.data = NULL;
		data.size = 0;
		value_len = snprintf(value, sizeof(value), "%X", i);
		fprintf(stderr, "INFO:get key '%s'\n", (char*)key.data);
		rc = btree_get(bt, &key, &data);
		if (rc != BT_SUCCESS) {
			fprintf(stderr, "ERROR:failed to read key '%s'\n",
				(char*)key.data);
			return 0;
		}
		if (value_len != data.size || memcmp(value, data.data, value_len)) {
			fprintf(stderr, "ERROR:unexpected value '%.*s' from key '%s'\n",
				(int)data.size, (char*)data.data,
				(char*)key.data);
			return 0;
		}
	}

	return 1;
}

static int
do_deletes(struct btree *bt, int count, int min, int max)
{
	int i;

	ans_srand(0);
	for (i = 0; i < count; i++) {
		struct btval key;
		int rc;

		key.data = (void*)randname(ans_rand() % (max - min + 1) + min);
		key.size = strlen(key.data);
		if (key.size <= 0) {
			fprintf(stderr, "ERROR:key is zero length\n");
			return 0;
		}
		fprintf(stderr, "INFO:delete key '%s'\n", (char*)key.data);
		rc = btree_del(bt, &key, NULL);
		if (rc != BT_SUCCESS) {
			fprintf(stderr, "ERROR:failed to delete key '%s'\n",
				(char*)key.data);
			return 0;
		}
	}

	return 1;
}

static void
test(const char *name, int status)
{
	if (status)
		passes++;
	else
		failures++;
	fprintf(stderr, "%s:%s\n", status ? "PASS" : "FAILURE", name);
}

static void
report(void)
{
	fprintf(stderr, "RESULTS:total=%d failures=%d\n",
		passes + failures, failures);
}

int
main()
{
	struct btree *bt;
	const char filename[] = "test.db";

	test("btree_open(BT_NOSYNC)",
		(bt = btree_open(filename, BT_NOSYNC, 0644)) != NULL);
	test("do_writes(50, 1, 33)", do_writes(bt, 50, 1, 33));
	test("do_reads(50, 1, 33)", do_reads(bt, 50, 1, 33));
	test("do_deletes(50, 1, 33)", do_deletes(bt, 50, 1, 33));
	test("btree_close", (btree_close(bt),1));


	test("btree_open(!BT_NOSYNC)",
		(bt = btree_open(filename, 0, 0644)) != NULL);
	test("do_writes(50, 1, 33)", do_writes(bt, 50, 1, 33));
	test("do_reads(50, 1, 33)", do_reads(bt, 50, 1, 33));
	test("do_deletes(50, 1, 33)", do_deletes(bt, 50, 1, 33));
	test("btree_close", (btree_close(bt),1));


	test("btree_open(BT_RDONLY)",
		(bt = btree_open(filename, BT_RDONLY, 0644)) != NULL);
	test("do_reads(50, 1, 33)", do_reads(bt, 50, 1, 33));
	test("btree_close", (btree_close(bt),1));


	test("btree_open(BT_NOSYNC | BT_REVERSEKEY)",
		(bt = btree_open(filename, BT_NOSYNC | BT_REVERSEKEY, 0644)) != NULL);
	test("do_writes(50, 1, 33)", do_writes(bt, 50, 1, 33));
	test("do_reads(50, 1, 33)", do_reads(bt, 50, 1, 33));
	test("do_deletes(50, 1, 33)", do_deletes(bt, 50, 1, 33));
	test("btree_close", (btree_close(bt),1));

	report();
	return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
