#include <emscripten/emscripten.h>
#include <stdbool.h>
#include <vwblockchain/error.h>

static volatile bool REGISTERED = false;

EMSCRIPTEN_KEEPALIVE
void vwblockchain_init() {
    if (REGISTERED) {
        JS_THROW("mistakes were made");

    }
}