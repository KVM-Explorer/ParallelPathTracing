


-- 在目标配置中加入对 "openmp" 选项的判断
target("PathTracing")
    add_files("main.cpp")
    set_kind("binary")
    add_includedirs("./",{public=true})
    add_tests("cpu")
    add_packages("openmp")
    add_cxxflags("-openmp:llvm")
    add_vectorexts("all")

    

target("STD")
    add_files("smallpt.cpp")
    set_kind("binary")
    add_packages("openmp")


