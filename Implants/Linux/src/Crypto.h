
#include <unistd.h>
#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/evp.h>

#ifndef CRYPTO_H
#define CRYPTO_H

#define BUFSIZE 1024

namespace Crypto {

    const std::string base64_chars =
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";

    std::string base64_encode(unsigned char * bytes_to_encode, unsigned int in_len) {

        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);
            if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
            }
        }

        if (i)
        {
            for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

            while((i++ < 3))
            ret += '=';

        }

        return ret;
    }

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