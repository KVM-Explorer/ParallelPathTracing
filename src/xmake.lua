



-- 在目标配置中加入对 "openmp" 选项的判断
target("PathTracing")
    add_files("main.cpp")
    set_kind("binary")
    add_includedirs("./", {public = true})
    add_tests("cpu")

    if has_config("openmp") then
        add_packages("openmp")
        add_cxxflags("-openmp:llvm")
    end
    if has_config("simd") then 
         add_vectorexts("all")
    end


    

target("STD")
    add_files("smallpt.cpp")
    set_kind("binary")
    if has_config("openmp") then
        add_packages("openmp")
    end 


