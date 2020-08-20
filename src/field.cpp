#include <vwbc/field.h>

Field::Field(std::vector<uint8_t> bytes)
{
    this->bytes = bytes;
}

std::string Field::string()
{
    return std::string(this->bytes.begin(), this->bytes.end());
}
