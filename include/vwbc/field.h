#pragma once

#include <string>
#include <emscripten/bind.h>

class Field {
public:
    std::string uuid();
    std::string string();
    emscripten::val date();
    uint32_t uint32();
    Field(std::vector<uint8_t> bytes);
    ~Field() = default;
private:
    std::vector<uint8_t> bytes;
};