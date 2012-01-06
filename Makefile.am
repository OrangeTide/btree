AUTOMAKE_OPTIONS = subdir-objects

lib_LIBRARIES = libbtree.a

libbtree_a_SOURCES = btree.c

include_HEADERS = btree.h

man3_MANS = btree.3

noinst_PROGRAMS = btest

btest_SOURCES = btest.c

btest_LDADD = libbtree.a -lssl

AM_CFLAGS = -Wall -W -g -std=c89 -D_XOPEN_SOURCE=700
AM_CFLAGS += -D_GNU_SOURCE # asprintf
AM_CFLAGS += -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
AM_CFLAGS += -I$(top_srcdir)/compat
