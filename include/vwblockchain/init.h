#ifndef VWBLOCKCHAIN_INIT_H
#define VWBLOCKCHAIN_INIT_H

#include <stdbool.h>

extern volatile bool vjblockchain_initialized;
extern allocator_options_t alloc_opts;
extern vccrypt_suite_options_t crypto_suite;
extern vccert_builder_options_t builder_opts;


#endif //VWBLOCKCHAIN_INIT_H
