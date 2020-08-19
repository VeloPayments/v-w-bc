#pragma once

#include <cstdint>
#include <emscripten/bind.h>
#include <tuple>
#include <variant>
#include <vector>

void
certificate_parser(const std::string &bytes, bool attest,
                   const emscripten::val &callback);

