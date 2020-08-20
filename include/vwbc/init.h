#pragma once

#include <vpr/allocator.h>
#include <vpr/allocator/malloc_allocator.h>
#include <vccert/builder.h>
#include <vccrypt/suite.h>


extern allocator_options_t allocator_options;
extern vccert_builder_options_t cert_builder_options;
extern vccert_parser_options_t parser_options;
extern vccrypt_suite_options_t crypt_options;
