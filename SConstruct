env = Environment()

env["CXX"] = "g++"
env["CXXFLAGS"] = ["-std=c++23", "-O2" ]
""" "-Wall", "-Wextra" """

source_files = []
source_files.append("main.cc")
for folder in [
    "math",
    "model",
    "model/animation",
    "model/foreign",
    "model/renderer",
    "imgui",
    "viewer",
    "app",
]:
    source_files += Glob(f"{folder}/*.cpp")
    source_files += Glob(f"{folder}/*.cc")
    source_files += Glob(f"{folder}/*.cxx")


program = env.Program(
    target="model_viewer",
    source=source_files,
    LIBS=[
        "SDL2",
        "GL",
        "GLEW",
    ],
)
