#include <emscripten/emscripten.h>
#include <vccrypt/buffer.h>
#include <vccert/parser.h>
#include <vccert/fields.h>
#include <vwblockchain/init.h>
#include <vwblockchain/error.h>
#include <string.h>
#include <vccrypt/compare.h>
#include <vccrypt/key_agreement.h>
#include <vccrypt/block_cipher.h>
#include <vccrypt/stream_cipher.h>
#include <vccert/error_codes.h>

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

EM_JS(char*, vwblockchain_buffer_to_string, (vccrypt_buffer_t * buf), {
    var out = _vccrypt_buffer_to_string(buf);
    return Pointer_stringify(out);
});


EMSCRIPTEN_KEEPALIVE
char *vccrypt_buffer_to_string(vccrypt_buffer_t *buf)
{
    char *str = malloc(buf->size + 1);
    strcpy(str, (const char *) buf->data);
    return str;
}

EMSCRIPTEN_KEEPALIVE
vccrypt_buffer_t *match_fragment(vccrypt_buffer_t *uuid, const uint8_t *value, size_t size)
{
    int result;
    vccert_parser_context_t fragment_parser;
    result = vccert_parser_init(&parser_opts, &fragment_parser, value, size);
    if (result != VCCERT_STATUS_SUCCESS)
    {
        JS_THROW("vccert_parser_init: fragment", result);
    }

    const uint8_t *uuid_value;
    size_t uuid_size;
    result = vccert_parser_find_short(
        &fragment_parser, VCCERT_FIELD_TYPE_VELO_ENCRYPTED_SHARED_SECRET_ENTITY_UUID, &uuid_value, &uuid_size
    );
    if (result != VCCERT_STATUS_SUCCESS)
    {
        JS_THROW("vccert_parser_find_short", result);
    }

    int match = crypto_memcmp(uuid->data, uuid_value, 16);
    if (0 != match)
    {
        return NULL;
    }

    const uint8_t *secret_value;
    size_t secret_size;
    vccert_parser_find_short(
        &fragment_parser, VCCERT_FIELD_TYPE_VELO_ENCRYPTED_SHARED_SECRET_KEY, &secret_value, &secret_size
    );

    vccrypt_buffer_t *buf = malloc(sizeof(vccrypt_buffer_t));
    vccrypt_buffer_init(buf, &alloc_opts, secret_size);
    memcpy(buf->data, secret_value, secret_size);
    return buf;
}

EMSCRIPTEN_KEEPALIVE
vccrypt_buffer_t *vwblockchain_get_field(vccrypt_buffer_t *cert, int field)
{
    int result;
    vccert_parser_context_t parser;
    result = vccert_parser_init(&parser_opts, &parser, cert->data, cert->size);
    if (result != 0)
    {
        JS_THROW("vccert_parser_init", result);
    }

    const uint8_t *value;
    size_t size;
    result = vccert_parser_find_short(&parser, (uint16_t) field, &value, &size);
    if (result != 0)
    {
        JS_THROW("vccert_parser_find_short", result);
    }

    vccrypt_buffer_t *buf = malloc(sizeof(vccrypt_buffer_t));
    vccrypt_buffer_init(buf, &alloc_opts, size);
    memcpy(buf->data, value, size);
    return buf;
}

EMSCRIPTEN_KEEPALIVE
vccrypt_buffer_t *vwblockchain_decrypt_field(vccrypt_buffer_t *secret, vccrypt_buffer_t *field)
{
    int result;

    vccrypt_stream_options_t stream_options;
    vccrypt_stream_options_init(&stream_options, &alloc_opts, VCCRYPT_STREAM_ALGORITHM_AES_256_2X_CTR);

    vccrypt_stream_context_t stream;
    vccrypt_stream_init(&stream_options, &stream, secret);

    const uint8_t *in = (const uint8_t *) field->data;
    size_t offset = 0;
    size_t input_offset = 0;
    size_t input_size = field->size;
    size_t output_size = input_size - stream_options.IV_size;

    vccrypt_buffer_t *buf = malloc(sizeof(vccrypt_buffer_t));
    vccrypt_buffer_init(buf, &alloc_opts, output_size);

    uint8_t *out = (uint8_t *) buf->data;

    result = vccrypt_stream_start_decryption(&stream, in, &input_offset);
    if (result != 0)
    {
        JS_THROW("vccrypt_stream_start_decryption", result);
    }

    result = vccrypt_stream_decrypt(&stream, in + input_offset, input_size - input_offset, out, &offset);
    if (result != 0)
    {
        JS_THROW("vccrypt_stream_decrypt", result);
    }

    return buf;
}

EMSCRIPTEN_KEEPALIVE
vccrypt_buffer_t *vwblockchain_decrypt_fragment(
    vccrypt_buffer_t *public_key, vccrypt_buffer_t *private_key, vccrypt_buffer_t *fragment)
{

    int result;
    vccrypt_buffer_t key_buffer;
    vccrypt_block_context_t block;
    vccrypt_block_options_t block_opts;
    vccrypt_key_agreement_context_t context;

    vccrypt_block_options_init(&block_opts, &alloc_opts, VCCRYPT_BLOCK_ALGORITHM_AES_256_2X_CBC);

    vccrypt_suite_buffer_init_for_cipher_key_agreement_shared_secret(&crypto_suite, &key_buffer);
    vccrypt_suite_cipher_key_agreement_init(&crypto_suite, &context);

    result = vccrypt_key_agreement_long_term_secret_create(
        &context, private_key, public_key, &key_buffer
    );

    if (result != 0)
    {
        JS_THROW("vccrypt_key_agreement_long_term_secret_create", result);
    }

    result = vccrypt_block_init(&block_opts, &block, &key_buffer, false);
    if (result != 0)
    {
        JS_THROW("vccrypt_block_init", result);
    }

    if (fragment->size != 48)
    {
        JS_THROW("Fragment too small.", fragment->size);
    }

    const uint8_t *key_data = (const uint8_t *) fragment->data;


    vccrypt_buffer_t *buf = malloc(sizeof(vccrypt_buffer_t));
    vccrypt_buffer_init(buf, &alloc_opts, 32);

    uint8_t *out = (uint8_t *) buf->data;

    result = vccrypt_block_decrypt(&block, key_data, key_data + 16, out);
    if (result != 0)
    {
        JS_THROW("vccrypt_block_decrypt: first", result);
    }
    result = vccrypt_block_decrypt(&block, key_data + 16, key_data + 32, out + 16);
    if (result != 0)
    {
        JS_THROW("vccrypt_block_decrypt: second", result);
    }

    return buf;
}

EMSCRIPTEN_KEEPALIVE
vccrypt_buffer_t *vwblockchain_find_shared_secret(vccrypt_buffer_t *cert, vccrypt_buffer_t *uuid)
{
    int result;
    vccert_parser_context_t parser;
    result = vccert_parser_init(&parser_opts, &parser, cert->data, cert->size);
    if (result != VCCERT_STATUS_SUCCESS)
    {
        JS_THROW("vccert_parser_init", result);
    }

    const uint8_t *value;
    size_t size;
    result = vccert_parser_find_short(&parser, VCCERT_FIELD_TYPE_VELO_ENCRYPTED_SHARED_SECRET_FRAGMENT, &value, &size);
    if (result != VCCERT_STATUS_SUCCESS)
    {
        JS_THROW("vccert_parser_find_short", result);
    }

    vccrypt_buffer_t *buf = match_fragment(uuid, value, size);
    if (buf != NULL)
    {
        return buf;
    }

    do
    {
        result = vccert_parser_find_next(
            &parser,
            &value,
            &size
        );

        buf = match_fragment(uuid, value, size);
        if (buf != NULL)
        {
            return buf;
        }

    } while (result != 0);


    JS_THROW("Didn't find the UUID in any fragment.", result);
    return NULL;
}

EMSCRIPTEN_KEEPALIVE
vccrypt_buffer_t *vwblockchain_actual_uncook_uuid(char *raw)
{
    vccrypt_buffer_t src;
    vccrypt_buffer_init(&src, &alloc_opts, 32);

    uint8_t *buf = (uint8_t *) src.data;
    int counter = 0;
    while (*raw && counter < 32)
    {
        if (*raw != '-')
        {
            buf[counter] = (uint8_t) *raw;
            ++counter;
        }
        ++raw;
    }

    if (counter != 32)
    {
        JS_THROW("Counter wasn't 32", counter);
    }

    vccrypt_buffer_t *dest = malloc(sizeof(vccrypt_buffer_t));
    vccrypt_buffer_init(dest, &alloc_opts, 16);

    int result = vccrypt_buffer_read_hex(dest, &src);
    if (result != 0)
    {
        JS_THROW("vccrypt_buffer_read_hex", result);
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
        JS_THROW("vccrypt_buffer_read_base64", result);
    }
    dispose((disposable_t *) &src);

    return dest;
}


