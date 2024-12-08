GODOT = godot
ROOT_DIR = ../../..
NATIVE_DIR = $(shell cd "$(ROOT_DIR)" && realpath --relative-to=. "$(CURDIR)")

BUILD_FLAGS = -j7
TARGET_DEBUG = target=template_debug debug_symbols=yes $(BUILD_FLAGS)
TARGET_RELEASE = target=template_release $(BUILD_FLAGS)


build:
	git submodule update --init --recursive

	# Build extension before creating docs.
	@$(call build_extension)
	@$(call build_doc)

	# Build extension again to include changed docs.
	@$(call build_extension)


define build_extension
	scons platform=macos arch=universal $(TARGET_DEBUG)
#	scons platform=macos arch=universal $(TARGET_RELEASE)

# 	scons platform=linux arch=arm64 $(TARGET_DEBUG)
# 	scons platform=linux arch=arm64 $(TARGET_RELEASE)
# 	scons platform=linux arch=x86_64 $(TARGET_DEBUG)
# 	scons platform=linux arch=x86_64 $(TARGET_RELEASE)

# 	scons platform=windows arch=x86_64 $(TARGET_DEBUG)
# 	scons platform=windows arch=x86_64 $(TARGET_RELEASE)
endef


define build_doc
	cd "$(ROOT_DIR)" && $(GODOT) --headless --gdextension-docs --doctool "$(NATIVE_DIR)"
endef
