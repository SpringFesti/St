add_rules("mode.debug", "mode.release")
add_requires("nana", "fmt", "sqlitecpp")

target("stpro")
    set_kind("binary")
    add_packages("nana", "fmt", "sqlitecpp")
    set_encodings("utf-8")
    add_files("src/*.cpp")
    set_targetdir("bin")

