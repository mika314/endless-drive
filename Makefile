SHADERC := bgfx/bgfx/.build/linux64_clang/bin/shadercRelease
TEXTUREC := bgfx/bgfx/.build/linux64_clang/bin/texturecRelease
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
DDS_FROM_XCF := $(patsubst $(ASSETS_DIR)/%.xcf,$(DATA_DIR)/%.dds,$(GIMP_FILES))
ASSETS_WAVS := $(wildcard $(ASSETS_DIR)/*.wav)
DATA_WAVS := $(patsubst $(ASSETS_DIR)/%,$(DATA_DIR)/%,$(ASSETS_WAVS))


all: FORCE $(SHADERC) $(VERTEX_BINS) $(FRAGMENT_BINS) $(GLTF_FILES) $(FONT_VERTEX_BINS) $(FONT_FRAGMENT_BINS) $(DATA_FONTS) $(DDS_FROM_XCF) $(DATA_WAVS)
	coddle

$(SHADERC):
	@echo "Building bgfx (one-time setup)..."
	sed -i '/no-nan-infinity-disabled/d' bgfx/bx/scripts/toolchain.lua
	$(MAKE) -j$$(nproc) -C bgfx/GENie/build/gmake.linux
	cp bgfx/GENie/bin/linux/genie bgfx/bx/tools/bin/linux/genie
	$(MAKE) -j$$(nproc) -C bgfx/bgfx linux-clang-release64

shaders: $(VERTEX_BINS) $(FRAGMENT_BINS) $(FONT_VERTEX_BINS) $(FONT_FRAGMENT_BINS)

$(DATA_DIR)/%-vs.bin: %-vs.sc varying.def.sc shaderlib.sh | $(DATA_DIR)
	$(SHADERC) -i bgfx/bgfx/src -f $< -o $@ --type vertex --platform linux --profile spirv --varyingdef varying.def.sc

$(DATA_DIR)/%-fs.bin: %-fs.sc varying.def.sc shaderlib.sh | $(DATA_DIR)
	$(SHADERC) -i bgfx/bgfx/src -f $< -o $@ --type fragment --platform linux --profile spirv --varyingdef varying.def.sc

$(DATA_DIR)/%-uivs.bin: %-uivs.sc ui-varying.def.sc shaderlib.sh | $(DATA_DIR)
	$(SHADERC) -i bgfx/bgfx/src -f $< -o $@ --type vertex --platform linux --profile spirv --varyingdef ui-varying.def.sc

$(DATA_DIR)/%-uifs.bin: %-uifs.sc ui-varying.def.sc shaderlib.sh | $(DATA_DIR)
	$(SHADERC) -i bgfx/bgfx/src -f $< -o $@ --type fragment --platform linux --profile spirv --varyingdef ui-varying.def.sc

$(DATA_DIR)/%: $(ASSETS_DIR)/% | $(DATA_DIR)
	cp $< $@

$(DATA_DIR)/%.wav: $(ASSETS_DIR)/%.wav | $(DATA_DIR)
	ffmpeg -i $< -ar 48000 -ac 2 -c:a pcm_s16le -y $@

$(DATA_DIR)/%.gltf: $(ASSETS_DIR)/%.blend $(EXPORT_SCRIPT) | $(DATA_DIR) $(TEXTUREC)
	@echo "Exporting $< to $@"
	$(eval BLEND_TEMP_DIR := $(shell mktemp -d --tmpdir=.))
	/home/mika/bin/blender-5.0.1-linux-x64/blender -b $< -P $(EXPORT_SCRIPT) -- -o $(BLEND_TEMP_DIR)/$(notdir $*).gltf
	find $(BLEND_TEMP_DIR) -name "*.png" -exec sh -c '$(TEXTUREC) -f "$$1" -o "$(DATA_DIR)/$$(basename "$$1" .png).dds" -t BC3' _ {} \;
	find $(BLEND_TEMP_DIR) -type f -not -name "*.png" -exec cp {} $(DATA_DIR) \;
	mv $(BLEND_TEMP_DIR)/$(notdir $*).gltf $@
	rm -rf $(BLEND_TEMP_DIR)


$(DATA_DIR)/%.dds: $(ASSETS_DIR)/%.xcf | $(DATA_DIR) $(TEXTUREC)
	@echo "Exporting $< to $@"
	gimp -i -b '(let* ((image (car (gimp-file-load RUN-NONINTERACTIVE "$<" "$<"))) (layer (car (gimp-image-merge-visible-layers image CLIP-TO-IMAGE)))) (gimp-file-save RUN-NONINTERACTIVE image layer "$(@:.dds=.png)" "$(@:.dds=.png)") (gimp-quit 0))'
	$(TEXTUREC) -f "$(@:.dds=.png)" -o $@ -t BC3
	rm "$(@:.dds=.png)"

$(DATA_DIR):
	mkdir -p $(DATA_DIR)

FORCE:

clean:
	rm -rf .coddle
	rm -rf data
	rm -rf bgfx/bgfx/.build
	rm -rf .tmp-*
