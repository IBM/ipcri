Author: Danny Tsen <dtsen@us.ibm.com>

This README file documents how to use the POWER10 version of AES/GCM stitched reference code.

The POWER10 AES/GCM stitched reference code  is contained in POWER10.s for ppc64le.  This code implements
the AES/GCM stitched design and doesn't include all the neccessary init file for creating AES round keys,
hash table and the final hash tag generation.

To use this code, refer to OpenSSL for the full path for a complete design.  Here, we outline some of the
neccesary init function required for a full implementation of authenticated AES/GCM algorithm.

First, for some init components, you will need to,

1. Compute round keys depending on key size (128, 192, or 256 bits).
2. Generate a Hash subkey to encrypt a null string block with the round keys just created.
3. Set inital vector (IV).
     - encrypt first IV which will be used for later final tag (IV tag).
     - increment IV, this IV will be used for encryption/decryption.
4. Init Hash table with the subkey just created.
5. If there is AAD, hash the AAD string.
     - AAD should be padded to full mutiple 16 bytes block if needed.
     - Hash all complete blocks (16 bytes) and xor the remaining bytes to the hash output just created. Then,
       hash the xor output.
       This will be the starting Xi value for encryption/decryption.
6. Call the POWER10 AES/GCM function which will generate a encrypt/decrypt output and a tag.
     int aes_p10_gcm_encrypt (const void *inp, void *out, size_t len, const char *rk, unsigned char iv[16], void *Xip);
       inp - input text
       out - ouptput (encrypted)
       len - text length
       rk - round keys
       iv - IV
       Xip - Xi, HPoli, hash keys
     - The function returns how many bytes are processed as a function return.
     - The calculated hash was return in Xi.
7. To generate the final authenticated tag.
     - Hash the AAD length and text length in bits with the Xi.  
       ( 64 bits AAD len and 64 bits text length, this comprise a full 16 bytes block)..
     - Xor the previous hash results with the IV tag generated in step 3.

