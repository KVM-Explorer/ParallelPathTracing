

includes("../lib")


local  root = os.scriptdir()

-- find all files in current directory

local files = os.files(path.join(root, "*.cpp"))

-- loop to gen all targets

for _, file in ipairs(files) do

    -- get the file name
    local filename = path.filename(file)

    -- get the target name
    local targetname = filename:sub(1, #filename - 4)

    -- add target
    target(targetname)
        add_files(file)
        set_group("tests")
        set_kind("binary")
        add_deps("catch2")
        add_vectorexts("all")
        add_includedirs("../src")

end



-- target("test_cpu_none")
--     add_files("test_cpu.cpp")
--     set_kind("binary")
--     add_deps("catch2")
--     add_rules("mode.profile")

-- target("test_sse_only")
--     add_files("test_sse.cpp")
--     add_rules("mode.profile")
--     set_kind("binary")
--     add_deps("catch2")
--     add_vectorexts("all")



