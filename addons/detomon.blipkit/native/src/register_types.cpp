#include "audio_stream_blipkit.hpp"
#include "blipkit_assembler.hpp"
#include "blipkit_bytecode.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_interpreter.hpp"
#include "blipkit_track.hpp"
#include "blipkit_waveform.hpp"
#include <gdextension_interface.h>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace BlipKit;
using namespace godot;

static Ref<BlipKitBytecodeLoader> bytecode_loader;
static Ref<BlipKitBytecodeSaver> bytecode_saver;

static void initialize_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_CLASS(AudioStreamBlipKit);
	GDREGISTER_CLASS(AudioStreamBlipKitPlayback);
	GDREGISTER_CLASS(BlipKitAssembler);
	GDREGISTER_CLASS(BlipKitBytecode);
	GDREGISTER_CLASS(BlipKitBytecodeLoader);
	GDREGISTER_CLASS(BlipKitBytecodeSaver);
	GDREGISTER_CLASS(BlipKitInstrument);
	GDREGISTER_CLASS(BlipKitInterpreter);
	GDREGISTER_CLASS(BlipKitTrack);
	GDREGISTER_CLASS(BlipKitWaveform);

	AudioStreamBlipKit::initialize();

	bytecode_loader.instantiate();
	ResourceLoader::get_singleton()->add_resource_format_loader(bytecode_loader, false);

	bytecode_saver.instantiate();
	ResourceSaver::get_singleton()->add_resource_format_saver(bytecode_saver, false);
}

static void uninitialize_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ResourceLoader::get_singleton()->remove_resource_format_loader(bytecode_loader);
	bytecode_loader.unref();

	ResourceSaver::get_singleton()->remove_resource_format_saver(bytecode_saver);
	bytecode_saver.unref();

	AudioStreamBlipKit::uninitialize();
}

extern "C" {

GDExtensionBool GDE_EXPORT detomon_blipkit_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_module);
	init_obj.register_terminator(uninitialize_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
} // extern "C"
