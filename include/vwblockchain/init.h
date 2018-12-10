#ifndef VWBLOCKCHAIN_INIT_H
#define VWBLOCKCHAIN_INIT_H

#include <stdbool.h>
#include <vccrypt/suite.h>
#include <vpr/allocator.h>
#include <vccert/parser.h>

extern volatile bool vjblockchain_initialized;
extern allocator_options_t alloc_opts;
extern vccrypt_suite_options_t crypto_suite;
extern vccert_parser_options_t parser_opts;

#endif //VWBLOCKCHAIN_INIT_H
