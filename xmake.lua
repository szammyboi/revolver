add_rules {
	"mode.debug",
	"mode.release",
	"plugin.compile_commands.autoupdate"
}

target "Engine"
	set_languages("c++17")
	set_kind "binary"
	add_files {
		"./App/*.cpp",
		"./App/**/*.cpp",
	}
	add_includedirs {
		"./App",
		"./Revolver",
	}
	add_deps {
		"Revolver"
	}

target "Revolver"
	set_languages("c++17")
	set_kind "static"
	add_files {
		"./Revolver/*.cpp",
		--"./Revolver/**/*.cpp",
	}
	add_includedirs {
		"./Revolver",
		"./Revolver/efsw/include/",
		"./Revolver/efsw/include/efsw/"
	}
	add_deps("EFSW")

target "EFSW"
	set_languages("c++17")
	set_kind "static"
	add_files {
		"./Revolver/efsw/src/efsw/*.cpp",
		"./Revolver/efsw/src/efsw/**/*.cpp",
		--"./Revolver/**/*.cpp",
	}
	add_includedirs {
		"./Revolver/efsw/include/",
		"./Revolver/efsw/src/",
		"./Revolver/efsw/include/efsw/",
	}

local timestamp = os.date("%Y%m%d_%H%M%S")

target "HotReload"
	set_languages("c++17")
	set_kind "shared"
	add_files {
		"./HotReload/*.cpp",
		"./HotReload/**/*.cpp",
	}
	add_includedirs {
		"./Revolver",
	}
	set_basename("HotReload_" .. timestamp)
