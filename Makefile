GIT = git
SCONS = scons
GODOT = godot
CLANG_FORMAT = clang-format
PROJECT_DIR = .
API_VERSION = 4.4

FLAGS =
SCONS_FLAGS = api_version=$(API_VERSION)
TARGET_DEBUG = target=template_debug debug_symbols=yes $(FLAGS)
TARGET_RELEASE = target=template_release $(FLAGS)

build:
	$(GIT) submodule update --init --recursive
	$(SCONS) $(SCONS_FLAGS) $(TARGET_DEBUG)

.PHONY: doc
doc:
	$(GODOT) --headless --gdextension-docs --doctool "$(PROJECT_DIR)"
	./doc/markdown.py "$(PROJECT_DIR)/doc_classes/*.xml" "./doc/classes"

.PHONY: format
format:
	cd "$(PROJECT_DIR)/src" && $(CLANG_FORMAT) -i *.cpp -i *.hpp
