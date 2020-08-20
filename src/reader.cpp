#include <vwbc/reader.h>
#include <vccert/parser.h>
#include <vwbc/init.h>
#include <vwbc/error.h>
#include <vector>
#include <iostream>

void CertificateReader::parse(const std::string &bytes, bool attest,
                              const emscripten::val &callback)
{
    if (attest)
    {
        JS_THROW("TODO - Cannot attest.")
    }

    std::map<uint16_t, std::vector<std::vector<uint8_t>>> fields;

    vccert_parser_context_t parser_context;
    int result =
            vccert_parser_init(&parser_options, &parser_context, bytes.data(),
                               bytes.size());
    if (result != VCCERT_STATUS_SUCCESS)
    {
        JS_THROW("Failed to init parser.")
    }

    uint16_t field;
    const uint8_t *field_value;
    size_t field_size;
    result = vccert_parser_field_first(&parser_context, &field, &field_value,
                                       &field_size);
    if (result != VCCERT_STATUS_SUCCESS)
    {
        JS_THROW("Failed to read the first field from the certificate");
    }

    fields[field].push_back(
            (std::vector<uint8_t>(&field_value[0], &field_value[field_size])));
    std::cout << "Read field " << field << "\n";

    while (true)
    {
        result = vccert_parser_field_next(&parser_context, &field, &field_value,
                                          &field_size);
        if (result != VCCERT_STATUS_SUCCESS)
        {
            break;
        }
        std::cout << "Read field" << field << "\n";
        fields[field].push_back(std::vector<uint8_t>(&field_value[0],
                                                     &field_value[field_size]));
    }

    CertificateReader reader(fields);
    callback(reader);
}

CertificateReader::CertificateReader(
        std::map<uint16_t, std::vector<std::vector<uint8_t>>> fields)
{
    this->fields = fields;
}

size_t CertificateReader::count(uint16_t field_id)
{
    return this->fields[field_id].size();
}

Field CertificateReader::getFirst(uint16_t field_id)
{
    return Field(this->fields[field_id].front());
}

Field CertificateReader::get(uint16_t field_id, uint16_t index)
{

    return Field(this->fields[field_id].at(index));
}
