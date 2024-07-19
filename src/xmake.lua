

add_requires("openmp")
add_requires("cli11","spdlog")

target("PathTracing")
    add_files("main.cpp","config.cpp")
    set_kind("binary")
    add_includedirs("./", {public = true})
    add_tests("cpu")
    -- add_packages("openmp")
    -- add_cxxflags("-openmp:llvm")
    add_vectorexts("all")
    add_packages("cli11","spdlog")

    

target("STD")
    add_files("smallpt.cpp")
    set_kind("binary")
    if has_config("openmp") then
        add_packages("openmp")
    end 


