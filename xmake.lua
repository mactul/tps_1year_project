add_rules("mode.debug", "mode.release")

set_policy("build.warning", true)
set_warnings("all", "extra")


rule("flags_extras")
    if is_mode("debug") then
        on_config(function(target)
            target:add("defines", "DEBUG")
        end)
    end

target("film_stats")
    add_includedirs("./", {public = true})
    add_files("src/**.c")
    add_files("film_stats/**.c")
    add_rules("flags_extras")
    add_links("sa")
    add_cxflags("-pedantic", "-O3")

target("film_parser")
    add_includedirs("./", {public = true})
    add_files("src/**.c")
    add_files("film_parser/**.c")
    add_rules("flags_extras")
    add_links("sa")
    add_cxflags("-pedantic", "-O3")