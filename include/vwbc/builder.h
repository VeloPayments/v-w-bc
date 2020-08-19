#pragma once

#include <cstdint>
#include <emscripten/bind.h>
#include <tuple>
#include <variant>
#include <vector>
#include <vccert/builder.h>

enum FieldType
{
    INT8, INT16, INT32, INT64, STRING, BUFFER, UINT64
};

typedef std::variant<
        int8_t,
        int16_t,
        int32_t,
        int64_t,
        uint64_t,
        std::string,
        std::vector<uint8_t>
> FieldValue;

typedef std::tuple<uint16_t, FieldType, FieldValue> FieldTuple;

class CertificateBuilder
{
public:
    static void use(const emscripten::val &callback);
    ~CertificateBuilder();

    void add_string(uint16_t field, const std::string &value);

    void add_byte(uint16_t field, int8_t value);

    void add_short(uint16_t field, int16_t value);

    void add_int(uint16_t field, int32_t value);

    void add_long(uint16_t field, int64_t value);

    void add_bytes(uint16_t field, const std::string &value);

    void add_uuid(uint16_t field, const std::string &value);

    void add_date(uint16_t field, emscripten::val value);

    emscripten::val emit();

private:
    CertificateBuilder() = default;

    void write_int8_field(uint16_t field, int8_t value);

    void write_int16_field(uint16_t field, int16_t value);

    void write_int32_field(uint16_t field, int32_t value);

    void write_int64_field(uint16_t field, int64_t value);

    void write_uint64_field(uint16_t field, uint64_t value);

    void write_string_field(uint16_t field, const std::string &value);

    void write_bytes_field(uint16_t field, std::vector<uint8_t> value);

    size_t size;
    std::vector<FieldTuple> fields;
    vccert_builder_context_t context;
    bool context_is_valid = false;
};

void certificate_builder(const emscripten::val &callback);