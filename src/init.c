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
vccert_parser_options_t parser_opts;

static bool dummy_txn_resolver(void *a, void *b, const uint8_t *c, const uint8_t *d, vccrypt_buffer_t *e, bool *f)
{
    return false;
};

static int32_t dummy_artifact_state_resolver(void *a, void *b, const uint8_t *c, vccrypt_buffer_t *d)
{
    return -1;
}

static bool dummy_entity_key_resolver(void *a, void *b, uint64_t c, const uint8_t *d, vccrypt_buffer_t *e,
                                      vccrypt_buffer_t *f)
{
    return false;
}

static vccert_contract_fn_t dummy_contract_resolver(void *a, void *b, const uint8_t *c, const uint8_t *d)
{
    return NULL;
}


EMSCRIPTEN_KEEPALIVE
int vwblockchain_init()
{
    if (vjblockchain_initialized)
    {
        JS_THROW("vwblockchain already initialized.")
        return -1;
    }

    vccrypt_suite_register_velo_v1();
    vccrypt_stream_register_AES_256_2X_CTR();
    vccrypt_block_register_AES_256_2X_CBC();

    malloc_allocator_options_init(&alloc_opts);

    int dummy_context;

    INIT_OR_FAIL(
        "vccrypt suite options",
        (vccrypt_suite_options_init(&crypto_suite, &alloc_opts, VCCRYPT_SUITE_VELO_V1))
    )

    INIT_OR_FAIL(
        "vccert options",
        (vccert_parser_options_init(&parser_opts, &alloc_opts, &crypto_suite, &dummy_txn_resolver,
                                    &dummy_artifact_state_resolver, &dummy_contract_resolver,
                                    &dummy_entity_key_resolver, &dummy_context))
    )

    vjblockchain_initialized = true;
    return 0;
}

