#pragma once

class Field {
public:
    std::string uuid();
    std::string string();
    emscripten::val date();
    uint16_t uint16();
    uint32_t uint32();
};