#include "audio_stream_blipkit.hpp"
#include "blipkit_assembler.hpp"
#include "blipkit_instrument.hpp"
#include "blipkit_interpreter.hpp"
#include "blipkit_track.hpp"
#include "blipkit_waveform.hpp"
#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace detomon::BlipKit;
using namespace godot;

static void initialize_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_CLASS(AudioStreamBlipKit);
	GDREGISTER_CLASS(AudioStreamBlipKitPlayback);
	GDREGISTER_CLASS(BlipKitAssembler);
	GDREGISTER_CLASS(BlipKitInstrument);
	GDREGISTER_CLASS(BlipKitInterpreter);
	GDREGISTER_CLASS(BlipKitTrack);
	GDREGISTER_CLASS(BlipKitWaveform);
}

static void uninitialize_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
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
