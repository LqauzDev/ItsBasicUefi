#include "uefi_lib.h"
#include "../../Uefi.StdLib/include/stdio.h"
#include "../../Uefi.StdLib/include/stdlib.h"
#include "../../Uefi.Crypto/include/crypto.h"

EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
    uefi_init(SystemTable);
    
    printf("Cryptography Demo\r\n");
    printf("==================\r\n\r\n");
    
    const char* test_string = "Hello, UEFI Crypto!";
    printf("Testing string: %s\r\n", test_string);
    
    uint8_t sha1_hash[SHA1_DIGEST_SIZE];
    sha1_hash_string(test_string, sha1_hash);
    
    printf("SHA1: ");
    for (int i = 0; i < SHA1_DIGEST_SIZE; i++) {
        printf("%02x", sha1_hash[i]);
    }
    printf("\r\n");
    
    uint8_t sha256_hash[SHA256_DIGEST_SIZE];
    sha256_hash_string(test_string, sha256_hash);
    
    printf("SHA256: ");
    for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
        printf("%02x", sha256_hash[i]);
    }
    printf("\r\n");
    
    uint8_t random_bytes[16];
    crypto_generate_random_bytes(random_bytes, 16);
    
    printf("Random bytes: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", random_bytes[i]);
    }
    printf("\r\n");
    
    uint8_t hash_copy1[SHA256_DIGEST_SIZE];
    uint8_t hash_copy2[SHA256_DIGEST_SIZE];
    sha256_hash_string(test_string, hash_copy1);
    sha256_hash_string(test_string, hash_copy2);
    
    printf("Hash verification: %s\r\n", 
           crypto_verify_hash(hash_copy1, hash_copy2, SHA256_DIGEST_SIZE) ? "PASS" : "FAIL");
    
    printf("\nPress any key to exit...\r\n");
    uefi_wait_for_key();
    
    return EFI_SUCCESS;
}
