target("catch2")
    -- on_build(function (target)
    --     if is_mode("debug") then
    --         set_kind("shared")
    --     else 
    --         set_kind("static")
    --     end
    -- end)
    set_kind("static")
    add_files("catch2/catch_amalgamated.cpp")
    add_includedirs("./",{public=true})