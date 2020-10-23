/*
    +--------------------------------------------------------------------+
    | libmemcached - C/C++ Client Library for memcached                  |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020 Michael Wallner   <mike@php.net>                |
    +--------------------------------------------------------------------+
*/

#include "libhashkit/common.h"

#include "libhashkit/rijndael.hpp"

#include <cstring>

#define AES_KEY_LENGTH 256 /* 128, 192, 256 */
#define AES_BLOCK_SIZE 16

enum encrypt_t { AES_ENCRYPT, AES_DECRYPT };

struct _key_t {
  int nr;
  uint32_t rk[4 * (AES_MAXNR + 1)];
};

struct aes_key_t {
  _key_t encode_key;
  _key_t decode_key;
};

aes_key_t *aes_create_key(const char *key, const size_t key_length) {
  aes_key_t *_aes_key = (aes_key_t *) (calloc(1, sizeof(aes_key_t)));
  if (_aes_key) {
    uint8_t rkey[AES_KEY_LENGTH / 8];
    uint8_t *rkey_end = rkey + AES_KEY_LENGTH / 8;
    const char *key_end = key + key_length;

    memset(rkey, 0, sizeof(rkey)); /* Set initial key  */

    uint8_t *ptr = rkey;
    const char *sptr = key;
    for (; sptr < key_end; ptr++, sptr++) {
      if (ptr == rkey_end) {
        ptr = rkey; /*  Just loop over tmp_key until we used all key */
      }
      *ptr ^= (uint8_t)(*sptr);
    }

    _aes_key->decode_key.nr = rijndaelKeySetupDec(_aes_key->decode_key.rk, rkey, AES_KEY_LENGTH);
    _aes_key->encode_key.nr = rijndaelKeySetupEnc(_aes_key->encode_key.rk, rkey, AES_KEY_LENGTH);
  }

  return _aes_key;
}

aes_key_t *aes_clone_key(aes_key_t *_aes_key) {
  if (_aes_key == NULL) {
    return NULL;
  }

  aes_key_t *_aes_clone_key = (aes_key_t *) (calloc(1, sizeof(aes_key_t)));
  if (_aes_clone_key) {
    memcpy(_aes_clone_key, _aes_key, sizeof(aes_key_t));
  }

  return _aes_clone_key;
}

hashkit_string_st *aes_encrypt(aes_key_t *_aes_key, const char *source, size_t source_length) {
  if (_aes_key == NULL) {
    return NULL;
  }

  size_t num_blocks = source_length / AES_BLOCK_SIZE;

  hashkit_string_st *destination = hashkit_string_create(source_length);
  if (destination) {
    char *dest = hashkit_string_c_str_mutable(destination);

    for (size_t x = num_blocks; x > 0; x--) /* Encode complete blocks */ {
      rijndaelEncrypt(_aes_key->encode_key.rk, _aes_key->encode_key.nr, (const uint8_t *) (source),
                      (uint8_t *) (dest));
      source += AES_BLOCK_SIZE;
      dest += AES_BLOCK_SIZE;
    }

    uint8_t block[AES_BLOCK_SIZE];
    char pad_len = AES_BLOCK_SIZE - (source_length - AES_BLOCK_SIZE * num_blocks);
    memcpy(block, source, 16 - pad_len);
    memset(block + AES_BLOCK_SIZE - pad_len, pad_len, pad_len);
    rijndaelEncrypt(_aes_key->encode_key.rk, _aes_key->encode_key.nr, block, (uint8_t *) (dest));
    hashkit_string_set_length(destination, AES_BLOCK_SIZE * (num_blocks + 1));
  }

  return destination;
}

hashkit_string_st *aes_decrypt(aes_key_t *_aes_key, const char *source, size_t source_length) {
  if (_aes_key == NULL) {
    return NULL;
  }

  size_t num_blocks = source_length / AES_BLOCK_SIZE;
  if ((source_length != num_blocks * AES_BLOCK_SIZE) or num_blocks == 0) {
    return NULL;
  }

  hashkit_string_st *destination = hashkit_string_create(source_length);
  if (destination) {
    char *dest = hashkit_string_c_str_mutable(destination);

    for (size_t x = num_blocks - 1; x > 0; x--) {
      rijndaelDecrypt(_aes_key->decode_key.rk, _aes_key->decode_key.nr, (const uint8_t *) (source),
                      (uint8_t *) (dest));
      source += AES_BLOCK_SIZE;
      dest += AES_BLOCK_SIZE;
    }

    uint8_t block[AES_BLOCK_SIZE];
    rijndaelDecrypt(_aes_key->decode_key.rk, _aes_key->decode_key.nr, (const uint8_t *) (source),
                    block);
    /* Use last char in the block as size */
    unsigned int pad_len = (unsigned int) (unsigned char) (block[AES_BLOCK_SIZE - 1]);
    if (pad_len > AES_BLOCK_SIZE) {
      hashkit_string_free(destination);
      return NULL;
    }

    /* We could also check whole padding but we do not really need this */

    memcpy(dest, block, AES_BLOCK_SIZE - pad_len);
    hashkit_string_set_length(destination, AES_BLOCK_SIZE * num_blocks - pad_len);
  }

  return destination;
}
