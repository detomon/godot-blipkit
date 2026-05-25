GIT = git
SCONS = scons
GODOT = godot
CLANG_FORMAT = clang-format
SVGO = svgo
PROJECT_DIR = .

FLAGS =
TARGET_DEBUG = target=template_debug debug_symbols=yes $(FLAGS)
TARGET_RELEASE = target=template_release $(FLAGS)

build:
	$(GIT) submodule update --init --recursive
	$(SCONS) $(TARGET_DEBUG)

.PHONY: doc
doc:
	$(GODOT) --headless --gdextension-docs --doctool "$(PROJECT_DIR)"
	./doc/markdown.py "$(PROJECT_DIR)/doc_classes/*.xml" "./doc/classes"

.PHONY: format
format:
	cd "$(PROJECT_DIR)/src" && $(CLANG_FORMAT) -i *.cpp -i *.hpp

.PHONY: svgo
svgo:
	find . -name "*.svg" -exec $(SVGO) --final-newline {} \;

.PHONY: otool
otool:
	otool -tVv bin/macos/libblipkit.macos.template_debug.universal.framework/libblipkit.macos.template_debug.universal | c++filt

.PHONY: otool-release
otool-release:
	otool -tVv bin/macos/libblipkit.macos.template_release.universal.framework/libblipkit.macos.template_release.universal | c++filt
