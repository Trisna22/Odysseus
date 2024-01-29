
#include <unistd.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#ifndef CRYPTO_H
#define CRYPTO_H

#define BUFSIZE 1024

namespace Crypto {

    /**
     * @brief WARNING Future me,
     * Rewrite whole decryption, I adjusted it to have it work.
     * Use code from the LockFile program.
     */
    bool decryptObject(FILE* inputFile, int outputFile, off64_t sizeInput) {

        // printf("Decrypting %d bytes of data...\n", *sizeInput);
        // int cipher_block_size = EVP_CIPHER_block_size(EVP_aes_256_cbc_hmac_sha256());
        unsigned char in_buf[BUFSIZE], out_buf[BUFSIZE];// + cipher_block_size];

        int num_bytes_read, out_len;
        off64_t counter = sizeInput;
        while (counter != 0) {
            
            // Read data in buffer.
            if (counter < BUFSIZE)
                    num_bytes_read = fread(in_buf, sizeof(unsigned char), counter, inputFile);
                else 
                    num_bytes_read = fread(in_buf, sizeof(unsigned char), BUFSIZE, inputFile);


            // Process data (decryption)
            // EVP_Decrypt(in_buf, size, ...);

            write(outputFile, in_buf, num_bytes_read);

            counter -= num_bytes_read; // We want to count the encrypted bytes, not the output bytes.
            if (counter <= 0)
                break;
        }

        return true;
    }
}

#endif