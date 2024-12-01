build:
	git submodule update --init --recursive

	scons platform=macos arch=universal target=template_debug -j7
	scons platform=macos arch=universal target=template_release -j7

# 	scons platform=linux arch=arm64 target=template_debug -j7
# 	scons platform=linux arch=arm64 target=template_release -j7
# 	scons platform=linux arch=x86_64 target=template_debug -j7
# 	scons platform=linux arch=x86_64 target=template_release -j7

# 	scons platform=windows arch=x86_64 target=template_debug -j7
# 	scons platform=windows arch=x86_64 target=template_release -j7
