#ifndef __MD5_H__
#define __MD5_H__

#include "c_types.h"

#define MD5_DIGEST_LENGTH		16

typedef struct
{
  uint32_t state[5];
  uint32_t count[2];
  uint8_t buffer[64];
} MD5_CTX;

void MD5Init(MD5_CTX *);
void MD5Update(MD5_CTX *, const unsigned char *, unsigned int);
void MD5Final(unsigned char[MD5_DIGEST_LENGTH], MD5_CTX *);

/*
 * base64_encode/decode derived by Cal. Appears to match base64.h
 * from netbsd wpa utils.
 */
unsigned char * base64_encode(const unsigned char *src, size_t len, size_t *out_len);
unsigned char * base64_decode(const unsigned char *src, size_t len, size_t *out_len);

#endif
