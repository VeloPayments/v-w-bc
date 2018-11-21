PWD?=$(CURDIR)
BUILD_DIR=$(CURDIR)/build

SRCDIR=$(PWD)/src
DIRS=$(SRCDIR) $(SRCDIR)/cert
SOURCES=$(foreach d,$(DIRS),$(wildcard $(d)/*.c))
STRIPPED_SOURCES=$(patsubst $(SRCDIR)/%,%,$(SOURCES))

VPR_DIR?=$(PWD)/lib/vcblockchain/lib/vpr/
VPR_INCLUDE_PATH?=$(VPR_DIR)/include
VPR_CFLAGS=-I $(VPR_INCLUDE_PATH)

VCCRYPT_DIR?=$(PWD)/lib/vcblockchain/lib/vccrypt/
VCCRYPT_INCLUDE_PATH?=$(VCCRYPT_DIR)/include
VCCRYPT_CFLAGS=-I $(VCCRYPT_INCLUDE_PATH)

VCCERT_DIR?=$(PWD)/lib/vctoolchain/lib/vccert/
VCCERT_INCLUDE_PATH?=$(VCCERT_DIR)/include
VCCERT_CFLAGS=-I $(VCCERT_INCLUDE_PATH)

COMMON_INCLUDES=$(VPR_CFLAGS) $(VCCRYPT_CFLAGS) $(VCCERT_CFLAGS) -I $(PWD)/include
COMMON_CFLAGS=$(COMMON_INCLUDES) -Wall -Werror -Wextra
WASM_RELEASE_CFLAGS=$(COMMON_CFLAGS) -O2

WASM_RELEASE_BUILD_DIR=$(BUILD_DIR)/wasm/release
WASM_RELEASE_DIRS=$(filter-out $(SRCDIR), \
				  $(patsubst $(SRCDIR)/%,$(WASM_RELEASE_BUILD_DIR)/%,$(DIRS)))

WASM_RELEASE_OBJECTS=$(patsubst %.c,$(WASM_RELEASE_BUILD_DIR)/%.o,$(STRIPPED_SOURCES))

WASM_RELEASE_CC=emcc
WASM_RELEASE_AR=emar

.PHONY: ALL
.PHONY: wasm.lib.release

wasm.lib.release: $(WASM_RELEASE_DIRS)

$(WASM_RELEASE_DIRS):
	mkdir -p $@

$(WASM_RELEASE_BUILD_DIR)/%.o: $(SRCDIR)/%.c
	$(info $<)
	$(info $@)
	mkdir -p $(dir $@)
	$(WASM_RELEASE_CC) $(WASM_RELEASE_CFLAGS) -c -o $@ $<
