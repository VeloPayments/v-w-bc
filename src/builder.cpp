#include <vwbc/builder.h>
#include <vccert/builder.h>
#include <vwbc/init.h>
#include <vwbc/error.h>
#include <vpr/parameters.h>

CertificateBuilder::~CertificateBuilder()
{
    if (context_is_valid)
    {
        dispose((disposable_t *) &context);
    }
}


void CertificateBuilder::add_string(uint16_t field, const std::string &value)
{
    size += 2 + 2 + value.length();
    fields.emplace_back(std::make_tuple(field, STRING, value));
}

void CertificateBuilder::add_int(uint16_t field, int32_t value)
{
    size += 2 + 2 + 4;
    fields.emplace_back(std::make_tuple(field, INT32, value));
}

emscripten::val CertificateBuilder::emit()
{
    int result = vccert_builder_init(&cert_builder_options, &context, size);
    if (result != VCCERT_STATUS_SUCCESS)
    {
        JS_THROW("Failed to setup certificate builder context.");
    }
    context_is_valid = true;

    for (auto[field, type, value] : fields)
    {
        switch (type)
        {
            case STRING:
                write_string_field(field, std::get<std::string>(value));
                break;
            case INT8:
                write_int8_field(field, std::get<int8_t>(value));
                break;
            case INT16:
                write_int16_field(field, std::get<int16_t>(value));
                break;
            case INT32:
                write_int32_field(field, std::get<int32_t>(value));
                break;
            case INT64:
                write_int64_field(field, std::get<int64_t>(value));
                break;
            case UINT64:
                write_uint64_field(field, std::get<uint64_t>(value));
                break;
            case BUFFER:
                write_bytes_field(field, std::get<std::vector<uint8_t>>(value));
                break;
        }
    }

    size_t total_size;
    const uint8_t *buffer = vccert_builder_emit(&context, &total_size);
    return emscripten::val(emscripten::typed_memory_view(total_size, buffer));
}

void CertificateBuilder::add_byte(uint16_t field, int8_t value)
{
    size += 2 + 2 + 1;
    fields.emplace_back(std::make_tuple(field, INT8, value));
}

void CertificateBuilder::add_short(uint16_t field, int16_t value)
{
    size += 2 + 2 + 2;
    fields.emplace_back(std::make_tuple(field, INT16, value));
}

void CertificateBuilder::add_long(uint16_t UNUSED(field), int64_t UNUSED(value))
{
    JS_THROW("int64_t is unsupported until we can figure out "
             + "how to do it with JS lack of int64 support. (2^53-1) :(")
}


void CertificateBuilder::add_bytes(uint16_t field,
                                   const std::string &value)
{
    // Its not actually a string. its just how embind matches types JS<->C++
    std::vector<uint8_t> bytes(std::begin(value), std::end(value));
    size += 2 + 2 + bytes.size();
    fields.emplace_back(std::make_tuple(field, BUFFER, bytes));
}

void CertificateBuilder::add_uuid(uint16_t field,
                                  const std::string &value)
{
    size += 2 + 2 + 16;
    vpr_uuid id;
    int result = vpr_uuid_from_string(&id, value.c_str());
    if (result != VPR_STATUS_SUCCESS)
    {
        JS_THROW("Failed to parse UUID.");
    }
    std::vector<uint8_t> bytes(id.data, std::end(id.data));
    fields.emplace_back(std::make_tuple(field, BUFFER, bytes));
}

void CertificateBuilder::add_date(uint16_t field,
                                  emscripten::val value)
{
    size += 2 + 2 + 4;
    // Why is Javascript so stupid. Javascript number caps out at (2^53) - 1
    // This means this code will be broken in 235 years
    uint64_t epoch = (uint64_t) value.call<double>("getTime") / 1000;
    fields.emplace_back(std::make_tuple(field, UINT64, epoch));
}

void
CertificateBuilder::write_string_field(uint16_t field, const std::string &value)
{
    vccert_builder_add_short_buffer(&context, field,
                                    (const uint8_t *) value.c_str(),
                                    value.size());
}

void CertificateBuilder::write_int8_field(uint16_t field, int8_t value)
{
    vccert_builder_add_short_int8(&context, field, value);
}

void CertificateBuilder::write_int32_field(uint16_t field, int32_t value)
{
    vccert_builder_add_short_int32(&context, field, value);
}

void CertificateBuilder::write_int16_field(uint16_t field, int16_t value)
{
    vccert_builder_add_short_int16(&context, field, value);
}

void CertificateBuilder::write_int64_field(uint16_t field, int64_t value)
{
    vccert_builder_add_short_int64(&context, field, value);
}

void CertificateBuilder::write_uint64_field(uint16_t field, uint64_t value)
{
    vccert_builder_add_short_uint64(&context, field, value);
}

void
CertificateBuilder::write_bytes_field(uint16_t field,
                                      std::vector<uint8_t> value)
{
    vccert_builder_add_short_buffer(&context, field, value.data(),
                                    value.size());
}

void CertificateBuilder::use(const emscripten::val &callback)
{
    CertificateBuilder builder;
    callback(builder);
}