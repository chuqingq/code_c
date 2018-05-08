// sudo apt install libssl-dev
// g++ test_openssl_base64.cpp -lcrypto
#include <stdio.h>  
#include <string.h>  
#include <unistd.h>  
  
#include <openssl/pem.h> // buf_mem_st 
#include <openssl/bio.h> // bio_xx
#include <openssl/evp.h> // bio_f_base64
  
int base64_encode(char *in_str, int in_len, char *out_str)  
{  
    BIO *b64, *bio;  
    BUF_MEM *bptr = NULL;  
    size_t size = 0;  
  
    if (in_str == NULL || out_str == NULL)  
        return -1;  
  
    b64 = BIO_new(BIO_f_base64());  
    bio = BIO_new(BIO_s_mem());  
    bio = BIO_push(b64, bio);  
  
    BIO_write(bio, in_str, in_len);  
    BIO_flush(bio);  
  
    BIO_get_mem_ptr(bio, &bptr);  
    memcpy(out_str, bptr->data, bptr->length);  
    out_str[bptr->length] = '\0';  
    size = bptr->length;  
  
    BIO_free_all(bio);  
    return size;  
}  
  
int base64_decode(char *in_str, int in_len, char *out_str)  
{  
    BIO *b64, *bio;  
    BUF_MEM *bptr = NULL;  
    int counts;  
    int size = 0;  
  
    if (in_str == NULL || out_str == NULL)  
        return -1;  
  
    b64 = BIO_new(BIO_f_base64());  
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);  
  
    bio = BIO_new_mem_buf(in_str, in_len);  
    bio = BIO_push(b64, bio);  
  
    size = BIO_read(bio, out_str, in_len);  
    out_str[size] = '\0';  
  
    BIO_free_all(bio);  
    return size;  
}  
  
int main()  
{  
    char instr[] = "hello";  
    char outstr1[1024] = {0};  
    base64_encode(instr,5,outstr1);  
    printf("base64:%s\n",outstr1);  
  
    // outstr1[1] = 'a';
    char outstr2[6] = {0};  
    base64_decode(outstr1,strlen(outstr1)+1, outstr2);  
    printf("str:%s\n",outstr2);  
    return 0;  
}  
