#!/usr/bin/env python
import os
import sys

libname = "blipkit"
projectdir = "."
addondir = "addons/detomon.blipkit"

localenv = Environment(tools=["default"], PLATFORM="")

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localenv)

Help(opts.GenerateHelpText(localenv))

env = localenv.Clone()
env = SConscript(projectdir + "/vendor/godot-cpp/SConstruct", {"env": env, "customs": customs})

env.Append(CPPPATH=[projectdir + "/src/", projectdir + "/vendor/BlipKit/src/"])

if env["platform"] != "windows":
	env.Append(CFLAGS=["-Wno-shift-negative-value"])
	env.Append(CXXFLAGS=[
		"-Wall",
		"-Wformat",
		"-Wformat=2",
		"-Wimplicit-fallthrough",
		"-Werror=format-security",
		"-U_FORTIFY_SOURCE",
		"-D_FORTIFY_SOURCE=3",
		"-D_GLIBCXX_ASSERTIONS",
	])

if env["platform"] == "windows":
	env.Append(CXXFLAGS=["/std:c++20"])

sources = Glob(projectdir + "/src/*.cpp")

blipkitsrc = projectdir + "/vendor/BlipKit/src/"
sources += map(lambda src: blipkitsrc + src, [
	"BKBase.c",
	"BKBuffer.c",
	"BKClock.c",
	"BKContext.c",
	"BKData.c",
	"BKInstrument.c",
	"BKInterpolation.c",
	"BKObject.c",
	"BKSequence.c",
	"BKTone.c",
	"BKTrack.c",
	"BKUnit.c",
])

if env["target"] in ["editor", "template_debug"]:
	sources += env.GodotCPPDocData(projectdir + "/src/gen/doc_data.gen.cpp", source=Glob(projectdir + "/doc_classes/*.xml"))

lib_filename = "{}{}{}{}".format(env.subst("$SHLIBPREFIX"), libname, env["suffix"], env.subst("$SHLIBSUFFIX"))
lib_filepath = ""

if env["platform"] in ["macos", "ios"]:
	framework_name = "lib{}{}".format(libname, env["suffix"])
	lib_filename = framework_name
	lib_filepath = "{}.framework/".format(framework_name)

	# Prevents the binary from getting a prefix / suffix automatically
	env["SHLIBPREFIX"] = ""
	env["SHLIBSUFFIX"] = ""

libraryfile = "bin/{}/{}{}".format(env["platform"], lib_filepath, lib_filename)
library = env.SharedLibrary(
	libraryfile,
	source=sources,
)

copy = env.Install("{}/bin/{}/{}".format(addondir, env["platform"], lib_filepath), library)

default_args = [library, copy]
Default(*default_args)
