#include <emscripten/emscripten.h>
#include <vccrypt/buffer.h>
#include <vccert/parser.h>
#include <vccert/fields.h>
#include <vwblockchain/init.h>
#include <vwblockchain/error.h>
#include <string.h>
#include <stdio.h>
#include <vccrypt/compare.h>

vccrypt_buffer_t *match_fragment(vccrypt_buffer_t *uuid, uint8_t *value, size_t size)
{
    vccert_parser_context_t fragment_parser;
    result = vccert_parser_init(&parser_opts, &fragment_parser, value, size);
    if (result != 0)
    {
        JS_THROW("vccert_parser_init: fragment");
    }

    const uint8_t *uuid_value;
    size_t uuid_size;
    vccert_parser_find_short(
        &parser, VCCERT_FIELD_TYPE_VELO_ENCRYPTED_SHARED_SECRET_ENTITY_UUID, &uuid_value, &uuid_size
    );

    int match = crypto_memcmp(uuid->data, uuid_value, 16);
    if (0 != match)
    {
        return NULL;
    }

    const uint8_t *secret_value;
    size_t secret_size;
    vccert_parser_find_short(
        &parser, VCCERT_FIELD_TYPE_VELO_ENCRYPTED_SHARED_SECRET_FRAGMENT, &secret_value, &secret_size
    );

    vccrypt_buffer_t buf = malloc(sizeof(vccrypt_buffer_t));
    vccrypt_buffer_init(buf, &alloc_opts, secret_size);
    memcpy(buf.data, secret_value, secret_size);

    return buf;
}

EMSCRIPTEN_KEEPALIVE
vccrypt_buffer_t *vwblockchain_find_shared_secret(vccrypt_buffer_t *cert, vccrypt_buffer_t *uuid)
{
    int result;
    vccert_parser_context_t parser;
    result = vccert_parser_init(&parser_opts, &parser, cert->data, cert->size);
    if (result != 0)
    {
        JS_THROW("vccert_parser_init");
    }

    const uint8_t *value;
    size_t size;
    vccert_parser_find_short(&parser, VCCERT_FIELD_TYPE_VELO_ENCRYPTED_SHARED_SECRET_FRAGMENT, &value, &size);

    vccrypt_buffer_t *buf;
    buf = match_fragment(uuid, value, size);
    if (buf != NULL)
    {
        return buf;
    }

    do
    {
        result = vccert_parser_find_short(
            &parser,
            VCCERT_FIELD_TYPE_VELO_ENCRYPTED_SHARED_SECRET_FRAGMENT,
            &value,
            &size
        );

        buf = match_fragment(uuid, value, size);
        if (buf != NULL)
        {
            return buf;
        }

    } while (result != 0);

    return NULL;
}

EMSCRIPTEN_KEEPALIVE
vccrypt_buffer_t *vwblockchain_actual_uncook_uuid(char *raw)
{
    vccrypt_buffer_t src;
    vccrypt_buffer_init(&src, &alloc_opts, 32);

    uint8_t *buf = (uint8_t *) src.data;
    int counter = 0;
    while (*raw && counter < 16)
    {
        if (*raw != '-')
        {
            buf[counter] = (uint8_t) *raw;
            ++counter;
        }
        ++raw;
    }

    if (counter != 16)
    {
        JS_THROW("Counter wasn't 16");
    }

    vccrypt_buffer_t *dest = malloc(sizeof(vccrypt_buffer_t));
    vccrypt_buffer_init(dest, &alloc_opts, 16);

    int result = vccrypt_buffer_read_hex(dest, &src);
    if (result != 0)
    {
        JS_THROW("vccrypt_buffer_read_hex");
    }

    dispose((disposable_t *) &src);

    return dest;
}

EMSCRIPTEN_KEEPALIVE
vccrypt_buffer_t *vwblockchain_actual_vccrypt_buffer_from_base64(char *raw)
{
    size_t read;
    size_t len = strlen(raw);
    size_t buffer_size = (len * 3) / 4;
    if (buffer_size % 3)
    {
        buffer_size += 3 - buffer_size % 3;
    }

    // Setup the src buffer.
    vccrypt_buffer_t src;
    vccrypt_buffer_init(&src, &alloc_opts, len);
    memcpy(src.data, raw, len);

    // Setup the dest
    vccrypt_buffer_t *dest = malloc(sizeof(vccrypt_buffer_t));
    vccrypt_buffer_init(dest, &alloc_opts, buffer_size);

    int result = vccrypt_buffer_read_base64(dest, &src, &read);
    if (result != 0)
    {
        JS_THROW("vccrypt_buffer_read_base64");
    }
    dispose((disposable_t *) &src);

    return dest;
}

EM_JS(vccrypt_buffer_t*, vwblockchain_vccrypt_buffer_from_base64, (char* raw), {
    var ptr = allocate(intArrayFromString(raw), 'i8', ALLOC_NORMAL);
    var out = _vwblockchain_actual_vccrypt_buffer_from_base64(ptr);
    _free(ptr);
    return out;
});

EM_JS(vccrypt_buffer_t*, vwblockchain_uncook_uuid, (char* raw), {
    var ptr = allocate(intArrayFromString(raw), 'i8', ALLOC_NORMAL);
    var out = _vwblockchain_actual_uncook_uuid(ptr);
    _free(ptr);
    return out;
});


