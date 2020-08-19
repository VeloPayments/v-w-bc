#pragma once

#include <vwbc/field.h>
#include <cstdint>

class CertificateReader
{
public:
    ~CertificateReader();

    Field get(uint16_t field_id, uint16_t index);

    Field getFirst(uint16_t field_id);

    size_t count(uint16_t field_id);

private:
    CertificateReader() = default;

    bool context_is_valid = false;
    std::map<uint16_t, std::vector<std::vector<uint8_t>>> fields;
};

CertificateReader::~CertificateReader()
{

}
