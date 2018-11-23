#include <emscripten/emscripten.h>
#include <vwblockchain/error.h>

EMSCRIPTEN_KEEPALIVE
int magic() {
    JS_THROW("you dun goofed");
    return 0xCAFE;
}
