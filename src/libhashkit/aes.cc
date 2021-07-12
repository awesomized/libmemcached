/*
    +--------------------------------------------------------------------+
    | libmemcached-awesome - C/C++ Client Library for memcached          |
    +--------------------------------------------------------------------+
    | Redistribution and use in source and binary forms, with or without |
    | modification, are permitted under the terms of the BSD license.    |
    | You should have received a copy of the license in a bundled file   |
    | named LICENSE; in case you did not receive a copy you can review   |
    | the terms online at: https://opensource.org/licenses/BSD-3-Clause  |
    +--------------------------------------------------------------------+
    | Copyright (c) 2006-2014 Brian Aker   https://datadifferential.com/ |
    | Copyright (c) 2020-2021 Michael Wallner        https://awesome.co/ |
    +--------------------------------------------------------------------+
*/

#include "libhashkit/common.h"

#include <cstring>

#ifdef HAVE_OPENSSL_CRYPTO

#include <openssl/evp.h>

#define DIGEST_ROUNDS 5

#define AES_KEY_NBYTES 32
#define AES_IV_NBYTES  32

bool aes_initialize(const unsigned char *key, const size_t key_length,
                    encryption_context_t *crypto_context) {
  unsigned char aes_key[AES_KEY_NBYTES];
  unsigned char aes_iv[AES_IV_NBYTES];
  if (!key) {
    return false;
  }

  int i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), NULL, key, key_length, DIGEST_ROUNDS,
                         aes_key, aes_iv);
  if (i != AES_KEY_NBYTES) {
    return false;
  }

  EVP_CIPHER_CTX_init(crypto_context->encryption_context);
  EVP_CIPHER_CTX_init(crypto_context->decryption_context);
  if (EVP_EncryptInit_ex(crypto_context->encryption_context, EVP_aes_256_cbc(), NULL, key, aes_iv)
          != 1
      || EVP_DecryptInit_ex(crypto_context->decryption_context, EVP_aes_256_cbc(), NULL, key,
                            aes_iv)
          != 1)
  {
    return false;
  }
  return true;
}

hashkit_string_st *aes_encrypt(encryption_context_t *crypto_context, const unsigned char *source,
                               size_t source_length) {
  EVP_CIPHER_CTX *encryption_context = crypto_context->encryption_context;
  int cipher_length = source_length + EVP_CIPHER_CTX_block_size(encryption_context);
  int final_length = 0;
  unsigned char *cipher_text = (unsigned char *) malloc(cipher_length);
  if (cipher_text == NULL) {
    return NULL;
  }
  if (EVP_EncryptInit_ex(encryption_context, NULL, NULL, NULL, NULL) != 1
      || EVP_EncryptUpdate(encryption_context, cipher_text, &cipher_length, source, source_length)
          != 1
      || EVP_EncryptFinal_ex(encryption_context, cipher_text + cipher_length, &final_length) != 1)
  {
    free(cipher_text);
    return NULL;
  }

  hashkit_string_st *destination = hashkit_string_create(cipher_length + final_length);
  if (destination == NULL) {
    return NULL;
  }
  char *dest = hashkit_string_c_str_mutable(destination);
  memcpy(dest, cipher_text, cipher_length + final_length);
  hashkit_string_set_length(destination, cipher_length + final_length);
  return destination;
}

hashkit_string_st *aes_decrypt(encryption_context_t *crypto_context, const unsigned char *source,
                               size_t source_length) {
  EVP_CIPHER_CTX *decryption_context = crypto_context->decryption_context;
  int plain_text_length = source_length;
  int final_length = 0;
  unsigned char *plain_text = (unsigned char *) malloc(plain_text_length);
  if (plain_text == NULL) {
    return NULL;
  }
  if (EVP_DecryptInit_ex(decryption_context, NULL, NULL, NULL, NULL) != 1
      || EVP_DecryptUpdate(decryption_context, plain_text, &plain_text_length, source,
                           source_length)
          != 1
      || EVP_DecryptFinal_ex(decryption_context, plain_text + plain_text_length, &final_length)
          != 1)
  {
    free(plain_text);
    return NULL;
  }

  hashkit_string_st *destination = hashkit_string_create(plain_text_length + final_length);
  if (destination == NULL) {
    return NULL;
  }
  char *dest = hashkit_string_c_str_mutable(destination);
  memcpy(dest, plain_text, plain_text_length + final_length);
  hashkit_string_set_length(destination, plain_text_length + final_length);
  return destination;
}

encryption_context_t *aes_clone_cryptographic_context(encryption_context_t *source) {
  encryption_context_t *new_context = (encryption_context_t *) malloc(sizeof(encryption_context_t));
  if (new_context == NULL)
    return NULL;

  new_context->encryption_context = EVP_CIPHER_CTX_new();
  new_context->decryption_context = EVP_CIPHER_CTX_new();
  if (new_context->encryption_context == NULL || new_context->decryption_context == NULL) {
    free(new_context);
    return NULL;
  }
  EVP_CIPHER_CTX_copy(new_context->encryption_context, source->encryption_context);
  EVP_CIPHER_CTX_copy(new_context->decryption_context, source->decryption_context);
  return new_context;
}

#else

#  include "libhashkit/rijndael.hpp"

#  define AES_KEY_LENGTH 256 /* 128, 192, 256 */
#  define AES_BLOCK_SIZE 16

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
      *ptr ^= (uint8_t) (*sptr);
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
#endif
