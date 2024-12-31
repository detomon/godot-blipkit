GODOT = godot
NATIVE_DIR = addons/detomon.blipkit/native

TARGET_DEBUG = target=template_debug debug_symbols=yes
TARGET_RELEASE = target=template_release

build:
	git submodule update --init --recursive

	scons platform=macos arch=universal $(TARGET_DEBUG)
#	scons platform=macos arch=universal $(TARGET_RELEASE)

.PHONY: doc
doc:
	$(GODOT) --headless --gdextension-docs --doctool "$(NATIVE_DIR)"
	./doc/markdown.py "$(NATIVE_DIR)/doc_classes/*.xml" "doc/classes"
