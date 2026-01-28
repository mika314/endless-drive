SHADERC := bgfx/bgfx/.build/linux64_clang/bin/shadercRelease
DATA_DIR := data
ASSETS_DIR := assets
EXPORT_SCRIPT := tools/export-gltf.py
VERTEX_SHADERS := $(wildcard *-vs.sc)
FRAGMENT_SHADERS := $(wildcard *-fs.sc)
VERTEX_BINS := $(patsubst %-vs.sc,$(DATA_DIR)/%-vs.bin,$(VERTEX_SHADERS))
FRAGMENT_BINS := $(patsubst %-fs.sc,$(DATA_DIR)/%-fs.bin,$(FRAGMENT_SHADERS))
BLEND_FILES := $(wildcard $(ASSETS_DIR)/*.blend)
GLTF_FILES := $(patsubst $(ASSETS_DIR)/%.blend,$(DATA_DIR)/%.gltf,$(BLEND_FILES))
FONT_VERTEX_SHADERS := $(wildcard *-uivs.sc)
FONT_FRAGMENT_SHADERS := $(wildcard *-uifs.sc)
FONT_VERTEX_BINS := $(patsubst %-uivs.sc,$(DATA_DIR)/%-uivs.bin,$(FONT_VERTEX_SHADERS))
FONT_FRAGMENT_BINS := $(patsubst %-uifs.sc,$(DATA_DIR)/%-uifs.bin,$(FONT_FRAGMENT_SHADERS))
ASSETS_FONTS := $(wildcard $(ASSETS_DIR)/*.ttf)
DATA_FONTS := $(patsubst $(ASSETS_DIR)/%,$(DATA_DIR)/%,$(ASSETS_FONTS))
GIMP_FILES := $(wildcard $(ASSETS_DIR)/*.xcf)
PNG_FILES := $(patsubst $(ASSETS_DIR)/%.xcf,$(DATA_DIR)/%.png,$(GIMP_FILES))

all: FORCE $(SHADERC) $(VERTEX_BINS) $(FRAGMENT_BINS) $(GLTF_FILES) $(FONT_VERTEX_BINS) $(FONT_FRAGMENT_BINS) $(DATA_FONTS) $(PNG_FILES)
	coddle debug

$(SHADERC):
	@echo "Building bgfx (one-time setup)..."
	sed -i '/no-nan-infinity-disabled/d' bgfx/bx/scripts/toolchain.lua
	$(MAKE) -j$$(nproc) -C bgfx/GENie/build/gmake.linux
	cp bgfx/GENie/bin/linux/genie bgfx/bx/tools/bin/linux/genie
	$(MAKE) -j$$(nproc) -C bgfx/bgfx linux-clang-release64

shaders: $(VERTEX_BINS) $(FRAGMENT_BINS) $(FONT_VERTEX_BINS) $(FONT_FRAGMENT_BINS)

$(DATA_DIR)/%-vs.bin: %-vs.sc varying.def.sc | $(DATA_DIR)
	$(SHADERC) -i bgfx/bgfx/src -f $< -o $@ --type vertex --platform linux --profile 120 --varyingdef varying.def.sc

$(DATA_DIR)/%-fs.bin: %-fs.sc varying.def.sc | $(DATA_DIR)
	$(SHADERC) -i bgfx/bgfx/src -f $< -o $@ --type fragment --platform linux --profile 120 --varyingdef varying.def.sc

$(DATA_DIR)/%-uivs.bin: %-uivs.sc ui-varying.def.sc | $(DATA_DIR)
	$(SHADERC) -i bgfx/bgfx/src -f $< -o $@ --type vertex --platform linux --profile 120 --varyingdef ui-varying.def.sc

$(DATA_DIR)/%-uifs.bin: %-uifs.sc ui-varying.def.sc | $(DATA_DIR)
	$(SHADERC) -i bgfx/bgfx/src -f $< -o $@ --type fragment --platform linux --profile 120 --varyingdef ui-varying.def.sc

$(DATA_DIR)/%: $(ASSETS_DIR)/% | $(DATA_DIR)
	cp $< $@

$(DATA_DIR)/%.gltf: $(ASSETS_DIR)/%.blend $(EXPORT_SCRIPT) | $(DATA_DIR)
	@echo "Exporting $< to $@"
	/home/mika/bin/blender-5.0.1-linux-x64/blender -b $< -P $(EXPORT_SCRIPT) -- -o $@

$(DATA_DIR)/%.png: $(ASSETS_DIR)/%.xcf $(EXPORT_SCRIPT) | $(DATA_DIR)
	@echo "Exporting $< to $@"
	gimp -i -b '(let* ((image (car (gimp-file-load RUN-NONINTERACTIVE "$<" "$<"))) (layer (car (gimp-image-merge-visible-layers image CLIP-TO-IMAGE)))) (gimp-file-save RUN-NONINTERACTIVE image layer "$@" "$@") (gimp-quit 0))'

$(DATA_DIR):
	mkdir -p $(DATA_DIR)

FORCE:

clean:
	rm -rf .coddle
	rm -rf data
	rm -rf bgfx/bgfx/.build
