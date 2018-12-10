#ifndef VWBLOCKCHAIN_ERROR_H
#define VWBLOCKCHAIN_ERROR_H

#include <emscripten/emscripten.h>
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define JS_THROW(x, y) {\
    EM_ASM_({\
        var msg = Pointer_stringify($0);\
        var file = Pointer_stringify($2);\
        var err = new Error(msg);\
        var num = $1;\
        err.stack = 'Error' + ' (' + num + '): ' + msg + '\n\tat ' + file + ':' + $3;\
        throw err;\
    }, x, y, __FILENAME__, __LINE__);\
}\

#define INIT_OR_FAIL(text, invocation) {\
    if (0 != (invocation))\
    {\
        JS_THROW(text " not initialized", invocation);\
        return -1;\
    }\
}\

#endif //VWBLOCKCHAIN_ERROR_H
