#pragma once

#include <emscripten/em_asm.h>

#define JS_THROW(x) { EM_ASM(throw x); }