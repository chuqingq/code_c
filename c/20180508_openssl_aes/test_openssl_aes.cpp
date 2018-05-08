
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/aes.h>



uint8_t *get_default_aes_key()
{
    static uint8_t *aeskey_array = NULL;
    if (!aeskey_array)
    {
        char *aeskey = getenv("XCHANNEL_KEY");
        if (aeskey == NULL || strlen(aeskey) != 32)
        {
            TLOGF("get_default_aes_key", "", "env XCHANNEL_KEY invalid, will exit.");
            exit(1);
        }
        aeskey_array = (uint8_t *)malloc(16); // 密钥长度是16B
        CHECK(aeskey_array != NULL, "aeskey_array malloc failed.");
        convert_string2int(aeskey, aeskey_array, 16);
    }

    return aeskey_array;
}




int aes_encrypt(const uint8_t *aes_input, const int input_len, uint8_t *enc_out, const uint8_t *aes_key, const int key_len, const uint8_t *aes_iv, const int iv_len)
{
    if (!aes_input || !enc_out || !aes_key || key_len > AES_BLOCK_SIZE)
    {
        return 0;
    }

    // padding aes key，保证其长度为默认的16个字节
    uint8_t key[AES_BLOCK_SIZE];
    xmemset(key, 0x00, AES_BLOCK_SIZE);
    xmemcpy(key, aes_key, key_len);
    // padding iv
    uint8_t iv[AES_BLOCK_SIZE];
    xmemset(iv, 0x00, AES_BLOCK_SIZE);
    xmemcpy(iv, aes_iv, iv_len);

    AES_KEY enc_key;
    if (AES_set_encrypt_key(key, AES_BLOCK_SIZE * 8, &enc_key) < 0)
    {
        return 0;
    }

    // PKCS7 Padding
    uint8_t pad_len = AES_BLOCK_SIZE - input_len % AES_BLOCK_SIZE;
    int input_len_pad = input_len + pad_len;
    uint8_t aes_input_pad[input_len_pad];
    xmemcpy(aes_input_pad, aes_input, input_len);
    xmemset(aes_input_pad + input_len, pad_len, pad_len);

    AES_cbc_encrypt(aes_input_pad, enc_out, input_len_pad, &enc_key, iv, AES_ENCRYPT);

    return input_len_pad;
}

int aes_decrypt(const uint8_t *aes_input, const int input_len, uint8_t *dec_out, const uint8_t *aes_key, const int key_len, const uint8_t *aes_iv, const int iv_len)
{
    if (!aes_input || !dec_out || !aes_key || key_len > AES_BLOCK_SIZE || input_len % AES_BLOCK_SIZE != 0)
    {
        return 0;
    }

    // padding aes key，保证其长度为默认的16个字节
    uint8_t key[AES_BLOCK_SIZE];
    xmemset(key, 0x00, AES_BLOCK_SIZE);
    xmemcpy(key, aes_key, key_len);
    // padding iv
    uint8_t iv[AES_BLOCK_SIZE];
    xmemset(iv, 0x00, AES_BLOCK_SIZE);
    xmemcpy(iv, aes_iv, iv_len);

    AES_KEY dec_key;
    if (AES_set_decrypt_key(key, AES_BLOCK_SIZE * 8, &dec_key))
    {
        return 0;
    }
    AES_cbc_encrypt(aes_input, dec_out, input_len, &dec_key, iv, AES_DECRYPT);

    // PKCS7 Unpadding
    return input_len - (int)dec_out[input_len - 1];
}



