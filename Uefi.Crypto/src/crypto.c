#include "crypto.h"
#include "../Uefi.StdLib/include/string.h"

#define SHA1_K0 0x5a827999
#define SHA1_K20 0x6ed9eba1
#define SHA1_K40 0x8f1bbcdc
#define SHA1_K60 0xca62c1d6

#define SHA256_K0 0x428a2f98
#define SHA256_K1 0x71374491
#define SHA256_K2 0xb5c0fbcf
#define SHA256_K3 0xe9b5dba5
#define SHA256_K4 0x3956c25b
#define SHA256_K5 0x59f111f1
#define SHA256_K6 0x923f82a4
#define SHA256_K7 0xab1c5ed5
#define SHA256_K8 0xd807aa98
#define SHA256_K9 0x12835b01
#define SHA256_K10 0x243185be
#define SHA256_K11 0x550c7dc3
#define SHA256_K12 0x72be5d74
#define SHA256_K13 0x80deb1fe
#define SHA256_K14 0x9bdc06a7
#define SHA256_K15 0xc19bf174
#define SHA256_K16 0xe49b69c1
#define SHA256_K17 0xefbe4786
#define SHA256_K18 0x0fc19dc6
#define SHA256_K19 0x240ca1cc
#define SHA256_K20 0x2de92c6f
#define SHA256_K21 0x4a7484aa
#define SHA256_K22 0x5cb0a9dc
#define SHA256_K23 0x76f988da
#define SHA256_K24 0x983e5152
#define SHA256_K25 0xa831c66d
#define SHA256_K26 0xb00327c8
#define SHA256_K27 0xbf597fc7
#define SHA256_K28 0xc6e00bf3
#define SHA256_K29 0xd5a79147
#define SHA256_K30 0x06ca6351
#define SHA256_K31 0x14292967
#define SHA256_K32 0x27b70a85
#define SHA256_K33 0x2e1b2138
#define SHA256_K34 0x4d2c6dfc
#define SHA256_K35 0x53380d13
#define SHA256_K36 0x650a7354
#define SHA256_K37 0x766a0abb
#define SHA256_K38 0x81c2c92e
#define SHA256_K39 0x92722c85
#define SHA256_K40 0xa2bfe8a1
#define SHA256_K41 0xa81a664b
#define SHA256_K42 0xc24b8b70
#define SHA256_K43 0xc76c51a3
#define SHA256_K44 0xd192e819
#define SHA256_K45 0xd6990624
#define SHA256_K46 0xf40e3585
#define SHA256_K47 0x106aa070
#define SHA256_K48 0x19a4c116
#define SHA256_K49 0x1e376c08
#define SHA256_K50 0x2748774c
#define SHA256_K51 0x34b0bcb5
#define SHA256_K52 0x391c0cb3
#define SHA256_K53 0x4ed8aa4a
#define SHA256_K54 0x5b9cca4f
#define SHA256_K55 0x682e6ff3
#define SHA256_K56 0x748f82ee
#define SHA256_K57 0x78a5636f
#define SHA256_K58 0x84c87814
#define SHA256_K59 0x8cc70208
#define SHA256_K60 0x90befffa
#define SHA256_K61 0xa4506ceb
#define SHA256_K62 0xbef9a3f7
#define SHA256_K63 0xc67178f2

static const uint32_t sha256_constants[64] = {
    SHA256_K0, SHA256_K1, SHA256_K2, SHA256_K3, SHA256_K4, SHA256_K5, SHA256_K6, SHA256_K7,
    SHA256_K8, SHA256_K9, SHA256_K10, SHA256_K11, SHA256_K12, SHA256_K13, SHA256_K14, SHA256_K15,
    SHA256_K16, SHA256_K17, SHA256_K18, SHA256_K19, SHA256_K20, SHA256_K21, SHA256_K22, SHA256_K23,
    SHA256_K24, SHA256_K25, SHA256_K26, SHA256_K27, SHA256_K28, SHA256_K29, SHA256_K30, SHA256_K31,
    SHA256_K32, SHA256_K33, SHA256_K34, SHA256_K35, SHA256_K36, SHA256_K37, SHA256_K38, SHA256_K39,
    SHA256_K40, SHA256_K41, SHA256_K42, SHA256_K43, SHA256_K44, SHA256_K45, SHA256_K46, SHA256_K47,
    SHA256_K48, SHA256_K49, SHA256_K50, SHA256_K51, SHA256_K52, SHA256_K53, SHA256_K54, SHA256_K55,
    SHA256_K56, SHA256_K57, SHA256_K58, SHA256_K59, SHA256_K60, SHA256_K61, SHA256_K62, SHA256_K63
};

static uint32_t sha1_rotr(uint32_t value, uint32_t count) {
    return (value >> count) | (value << (32 - count));
}

static uint32_t sha1_ch(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (~x & z);
}

static uint32_t sha1_parity(uint32_t x, uint32_t y, uint32_t z) {
    return x ^ y ^ z;
}

static uint32_t sha1_maj(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

void sha1_init(Sha1Context* ctx) {
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xefcdab89;
    ctx->state[2] = 0x98badcfe;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xc3d2e1f0;
    ctx->bitcount = 0;
    memset(ctx->buffer, 0, 64);
}

void sha1_update(Sha1Context* ctx, const uint8_t* data, size_t len) {
    uint64_t bitlen = len * 8;
    uint64_t bitcount = ctx->bitcount;
    
    if (len >= 64) {
        size_t blocks = len / 64;
        for (size_t i = 0; i < blocks; i++) {
            memcpy(ctx->buffer, data + i * 64, 64);
            
            uint32_t w[80];
            for (int i = 0; i < 16; i++) {
                w[i] = (ctx->buffer[i*4] << 24) | (ctx->buffer[i*4+1] << 16) |
                       (ctx->buffer[i*4+2] << 8) | ctx->buffer[i*4+3];
            }
            
            for (int i = 16; i < 80; i++) {
                w[i] = sha1_rotr(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
            }
            
            uint32_t a = ctx->state[0];
            uint32_t b = ctx->state[1];
            uint32_t c = ctx->state[2];
            uint32_t d = ctx->state[3];
            uint32_t e = ctx->state[4];
            
            for (int i = 0; i < 80; i++) {
                uint32_t f, k;
                if (i < 20) {
                    f = sha1_ch(b, c, d);
                    k = SHA1_K0;
                } else if (i < 40) {
                    f = sha1_parity(b, c, d);
                    k = SHA1_K20;
                } else if (i < 60) {
                    f = sha1_maj(b, c, d);
                    k = SHA1_K40;
                } else {
                    f = sha1_parity(b, c, d);
                    k = SHA1_K60;
                }
                
                uint32_t temp = sha1_rotr(a, 5) + f + e + k + w[i];
                e = d;
                d = c;
                c = sha1_rotr(b, 30);
                b = a;
                a = temp;
            }
            
            ctx->state[0] += a;
            ctx->state[1] += b;
            ctx->state[2] += c;
            ctx->state[3] += d;
            ctx->state[4] += e;
        }
        
        data += blocks * 64;
        len -= blocks * 64;
    }
    
    memcpy(ctx->buffer, data, len);
    ctx->bitcount = bitcount + bitlen;
}

void sha1_final(Sha1Context* ctx, uint8_t hash[20]) {
    uint64_t bitcount = ctx->bitcount;
    size_t pad_len = (bitcount % 512) >= 448 ? 512 - (bitcount % 512) : 448 - (bitcount % 512);
    
    ctx->buffer[0] = 0x80;
    memset(ctx->buffer + 1, 0, pad_len - 1);
    
    for (int i = 0; i < 8; i++) {
        ctx->buffer[pad_len + i] = (bitcount >> (56 - i * 8)) & 0xFF;
    }
    
    sha1_update(ctx, ctx->buffer, pad_len + 8);
    
    for (int i = 0; i < 5; i++) {
        hash[i*4] = (ctx->state[i] >> 24) & 0xFF;
        hash[i*4+1] = (ctx->state[i] >> 16) & 0xFF;
        hash[i*4+2] = (ctx->state[i] >> 8) & 0xFF;
        hash[i*4+3] = ctx->state[i]) & 0xFF;
    }
}

static uint32_t sha256_rotr(uint32_t value, uint32_t count) {
    return (value >> count) | (value << (32 - count));
}

static uint32_t sha256_shr(uint32_t value, uint32_t count) {
    return value >> count;
}

static uint32_t sha256_sigma0(uint32_t x) {
    return sha256_rotr(x, 2) ^ sha256_rotr(x, 13) ^ sha256_rotr(x, 22);
}

static uint32_t sha256_sigma1(uint32_t x) {
    return sha256_rotr(x, 6) ^ sha256_rotr(x, 11) ^ sha256_rotr(x, 25);
}

static uint32_t sha256_gamma0(uint32_t x) {
    return sha256_rotr(x, 7) ^ sha256_rotr(x, 18) ^ sha256_shr(x, 3);
}

static uint32_t sha256_gamma1(uint32_t x) {
    return sha256_rotr(x, 17) ^ sha256_rotr(x, 19) ^ sha256_shr(x, 10);
}

static uint32_t sha256_ch(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (~x & z);
}

static uint32_t sha256_maj(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

void sha256_init(Sha256Context* ctx) {
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
    ctx->bitcount = 0;
    memset(ctx->buffer, 0, 128);
}

void sha256_update(Sha256Context* ctx, const uint8_t* data, size_t len) {
    uint64_t bitlen = len * 8;
    uint64_t bitcount = ctx->bitcount;
    
    if (len >= 128) {
        size_t blocks = len / 128;
        for (size_t i = 0; i < blocks; i++) {
            memcpy(ctx->buffer, data + i * 128, 128);
            
            uint32_t w[64];
            for (int i = 0; i < 16; i++) {
                w[i] = (ctx->buffer[i*4] << 24) | (ctx->buffer[i*4+1] << 16) |
                       (ctx->buffer[i*4+2] << 8) | ctx->buffer[i*4+3];
            }
            
            for (int i = 16; i < 64; i++) {
                w[i] = sha256_gamma1(w[i-2]) + w[i-7] + sha256_gamma0(w[i-15]) + w[i-16];
            }
            
            uint32_t a = ctx->state[0];
            uint32_t b = ctx->state[1];
            uint32_t c = ctx->state[2];
            uint32_t d = ctx->state[3];
            uint32_t e = ctx->state[4];
            uint32_t f = ctx->state[5];
            uint32_t g = ctx->state[6];
            uint32_t h = ctx->state[7];
            
            for (int i = 0; i < 64; i++) {
                uint32_t t1 = h + sha256_sigma1(e) + sha256_ch(e, f, g) + sha256_constants[i] + w[i];
                uint32_t t2 = sha256_sigma0(a) + sha256_maj(a, b, c);
                h = g;
                g = f;
                f = e;
                e = d + t1;
                d = c;
                c = b;
                b = a;
                a = t1 + t2;
            }
            
            ctx->state[0] += a;
            ctx->state[1] += b;
            ctx->state[2] += c;
            ctx->state[3] += d;
            ctx->state[4] += e;
            ctx->state[5] += f;
            ctx->state[6] += g;
            ctx->state[7] += h;
        }
        
        data += blocks * 128;
        len -= blocks * 128;
    }
    
    memcpy(ctx->buffer, data, len);
    ctx->bitcount = bitcount + bitlen;
}

void sha256_final(Sha256Context* ctx, uint8_t hash[32]) {
    uint64_t bitcount = ctx->bitcount;
    size_t pad_len = (bitcount % 1024) >= 896 ? 1024 - (bitcount % 1024) : 896 - (bitcount % 1024);
    
    ctx->buffer[0] = 0x80;
    memset(ctx->buffer + 1, 0, pad_len - 1);
    
    for (int i = 0; i < 8; i++) {
        ctx->buffer[pad_len + i] = (bitcount >> (56 - i * 8)) & 0xFF;
    }
    
    sha256_update(ctx, ctx->buffer, pad_len + 8);
    
    for (int i = 0; i < 8; i++) {
        hash[i*4] = (ctx->state[i] >> 24) & 0xFF;
        hash[i*4+1] = (ctx->state[i] >> 16) & 0xFF;
        hash[i*4+2] = (ctx->state[i] >> 8) & 0xFF;
        hash[i*4+3] = ctx->state[i]) & 0xFF;
    }
}

void crypto_hash_string(const char* input, uint8_t* output, size_t output_size) {
    if (output_size == SHA256_DIGEST_SIZE) {
        Sha256Context ctx;
        sha256_init(&ctx);
        sha256_update(&ctx, (const uint8_t*)input, strlen(input));
        sha256_final(&ctx, output);
    } else if (output_size == SHA1_DIGEST_SIZE) {
        Sha1Context ctx;
        sha1_init(&ctx);
        sha1_update(&ctx, (const uint8_t*)input, strlen(input));
        sha1_final(&ctx, output);
    }
}

int crypto_verify_hash(const uint8_t* hash1, const uint8_t* hash2, size_t size) {
    return memcmp(hash1, hash2, size) == 0;
}

void crypto_generate_random_bytes(uint8_t* buffer, size_t size) {
    static uint32_t seed = 0x12345678;
    
    for (size_t i = 0; i < size; i++) {
        seed = seed * 1103515245 + 12345;
        buffer[i] = (seed >> 16) & 0xFF;
    }
}
