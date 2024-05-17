add_rules("mode.debug", "mode.release")

set_languages("c++20")

target("PathTracing")
    add_files("src/*.cpp")
    set_kind("binary")
    add_includedirs("src/")

target("STD")
    add_files("smallpt.cpp")
    set_kind("binary")


