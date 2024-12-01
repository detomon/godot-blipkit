BUILD_FLAGS = -j7
TARGET_DEBUG = target=template_debug debug_symbols=yes $(BUILD_FLAGS)
TARGET_RELEASE = target=template_release $(BUILD_FLAGS)

build:
	git submodule update --init --recursive

	scons platform=macos arch=universal $(TARGET_DEBUG)
	scons platform=macos arch=universal $(TARGET_RELEASE)

# 	scons platform=linux arch=arm64 $(TARGET_DEBUG)
# 	scons platform=linux arch=arm64 $(TARGET_RELEASE)
# 	scons platform=linux arch=x86_64 $(TARGET_DEBUG)
# 	scons platform=linux arch=x86_64 $(TARGET_RELEASE)

# 	scons platform=windows arch=x86_64 $(TARGET_DEBUG)
# 	scons platform=windows arch=x86_64 $(TARGET_RELEASE)
