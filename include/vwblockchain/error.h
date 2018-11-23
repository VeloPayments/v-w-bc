#ifndef VWBLOCKCHAIN_ERROR_H
#define VWBLOCKCHAIN_ERROR_H

#include <emscripten/emscripten.h>

#define JS_THROW(x) {\
    EM_ASM_({\
        var msg = Pointer_stringify($0);\
        var file = Pointer_stringify($1);\
        var err = new Error(msg);\
        err.stack = 'Error: ' + msg + '\n\tat ' + file + ':' + $2;\
        throw err;\
    }, x, __FILE__, __LINE__);\
}\

#define INIT_OR_FAIL(text, invocation) {\
    if (0 != (invocation))\
    {\
        JS_THROW(text " not initialized");\
        return -1;\
    }\
}\

#endif //VWBLOCKCHAIN_ERROR_H
