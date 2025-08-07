{
    values = {
        [[C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.40.33807\bin\HostX64\x64\link.exe]],
        {
            "-nologo",
            "-dynamicbase",
            "-nxcompat",
            "-machine:x64",
            [[-libpath:build\windows\x64\release]],
            "/opt:ref",
            "/opt:icf",
            "Revolver.lib",
            "EFSW.lib"
        }
    },
    files = {
        [[build\.objs\Engine\windows\x64\release\App\main.cpp.obj]],
        [[build\windows\x64\release\EFSW.lib]],
        [[build\windows\x64\release\Revolver.lib]]
    }
}