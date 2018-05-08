#include <openssl/ssl.h>
#include <openssl/rsa.h>


// #define RSA_PUBKEY_BUFSIZE    600
#define RSA_PUBKEY_BUFSIZE 2048
#define RSA_KEY_LENPERLINE 64 // RSA 公私钥每行Base64字符数

RSA **private_rsas = NULL;
uint32_t RSA_SIZE = 0;
static RSA *create_rsa(uint8_t *key, int key_type);


static int wrapper_rsa_public_key(uint8_t *key, int key_len, uint8_t *wrapper_key);


#define RSA_KEY_TYPE_PUBLIC 1
#define RSA_KEY_TYPE_PRIVATE 0

int rsa_public_encrypt(const uint8_t *rsa_input, const int input_len, uint8_t *enc_out, uint8_t *public_key, const int key_len)
{
    uint8_t public_key_wrapper[RSA_PUBKEY_BUFSIZE] = {0};
    int wrapper_len = wrapper_rsa_public_key(public_key, key_len, public_key_wrapper);
    if (wrapper_len >= RSA_PUBKEY_BUFSIZE)
    {
        char pkstr[1024]; // 存储二进制公钥（294Byte），打印字符串为294*2
        convert_int2string(public_key, key_len, pkstr, sizeof(pkstr));
        TLOGE("rsa_public_encrypt", "", "Failed to wrapper_rsa_public_key. wrapper_len: %d, max_len: %d, pk: %s",
              wrapper_len, RSA_PUBKEY_BUFSIZE, pkstr);
        return 0;
    }

    RSA *rsa = create_rsa(public_key_wrapper, RSA_KEY_TYPE_PUBLIC);
    if (rsa == NULL)
    {
        char pkstr[1024]; // 存储二进制公钥（294Byte），打印字符串为294*2
        convert_int2string(public_key, key_len, pkstr, sizeof(pkstr));
        TLOGE("rsa_public_encrypt", "", "Failed to create RSA. pk: %s", pkstr);
        return 0;
    }

    uint32_t rsa_size = RSA_size(rsa);
    if (input_len >= rsa_size - 41) // less than RSA_size(rsa) - 41 for RSA_PKCS1_OAEP_PADDING
    {
        TLOGE("rsa_public_encrypt", "", "data too long to encrypt, must be less than %d.", RSA_size(rsa) - 41);
        RSA_free(rsa);
        return 0;
    }

    int ret = RSA_public_encrypt(input_len, rsa_input, enc_out, rsa, RSA_PKCS1_OAEP_PADDING);
    if (ret <= 0)
    {
        TLOGE("rsa_public_encrypt", "", "Failed to encrypt with RSA public key.");
        RSA_free(rsa);
        return 0;
    }
    RSA_free(rsa);
    return ret;
}

int rsa_private_decrypt_raw(const uint8_t *rsa_input, const int input_len, uint8_t *dec_out, uint8_t *private_key)
{
    RSA *rsa = create_rsa(private_key, RSA_KEY_TYPE_PRIVATE);
    if (rsa == NULL)
    {
        TLOGE("rsa_private_decrypt_raw", "", "Failed to create RSA.");
        return 0;
    }

    if (input_len != RSA_size(rsa))
    {
        TLOGE("rsa_private_decrypt_raw", "", "length of data to decrypt is not equal to %d.", RSA_size(rsa));
        RSA_free(rsa);
        return 0;
    }

    int ret = RSA_private_decrypt(input_len, rsa_input, dec_out, rsa, RSA_PKCS1_OAEP_PADDING);
    if (ret <= 0)
    {
        TLOGE("rsa_private_decrypt_raw", "", "Failed to decrypt with RSA private key.");
        RSA_free(rsa);
        return 0;
    }
    RSA_free(rsa);
    return ret;
}

int rsa_private_decrypt_default(const uint8_t *rsa_input, const int input_len, uint8_t *dec_out)
{
    if (input_len != RSA_SIZE)
    {
        TLOGE("rsa_private_decrypt_default", "", "lenth of data to decrypt is not equal to %d.", RSA_SIZE);
        return 0;
    }

    RSA *private_rsa = private_rsas[get_core_id()];

    int ret = RSA_private_decrypt(input_len, rsa_input, dec_out, private_rsa, RSA_PKCS1_OAEP_PADDING);
    if (ret <= 0)
    {
        TLOGE("rsa_private_decrypt_default", "", "Failed to decrypt with RSA private key.");
        return 0;
    }
    return ret;
}

int set_private_rsa(uint8_t *key)
{
    // 初始化 RSA *数组
    private_rsas = (RSA **)malloc((g_config.core_num + 1) * sizeof(RSA *));
    if (private_rsas == NULL)
    {
        TLOGE("set_private_rsa", "", "private_rsas malloc NULL.");
        return COMMON_FAIL;
    }
    int i;
    for (i = 0; i <= g_config.core_num; i++)
    {
        private_rsas[i] = create_rsa(key, RSA_KEY_TYPE_PRIVATE);
        if (private_rsas[i] == NULL)
        {
            TLOGE("set_private_rsa", "", "Failed to create RSA for channel rsa private key.");
            return COMMON_FAIL;
        }
    }
    RSA_SIZE = RSA_size(private_rsas[0]);
    return COMMON_SUCCESS;
}

uint32_t get_rsa_size()
{
    return RSA_SIZE;
}

static RSA *create_rsa(uint8_t *key, int key_type)
{
    RSA *rsa = NULL;
    BIO *keybio;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio == NULL)
    {
        TLOGE("create_rsa", "", "Failed to create key BIO.");
        return NULL;
    }

    if (key_type == RSA_KEY_TYPE_PUBLIC)
    {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
    }
    else
    {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
    }
    BIO_free(keybio);
    return rsa;
}

static int wrapper_rsa_public_key(uint8_t *key, int key_len, uint8_t *wrapper_key)
{
    uint8_t temp[RSA_PUBKEY_BUFSIZE] = {0};
    uint8_t head[] = "-----BEGIN PUBLIC KEY-----\n";
    uint8_t tail[] = "-----END PUBLIC KEY-----\n";

    uint8_t *p_wapper = wrapper_key;
    uint8_t *p_temp = temp;

    size_t base64_len;
    base64_encode((char *)key, key_len, (char *)temp, &base64_len, 0);

    xmemcpy(p_wapper, head, strlen((const char *)head));
    p_wapper += strlen((const char *)head);

    int times = base64_len / RSA_KEY_LENPERLINE;
    int ret_char_cnt = times;
    int i;
    for (i = 0; i < times; i++)
    {
        xmemcpy(p_wapper, p_temp, RSA_KEY_LENPERLINE);
        p_wapper += RSA_KEY_LENPERLINE;
        p_temp += RSA_KEY_LENPERLINE;
        xmemset(p_wapper, '\n', 1);
        p_wapper++;
    }
    if (base64_len % RSA_KEY_LENPERLINE > 0)
    {
        xmemcpy(p_wapper, p_temp, base64_len % RSA_KEY_LENPERLINE);
        p_wapper += base64_len % RSA_KEY_LENPERLINE;
        xmemset(p_wapper, '\n', 1);
        p_wapper++;
        ret_char_cnt++;
    }
    xmemcpy(p_wapper, tail, strlen((const char *)tail));

    return strlen((const char *)head) + base64_len + ret_char_cnt + strlen((const char *)tail);
}

