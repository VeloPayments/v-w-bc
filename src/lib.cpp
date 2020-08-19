#include <emscripten/bind.h>
#include <emscripten/em_asm.h>
#include <iostream>
#include <vwbc/init.h>
#include <vwbc/builder.h>
#include <vwbc/parser.h>
#include <vwbc/fields.h>
#include <vwbc/reader.h>

using namespace emscripten;

allocator_options_t allocator_options;
vccert_builder_options_t cert_builder_options;
vccrypt_suite_options_t crypt_options;

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
        EM_ASM(throw "Failed to init crypto suite.");
    }

    result = vccert_builder_options_init(
            &cert_builder_options,
            &allocator_options,
            &crypt_options);
    if (result != VCCERT_STATUS_SUCCESS)
    {
        std::cout << VCCERT_ERROR_BUILDER_OPTIONS_INIT_INVALID_ARG << "\n";
        std::cout << result;
        std::cout << "\n";
        EM_ASM(throw "Failed to init cert builder suite.");
    }
}

EMSCRIPTEN_BINDINGS(vwbc)
{
    function("init", &init);

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