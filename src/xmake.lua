
target("PathTracing")
    add_files("main.cpp")
    set_kind("binary")
    add_includedirs("./",{public=true})
    add_tests("cpu")

target("STD")
    add_files("smallpt.cpp")
    set_kind("binary")

