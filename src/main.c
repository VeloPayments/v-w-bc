#include <emscripten/emscripten.h>
#include <stdio.h>
#include <vccrypt/suite.h>
#include <vpr/allocator/malloc_allocator.h>
#include <string.h>

#define ASSERT_EQ(L, R) \
    if (L != R) { \
        printf("SHITS FUCKED YO.\n"); \
        return -1; \
    } \

EMSCRIPTEN_KEEPALIVE
int magic() {
    allocator_options_t alloc_opts;
    vccrypt_suite_options_t options;
    vccrypt_suite_register_velo_v1();

    malloc_allocator_options_init(&alloc_opts);

    vccrypt_suite_options_init(&options, &alloc_opts, VCCRYPT_SUITE_VELO_V1);

    
    const uint8_t message[] = "foo suite bar baz";
    vccrypt_digital_signature_context_t context;

    //create a buffer for the private key
    vccrypt_buffer_t priv;
    ASSERT_EQ(0,
        vccrypt_suite_buffer_init_for_signature_private_key(&options, &priv));
    ASSERT_EQ(64U, priv.size);

    //create a buffer for the public key
    vccrypt_buffer_t pub;
    ASSERT_EQ(0,
        vccrypt_suite_buffer_init_for_signature_public_key(&options, &pub));
    ASSERT_EQ(32U, pub.size);

    //create a buffer for the signature
    vccrypt_buffer_t signature;
    ASSERT_EQ(0,
        vccrypt_suite_buffer_init_for_signature(&options, &signature));
    ASSERT_EQ(64U, signature.size);

    //create the digital signature context
    ASSERT_EQ(0, vccrypt_suite_digital_signature_init(&options, &context));

    //generate a keypair
    ASSERT_EQ(0,
        vccrypt_digital_signature_keypair_create(&context, &priv, &pub));

    uint8_t* buf = (uint8_t*)pub.data;
    printf("pub = ");
    for (int i = 0; i < 32; ++i)
    {
        printf("%02x",buf[i]);
    }
    printf("\n");

    //sign the message
        vccrypt_digital_signature_sign(
                &context, &signature, &priv,
                message, strlen(message));

    uint8_t* buf2 = (uint8_t*)signature.data;
    printf("sig = ");
    for (int i = 0; i < 64; ++i) {
        printf("%02x", buf2[i]);
    }
    printf("\n");


    //verify the signature
        vccrypt_digital_signature_verify(
                &context, &signature, &pub,
                message, sizeof(message));

    //dispose the digital signature context
    dispose((disposable_t*)&context);

    //dispose all buffers
    dispose((disposable_t*)&priv);
    dispose((disposable_t*)&pub);
    dispose((disposable_t*)&signature);
    return 0xCAFE;
}
