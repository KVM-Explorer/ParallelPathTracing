add_rules("mode.debug", "mode.release")

set_languages("c++20")

-- 添加一个名为 "openmp" 的可选配置项
-- option("openmp")
--     set_description("Enable OpenMP support")
--     set_default(true)

-- option("simd")
--     set_description("Enable SIMD support")
--     set_default(true)

includes("lib")
includes("src")
includes("tests")
includes("cuda")
