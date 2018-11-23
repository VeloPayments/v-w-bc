#ifndef VWBLOCKCHAIN_ERROR_H
#define VWBLOCKCHAIN_ERROR_H

#define JS_THROW(x)\
EM_ASM({\
    var err = new Error(x);\
    err.stack = "Error: " + x + "\n\tat " $0 + ":" + $1;\
    throw err;\
}, __FILE__, __LINE__);\

#endif //VWBLOCKCHAIN_ERROR_H
