/******************************************************************************
 * SHA1
 ******************************************************************************/
/* SHA-1 hashing routines.
 * see RFC3174 for the SHA-1 algorithm.
 *
 * Jon Mayo - PUBLIC DOMAIN - June 24, 2010
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "sha1tiny.h"

/* rotate v left by b bits, truncate to 32-bits. */
#define ROL32(v, b) ((((v) << (b)) | ((v) >> (32 - (b)))) & 0xfffffffful)

/**
 * quick calculation of SHA1 on buffer data.
 * data - pointer.
 * len - length of data.
 * md - cannot be NULL.
 * return md.
 */
unsigned char *sha1(const void *data, size_t len, unsigned char md[SHA1_DIGEST_LENGTH])
{
	uint_least32_t h[5] = { 0x67452301lu, 0xefcdab89lu, 0x98badcfelu, 0x10325476lu, 0xc3d2e1f0lu };
	uint_least32_t v[5]; /* called a, b, c, d, e in documentation. */
	uint_least64_t cnt = 0; /* total so far in bits. */
	uint_least32_t chunk[80]; /* 0..15 is 512-bit of data. 16..79 is copies. */
	unsigned i, tail_fl = 0, length_fl = 0;

	while (tail_fl || len) {
		/* fill in chunk[0..15] using data[+0..+63] */
		for (i = 0; i < 512 / 8; i++) {
			uint_least32_t n = chunk[i / 4], d = 0;

			if (len > i) {
				d = *(uint8_t*)data;
				data = (uint8_t*)data + 1;
				cnt += 8;
			} else if (!tail_fl && i == len) {
				d = 0x80;
				tail_fl = 1; /* tail_fl is used when end of data is reached. */
			} else if (tail_fl && i >= 448 / 8) {
				if (i == 448 / 8)
					length_fl = 1; /* length_fl is set when filling in length data. */

				if (length_fl) {
					d = (cnt >> (64 - (i - 448 / 8 + 1) * 8)) & 255;

					if (i == 512 / 8 - 1)
						tail_fl = 0; /* finished with length. */
				}
			}

			switch (i % 4) {
			case 0:
				n = (n & 0xffffff) | (d << 24);
				break;
			case 1:
				n = (n & 0xff00ffff) | (d << 16);
				break;
			case 2:
				n = (n & 0xffff00ff) | (d << 8);
				break;
			case 3:
				n = (n & 0xffffff00) | d;
				break;
			}

			chunk[i / 4] = n;
		}

		if (len > 512 / 8) {
			len -= 512 / 8;
		} else {
			len = 0;
		}

		/* transform chunk */
		for (i = 16; i < 80; i++) {
			chunk[i] = chunk[i - 3] ^ chunk[i - 8] ^ chunk[i - 14] ^ chunk[i - 16];
			chunk[i] = ROL32(chunk[i], 1); /* rotate left 1 */
		}

		for (i = 0; i < 5; i++)
			v[i] = h[i]; /* set a-e to h0..4 */

		for( i = 0; i < 80; i++) {
			uint_least32_t f, k, tmp;

			if (i < 20) {
				f = (v[1] & v[2]) | (~v[1] & v[3]);
				/* f=v[3]^(v[1]&(v[2]^v[3])); */
				k = 0x5a827999;
			} else if (i < 40) {
				f = v[1] ^ v[2] ^ v[3];
				/* f=(v[1]&v[2])^((~v[1])&v[3]) */
				k = 0x6ed9eba1;
			} else if (i < 60) {
				f = (v[1] & v[2]) | (v[1] & v[3]) | (v[2] & v[3]);
				/* f=(v[1]&v[2])+((~v[1])&v[3]); */
				k = 0x8f1bbcdc;
			} else {
				f = v[1] ^ v[2] ^ v[3];
				k = 0xca62c1d6;
			}

			tmp = ROL32(v[0], 5) + f + v[4] + k + chunk[i]; /* (a ROL 5) + f + e + k + w[i] */
			v[4] = v[3]; /* e = d */
			v[3] = v[2]; /* d = c */
			v[2] = ROL32(v[1], 30); /* c = b ROL 30 */
			v[1] = v[0]; /* b = a */
			v[0] = tmp; /* a = tmp */
		}

		for(i = 0; i < 5; i++) h[i] += v[i]; /* add a-e to h0..4 */
	}

	/* produce final hash */
	for(i = 0; i < 5; i++) {
		*md++ = h[i] >> 24;
		*md++ = h[i] >> 16;
		*md++ = h[i] >> 8;
		*md++ = h[i];
	}

	return md;
}
