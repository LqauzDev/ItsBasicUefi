#ifndef CRYPTO_H
#define CRYPTO_H

#include "uefi.h"
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t state[5];
    uint64_t bitcount;
    uint8_t buffer[64];
} Sha1Context;

typedef struct {
    uint32_t state[8];
    uint64_t bitcount;
    uint8_t buffer[128];
} Sha256Context;

typedef struct {
    uint32_t state[4];
    uint8_t buffer[64];
    uint64_t bitcount;
} Md5Context;

typedef struct {
    uint32_t key[44];
    uint32_t state[4];
    uint8_t buffer[16];
    uint64_t bitcount;
} AesContext;

void sha1_init(Sha1Context* ctx);
void sha1_update(Sha1Context* ctx, const uint8_t* data, size_t len);
void sha1_final(Sha1Context* ctx, uint8_t hash[20]);

void sha256_init(Sha256Context* ctx);
void sha256_update(Sha256Context* ctx, const uint8_t* data, size_t len);
void sha256_final(Sha256Context* ctx, uint8_t hash[32]);

void md5_init(Md5Context* ctx);
void md5_update(Md5Context* ctx, const uint8_t* data, size_t len);
void md5_final(Md5Context* ctx, uint8_t hash[16]);

int aes_init(AesContext* ctx, const uint8_t* key, size_t key_size);
void aes_encrypt_block(AesContext* ctx, const uint8_t* plaintext, uint8_t* ciphertext);
void aes_decrypt_block(AesContext* ctx, const uint8_t* ciphertext, uint8_t* plaintext);

void crypto_hash_string(const char* input, uint8_t* output, size_t output_size);
int crypto_verify_hash(const uint8_t* hash1, const uint8_t* hash2, size_t size);
void crypto_generate_random_bytes(uint8_t* buffer, size_t size);

#define SHA1_DIGEST_SIZE 20
#define SHA256_DIGEST_SIZE 32
#define MD5_DIGEST_SIZE 16
#define AES_BLOCK_SIZE 16
#define AES_KEY_SIZE_128 16
#define AES_KEY_SIZE_192 24
#define AES_KEY_SIZE_256 32

#endif
