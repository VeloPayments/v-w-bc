#include <emscripten/emscripten.h>
#include <stdbool.h>
#include <vwblockchain/error.h>
#include <vccrypt/suite.h>
#include <vccrypt/stream_cipher.h>
#include <vccert/builder.h>
#include <vccrypt/block_cipher.h>
#include <vpr/allocator/malloc_allocator.h>
#include <vwblockchain/init.h>

volatile bool vjblockchain_initialized = false;
allocator_options_t alloc_opts;
vccrypt_suite_options_t crypto_suite;
vccert_builder_options_t builder_opts;

EMSCRIPTEN_KEEPALIVE
int vwblockchain_init() {
    if (vjblockchain_initialized) {
        JS_THROW("vwblockchain already initialized.")
        return -1;
    }

    vccrypt_suite_register_velo_v1();
    vccrypt_stream_register_AES_256_2X_CTR();
    vccrypt_block_register_AES_256_2X_CBC();

    malloc_allocator_options_init(&alloc_opts);

    INIT_OR_FAIL(
        "vccrypt suite options",
        (vccrypt_suite_options_init(&crypto_suite, &alloc_opts, VCCRYPT_SUITE_VELO_V1))
    )

    INIT_OR_FAIL(
        "vccert builder options",
        (vccert_builder_options_init(&builder_opts, &alloc_opts, &crypto_suite))
    )

    vjblockchain_initialized = true;
    return 0;
}