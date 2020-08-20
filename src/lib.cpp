#include <emscripten/bind.h>
#include <emscripten/em_asm.h>
#include <iostream>
#include <vwbc/init.h>
#include <vwbc/builder.h>
#include <vwbc/fields.h>
#include <vwbc/reader.h>
#include <vwbc/error.h>

using namespace emscripten;

allocator_options_t allocator_options;
vccert_builder_options_t cert_builder_options;
vccrypt_suite_options_t crypt_options;
vccert_parser_options_t parser_options;

static bool dummy_txn_resolver(
        void *, void *, const uint8_t *, const uint8_t *,
        vccrypt_buffer_t *, bool *)
{
    return false;
}

static int32_t dummy_artifact_state_resolver(
        void *, void *, const uint8_t *, vccrypt_buffer_t *)
{
    return -1;
}

static bool dummy_entity_key_resolver(
        void *, void *, uint64_t, const uint8_t *, vccrypt_buffer_t *,
        vccrypt_buffer_t *)
{
    return false;
}

static int dummy_contract_resolver(
        void *, void *, const uint8_t *, const uint8_t *,
        vccert_contract_closure_t *)
{
    return VCCERT_ERROR_PARSER_ATTEST_MISSING_CONTRACT;
}


void init()
{
    vccrypt_suite_register_velo_v1();
    malloc_allocator_options_init(&allocator_options);

    int result = vccrypt_suite_options_init(
            &crypt_options,
            &allocator_options,
            VCCRYPT_SUITE_VELO_V1);
    if (result != VCCRYPT_STATUS_SUCCESS)
    {
        JS_THROW("Failed to init crypto suite.");
    }

    result = vccert_builder_options_init(
            &cert_builder_options,
            &allocator_options,
            &crypt_options);
    if (result != VCCERT_STATUS_SUCCESS)
    {
        JS_THROW("Failed to init cert builder suite.");
    }

    result = vccert_parser_options_init(
            &parser_options,
            &allocator_options,
            &crypt_options,
            &dummy_txn_resolver,
            &dummy_artifact_state_resolver,
            &dummy_contract_resolver,
            &dummy_entity_key_resolver,
            NULL
    );

    if (result != VCCERT_STATUS_SUCCESS)
    {
        JS_THROW("Failed to init parser options.");
    }

}

EMSCRIPTEN_BINDINGS(vwbc)
{
    function("init", &init);

    register_vector<uint8_t>("_ByteVector");

    class_<Field>("Field")
            .constructor<std::vector<uint8_t>>()
            .function("string", &Field::string);

    class_<CertificateReader>("CertificateReader")
            .class_function("parse", &CertificateReader::parse)
            .function("count", &CertificateReader::count)
            .function("getFirst", &CertificateReader::getFirst)
            .function("get", &CertificateReader::get);

    class_<CertificateBuilder>("CertificateBuilder")
            .class_function("use", &CertificateBuilder::use)
            .function("emit", &CertificateBuilder::emit)
            .function("add_byte", &CertificateBuilder::add_byte)
            .function("add_short", &CertificateBuilder::add_short)
            .function("add_int", &CertificateBuilder::add_int)
            .function("add_long", &CertificateBuilder::add_long)
            .function("add_date", &CertificateBuilder::add_date)
            .function("add_uuid", &CertificateBuilder::add_uuid)
            .function("add_string", &CertificateBuilder::add_string);

    // todo, more values.
    enum_<vccert_field_type_t>("FieldTuple")
            .value("INVALID", VCCERT_FIELD_TYPE_INVALID)
            .value("CERTIFICATE_VERSION", VCCERT_FIELD_TYPE_CERTIFICATE_VERSION)
            .value("CERTIFICATE_VALID_FROM",
                   VCCERT_FIELD_TYPE_CERTIFICATE_VALID_FROM)
            .value("CERTIFICATE_VALID_TO",
                   VCCERT_FIELD_TYPE_CERTIFICATE_VALID_TO);
}