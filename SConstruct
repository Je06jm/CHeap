import os

force_mingw = int(ARGUMENTS.get("force-mingw", 0)) == 1

env = None

if force_mingw:
    env = Environment(tools=["mingw"])
else:
    env = Environment()

env["SYSTEM"] = os.name()

if force_mingw:
    env["SYSTEM"] = "posix"

use_debug = int(ARGUMENTS.get("debug", 0)) == 1

if env["SYSTEM"] in ["posix"]:
    if use_debug:
        env.Append(CCFLAGS=["-g"])
    else:
        env.Append(CCFLAGS=["-O3"])

    env.Append(CCFLAGS=["-I./include/"])

else:
    if use_debug:
        env.Append(CCFLAGS=["/Zi"])
    else:
        env.Append(CCFLAGS=["/O2"])

    env.Append(CCFLAGS=["/I./include/"])

cheap_files = Glob("src/*.c")
test_files = Glob("test/*.c")

static = env.StaticLibrary("CHeap", cheap_files)

test = env.Program("CHeapTest", cheap_files + test_files)

env.Alias("static", static)
env.Alias("test", test)