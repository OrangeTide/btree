/*
 * Copyright (c) 2010 Martin Hedenfalk <martin@bzero.se>
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

#ifndef _compat_h_
#define _compat_h_

#include <sys/types.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdint.h>

#ifdef HAVE_SYS_FILE_H_
# include <sys/file.h>
#endif

#ifndef __dead
#define __dead
#endif

#ifndef __packed
#define __packed __attribute__ ((__packed__))
#endif

#ifndef HAVE_ARC4RANDOM
unsigned int	 arc4random(void);
void		 arc4random_stir(void);
#endif

#ifndef HAVE_ARC4RANDOM_BUF
void		 arc4random_buf(void *_buf, size_t n);
#endif

#ifndef HAVE_ARC4RANDOM_UNIFORM
u_int32_t	 arc4random_uniform(u_int32_t upper_bound);
#endif

#ifndef HAVE_STRLCPY
size_t		 strlcpy(char *dst, const char *src, size_t siz);
#endif

#ifndef HAVE_STRLCAT
size_t		 strlcat(char *dst, const char *src, size_t siz);
#endif

#ifndef HAVE_MEMRCHR
void		*memrchr(const void *s, int c, size_t n);
#endif

#ifndef HAVE_STRTONUM
long long	 strtonum(const char *numstr, long long minval,
		    long long maxval, const char **errstrp);
#endif

int		 base64_pton(char const *src, u_char *target, size_t targsize);

#endif

