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

#ifdef WITH_OPENSSL
#  include <openssl/evp.h>
#endif

hashkit_string_st *hashkit_encrypt(hashkit_st *kit, const char *source, size_t source_length) {
#ifdef WITH_OPENSSL
  return aes_encrypt((encryption_context_t *) kit->_cryptographic_context,
                     (const unsigned char *) source, source_length);
#else
  return aes_encrypt((aes_key_t *) kit->_cryptographic_context, source,
                     source_length);
#endif
}

hashkit_string_st *hashkit_decrypt(hashkit_st *kit, const char *source, size_t source_length) {
#ifdef WITH_OPENSSL
  return aes_decrypt((encryption_context_t *) kit->_cryptographic_context,
                     (const unsigned char *) source, source_length);
#else
  return aes_decrypt((aes_key_t *)kit->_cryptographic_context, source, source_length);
#endif
}

#ifdef WITH_OPENSSL
bool hashkit_key(hashkit_st *kit, const char *key, const size_t key_length) {
  kit->_cryptographic_context = (encryption_context_t *) malloc(sizeof(encryption_context_t));
  ((encryption_context_t *) kit->_cryptographic_context)->encryption_context = EVP_CIPHER_CTX_new();
  ((encryption_context_t *) kit->_cryptographic_context)->decryption_context = EVP_CIPHER_CTX_new();
  if (((encryption_context_t *) kit->_cryptographic_context)->encryption_context == NULL
      || ((encryption_context_t *) kit->_cryptographic_context)->decryption_context == NULL)
  {
    return false;
  }
  return aes_initialize((const unsigned char *) key, key_length,
                        (encryption_context_t *) kit->_cryptographic_context);
}
#else
bool hashkit_key(hashkit_st *kit, const char *key, const size_t key_length) {
  if (kit->_cryptographic_context) {
    free(kit->_cryptographic_context);
  }

  kit->_cryptographic_context = aes_create_key(key, key_length);

  return bool(kit->_cryptographic_context);
}
#endif