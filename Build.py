#!/usr/bin/python

import os
import platform
import subprocess
import sys
from pathlib import Path

CFLAGS = ["-O0", "-std=c99", "-fno-builtin-memcpy", "-fno-builtin-memset", "-ffreestanding", "-fno-stack-protector", "-nostdlib", "-nodefaultlibs"]

BINDIR = ".EXE"
LIBDIR = ".LIB"
BUILDDIR = ".OBJ"
SRCDIR = "Implementation"

COMPILER = "clang" if platform.system() == "Windows" else "gcc"
ARCHIVER = "llvm-ar" if platform.system() == "Windows" else "ar"

CORELIBNAME = "PureCore.lib" if platform.system() == "Windows" else "PureCore.a"
STDLIBNAME = "PureStd.lib" if platform.system() == "Windows" else "PureStd.a"

dll = ".LIB/Pure.dll"
import_lib = ".LIB/PureDLL.lib"

def run(command):
	print(">", " ".join(command))
	subprocess.run(command, check=True)

def compile(input, output):
	command = [
		COMPILER,
		*CFLAGS,
		"-c",
		str(input),
		"-o",
		str(output)
	]

	run(command)
	return str(output)

def compile_src(file):
	file = Path(file)

	# Source/Core/ASCII.c
	#     ↓
	# Core/ASCII.c
	relative = file.relative_to(SRCDIR)

	# .OBJ/Core/ASCII.c.o
	output = Path(BUILDDIR) / (str(relative) + ".o")

	# Make sure .OBJ/Core or .OBJ/Std exists
	output.parent.mkdir(parents=True, exist_ok=True)

	return compile( file, output )


def main(mods: list[str]):
	core_dir = Path(SRCDIR) / "Core"
	std_dir = Path(SRCDIR) / "Std"

	core_files = []
	std_files = []

	# compile("Compiler/Emits.c", Path(BUILDDIR) / (Path("Compiler/Emits.c").name + ".o"))

	# if platform.system() == "Windows":
	# 	compile("Platform/Windows/Start.c", Path(BUILDDIR) / (Path("Platform/Windows/Start.c").name + ".o"))
	# else:
	# 	compile("Platform/Linux/Start.c", Path(BUILDDIR) / (Path("Platform/Linux/Start.c").name + ".o"))


	if "core" in mods:
		core_files = [
			str(f)
			for f in core_dir.rglob("*")
			if f.is_file() and f.suffix.lower() in ('.c', '.cpp')
		]

	if "std" in mods:
		std_files = [
			str(f)
			for f in std_dir.rglob("*")
			if f.is_file() and f.suffix.lower() in ('.c', '.cpp')
		]

	print("Compiling Core files...")
	core_objects = [compile_src(f) for f in core_files]

	print("\nCompiling Std files...")
	std_objects = [compile_src(f) for f in std_files]

	all_objects = core_objects + std_objects

	compiler_emits = "Compiler/Emits.c"
	os.makedirs(BUILDDIR + "/Compiler", exist_ok=True)
	std_objects.append( compile(compiler_emits, Path(BUILDDIR) / (str(compiler_emits) + ".o") ) )

	if not all_objects:
		print("No object files were produced!")
		return 1

	print("\nCore Object files:")
	for obj in core_objects:
		print("  ", obj)

	print("\nStd Object files:")
	for obj in std_objects:
		print("  ", obj)

	# ---------------------------------------------------------
	# 1. Build combined DLL (Core + Std)
	# ---------------------------------------------------------
	print("\nBuilding shared DLL...")
	dll = os.path.join(LIBDIR, "Pure.dll")
	import_lib = os.path.join(LIBDIR, "PureDLL.lib")

	if platform.system() == "Windows":
		makedll_command = [
			COMPILER,
			"-shared"
		] + all_objects + [
			"-Wl,/implib:" + import_lib,
			"-o",
			dll
		]

	# ---------------------------------------------------------
	# 2. Build static libraries (Core & Std separately)
	# ---------------------------------------------------------
	print("\nBuilding static libraries...")
	core_lib = os.path.join(LIBDIR, CORELIBNAME)
	std_lib = os.path.join(LIBDIR, STDLIBNAME)

	if core_objects:
		makelib_core_command = [
			ARCHIVER,
			"rcs",
			core_lib
		] + core_objects
		run(makelib_core_command)

	if std_objects:
		makelib_std_command = [
			ARCHIVER,
			"rcs",
			std_lib
		] + std_objects
		run(makelib_std_command)

	print("\nBuild complete!")
	return 0


def test(test_file="Test/Unit.c"):
	if main(["core", "std"]) != 0:
		return 1

	test_path = Path(test_file)
	test_binary = Path(BUILDDIR) / (test_path.stem + (".exe" if platform.system() == "Windows" else ""))
	if platform.system() == "Windows":
		command = [
			COMPILER,
			"-ffreestanding",
			"-fno-stack-protector",
			"-nostdlib",
			"-nodefaultlibs",
			"-nostartfiles",
			str(test_path),
			"-I.",
			"-IInterface",
			"-Wl,/entry:_start",
			"-Wl,/subsystem:console",
			"-o",
			str(test_binary),
			os.path.join(LIBDIR, STDLIBNAME),
			os.path.join(LIBDIR, CORELIBNAME),
			"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.26100.0\\um\\x64\\kernel32.Lib",
		]
	else:
		command = [
			COMPILER,
			"-ffreestanding",
			"-fno-stack-protector",
			"-nostdlib",
			"-nodefaultlibs",
			"-nostartfiles",
			str(test_path),
			"Platform/Linux/Start.c",
			"-I.",
			"-IInterface",
			os.path.join(LIBDIR, STDLIBNAME),
			os.path.join(LIBDIR, CORELIBNAME),
			"-no-pie",
			"-Wl,-e,_start",
			"-o",
			str(test_binary),
		]
	run(command)
	run([str(test_binary)])
	return 0

def PureC_linux(files: list[str]):
	command = [
		COMPILER,
		"-ffreestanding",
		"-fno-stack-protector",
		"-nostdlib",
		"-nodefaultlibs",
		"-nostartfiles",
		"-I.",
		"-no-pie",
		"-Wl,-e,_start",
		*files,
		os.path.join(LIBDIR, STDLIBNAME),
		os.path.join(LIBDIR, CORELIBNAME),
	]
	run(command)
	return 0

def PureC_windows(files: list[str]):
	command = [
		COMPILER,
		"-ffreestanding",
		"-fno-stack-protector",
		"-nostdlib",
		"-nodefaultlibs",
		"-nostartfiles",
		"-I.",
		"-Wl,/entry:_start",
		"-Wl,/subsystem:console",
		*files,
		os.path.join(LIBDIR, STDLIBNAME),
		os.path.join(LIBDIR, CORELIBNAME),
		"C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.26100.0\\um\\x64\\kernel32.Lib",
	]

	run(command)
	return 0


def PureC(*args):
	command = [sys.executable, "Programs/PureC.py", *args]
	run(command)
	return 0

if __name__ == "__main__":
	os.makedirs(BUILDDIR, exist_ok=True)
	os.makedirs(LIBDIR, exist_ok=True)
	os.makedirs(BINDIR, exist_ok=True)

	if len(sys.argv) < 2:
		main(["core", "std"])
	else:
		exec(sys.argv[1])