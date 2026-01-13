SHADERC := bgfx/bgfx/.build/linux64_clang/bin/shadercRelease
BINS_DIR := .bins
VERTEX_SHADERS := $(wildcard *-vs.sc)
FRAGMENT_SHADERS := $(wildcard *-fs.sc)
VERTEX_BINS := $(patsubst %-vs.sc,$(BINS_DIR)/%-vs.bin,$(VERTEX_SHADERS))
FRAGMENT_BINS := $(patsubst %-fs.sc,$(BINS_DIR)/%-fs.bin,$(FRAGMENT_SHADERS))

all: FORCE $(SHADERC) $(VERTEX_BINS) $(FRAGMENT_BINS)
	coddle debug

$(SHADERC):
	@echo "Building bgfx (one-time setup)..."
	sed -i '/no-nan-infinity-disabled/d' bgfx/bx/scripts/toolchain.lua
	$(MAKE) -j$$(nproc) -C bgfx/GENie/build/gmake.linux
	cp bgfx/GENie/bin/linux/genie bgfx/bx/tools/bin/linux/genie
	$(MAKE) -j$$(nproc) -C bgfx/bgfx linux-clang-release64

shaders: $(VERTEX_BINS) $(FRAGMENT_BINS)

$(BINS_DIR)/%-vs.bin: %-vs.sc varying.def.sc | $(BINS_DIR)
	$(SHADERC) -i bgfx/bgfx/src -f $< -o $@ --type vertex --platform linux --profile 430 --varyingdef varying.def.sc

$(BINS_DIR)/%-fs.bin: %-fs.sc varying.def.sc | $(BINS_DIR)
	$(SHADERC) -i bgfx/bgfx/src -f $< -o $@ --type fragment --platform linux --profile 430 --varyingdef varying.def.sc

$(BINS_DIR):
	mkdir -p $(BINS_DIR)

FORCE:
