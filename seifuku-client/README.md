# Seifuku Client

Targeting patch 4351.

## Layout

- `src\3DBaseCode` - shared base code: the ini reader, the heap, the encrypted files; the exe's helpers
- `src\3DGameMap` - the map: terrain, layers, map objects, path finding
- `src\3DRole` - the roles, the hero, items, magic, effects
- `src\C3_CORE_DLL` - C3_CORE_DLL.dll: the 3D engine on Direct3D 9
- `src\DataThread` - DataThread.dll: the loading thread
- `src\FreeType` - the FreeType 2.1.10 library C3_CORE_DLL links statically (the release itself is vendor\freetype-2.1.10)
- `src\GameData` - GameData.dll
- `src\GameInput` - GameInput.dll: DirectInput
- `src\Graphic` - graphic.dll
- `src\GraphicData` - GraphicData.dll
- `src\MyShell` - the application, its dialogs and controls, the resources; client.vcxproj (Conquer.exe) and clientlib.vcxproj
- `src\NDSound` - NDsound.dll: sound
- `src\Role3D` - Role3D.dll
- `src\RoleView` - RoleView.dll
- `src\TqPackage` - TqPackage.dll: the .wdf packages
- `tests\<module>` - a CppUnitTest project per module (`client_tests` is `tests\MyShell`)
- `vendor\freetype-2.1.10` - FreeType 2.1.10 as released (the files the build uses), its licence in `docs\`
- `props\seifuku.props` - the settings every project shares
- `seifuku.sln` - the solution

## Third-party code

Portions of this software are copyright © 2005 The FreeType Project (www.freetype.org). All rights reserved.
`C3_CORE_DLL.dll` links FreeType 2.1.10 statically, under the FreeType License (`vendor\freetype-2.1.10\docs\FTL.TXT`). The
release's files in `vendor\freetype-2.1.10` are unmodified. `src\FreeType\c3_ftoption.h` is the release's
`include\freetype\config\ftoption.h` with two changes: `FT_CONFIG_OPTION_MAC_FONTS` is undefined and
`TT_CONFIG_OPTION_BYTECODE_INTERPRETER` is defined. `src\FreeType\c3_ftmodule.h` lists the font modules the build registers.

`src\NDSound`'s `dmutil` and `dsutil` files and `3DSound.cpp` start from Microsoft's DirectX SDK samples and keep
their notices.

## Requirements

- Visual Studio 2026 (Community or above) with the **Desktop development with C++** workload, the **v145** toolset (MSVC 14.50)
  and **C++ MFC for v145 build tools (x86 & x64)**.
- Windows SDK 10.0.26100.0 (Direct3D 9, DirectInput 8 and DirectSound come from it; no DirectX SDK is needed).

## Build

From a Developer Command Prompt (or with the full path to `MSBuild.exe`):

    msbuild seifuku.sln /m /p:Configuration=Release /p:Platform=Win32

`Debug` builds the same way. Everything lands in `bin\<Configuration>\`: `Conquer.exe`, `C3_CORE_DLL.dll`, `NDsound.dll`,
`TqPackage.dll`, `graphic.dll`, `GraphicData.dll`, `DataThread.dll`, `GameData.dll`, `GameInput.dll`, `Role3D.dll`,
`RoleView.dll`, and the test DLLs (`*_tests.dll`). Intermediate files go to `obj\`. Opening `seifuku.sln` in Visual Studio
works as well; pick `Win32`.

## Tests

From `bin\Release` (or `bin\Debug`):

    vstest.console.exe client_tests.dll c3core_tests.dll graphic_tests.dll /Platform:x86

(any of the `*_tests.dll`; `vstest.console.exe` is under `Common7\IDE\CommonExtensions\Microsoft\TestWindow` in the Visual Studio
folder). Some tests write fixture files next to their sources, under `tests\<module>\fixtures`.

## Run

The client needs the game's data files of build 4351 (`ani`, `c3`, `data`, `map`, `sound`, `ini`, `Help`, `c3.wdf`,
`data.wdf`, `res.dat`, `Server.dat` and the rest of the original folder, without its executables). Put the built exe and the
ten DLLs into that folder and start it as the launcher did:

    Conquer.exe blacknull

Any other command line shows an error and quits. `Server.dat`'s `Ip<n>=` lines name the servers; the client refuses the text
`127.0.0.1` (as the original did), so a server on the same machine is written `127.000.000.001`. `distributable\Server.dat` is
a sample: one server, `Seifuku`, at `127.000.000.001` port 9958; it replaces the original folder's.