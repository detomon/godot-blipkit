GODOT = godot
NATIVE_DIR = addons/detomon.blipkit/native

FLAGS = -j 16
TARGET_DEBUG = target=template_debug debug_symbols=yes $(FLAGS)
TARGET_RELEASE = target=template_release $(FLAGS)

build:
	git submodule update --init --recursive

	scons platform=macos arch=universal $(TARGET_DEBUG)
#	scons platform=macos arch=universal $(TARGET_RELEASE)
#	scons platform=ios $(TARGET_DEBUG)
#	scons platform=ios $(TARGET_RELEASE)

#	scons platform=web arch=wasm32 $(TARGET_RELEASE)

.PHONY: doc
doc:
	$(GODOT) --headless --gdextension-docs --doctool "$(NATIVE_DIR)"
	./doc/markdown.py "$(NATIVE_DIR)/doc_classes/*.xml" "doc/classes"
