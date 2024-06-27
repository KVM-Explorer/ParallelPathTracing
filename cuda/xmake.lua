
local cuda_home =  os.getenv("CUDA_PATH")

target("cuda-raytracing") 
    add_files("main.cu")
    set_kind("binary")
    set_languages("c++20")
    add_includedirs(cuda_home .. "/include") 

