#!/usr/bin/env python
import os
import sys

libname = "blipkit"
projectdir = "addons/detomon.blipkit/native"

localenv = Environment(tools=["default"], PLATFORM="")

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localenv)

Help(opts.GenerateHelpText(localenv))

env = localenv.Clone()
env = SConscript(projectdir + "/vendor/godot-cpp/SConstruct", {"env": env, "customs": customs})

env.Append(CPPPATH=[projectdir + "/src/", projectdir + "/vendor/BlipKit/src/"])
env.Append(CFLAGS=["-Wno-shift-negative-value"])

sources = Glob(projectdir + "/src/*.cpp")
sources += Glob(projectdir + "/vendor/BlipKit/src/*.c")

if env["target"] in ["editor", "template_debug"]:
    sources += env.GodotCPPDocData(projectdir + "/src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))

file = "{}{}{}".format(libname, env["suffix"], env["SHLIBSUFFIX"])
filepath = ""

if env["platform"] == "macos" or env["platform"] == "ios":
    filepath = "{}.framework/".format(env["platform"])
    file = "{}.{}.{}".format(libname, env["platform"], env["target"])

libraryfile = "bin/{}/{}{}".format(env["platform"], filepath, file)
library = env.SharedLibrary(
    libraryfile,
    source=sources,
)

copy = env.InstallAs("{}/bin/{}/{}lib{}".format(projectdir, env["platform"], filepath, file), library)

default_args = [library, copy]
Default(*default_args)
