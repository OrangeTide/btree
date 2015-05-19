#ifndef SHA1TINY_H
#define SHA1TINY_H
/**
 * size of a SHA-1 digest in bytes. SHA-1 is 160-bit.
 */
#define SHA1_DIGEST_LENGTH 20

unsigned char *sha1(const void *data, size_t len, unsigned char md[SHA1_DIGEST_LENGTH]);
#endif
