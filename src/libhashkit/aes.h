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

#pragma once

#ifdef WITH_OPENSSL

#include <openssl/evp.h>

typedef struct encryption_context {
  EVP_CIPHER_CTX *encryption_context;
  EVP_CIPHER_CTX *decryption_context;
} encryption_context_t;

hashkit_string_st *aes_encrypt(encryption_context_t *crypto_context, const unsigned char *source,
                               size_t source_length);

hashkit_string_st *aes_decrypt(encryption_context_t *crypto_context, const unsigned char *source,
                               size_t source_length);

bool aes_initialize(const unsigned char *key, const size_t key_length,
                    encryption_context_t *crypto_context);

encryption_context_t *aes_clone_cryptographic_context(encryption_context_t *source);
#else

struct aes_key_t;

hashkit_string_st *aes_encrypt(aes_key_t *_aes_key, const char *source, size_t source_length);

hashkit_string_st *aes_decrypt(aes_key_t *_aes_key, const char *source, size_t source_length);

aes_key_t *aes_create_key(const char *key, const size_t key_length);

aes_key_t *aes_clone_key(aes_key_t *_aes_key);
#endif