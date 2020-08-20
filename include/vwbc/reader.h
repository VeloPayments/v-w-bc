#pragma once

#include <vwbc/field.h>
#include <cstdint>

class CertificateReader
{
public:
    ~CertificateReader() = default;

    Field get(uint16_t field_id, uint16_t index);

    Field getFirst(uint16_t field_id);

    size_t count(uint16_t field_id);

    static void parse(const std::string &bytes, bool attest,
                      const emscripten::val &callback);

private:
    CertificateReader(std::map<uint16_t,
                      std::vector<std::vector<uint8_t>>> fields);
    std::map<uint16_t, std::vector<std::vector<uint8_t>>> fields;
};



