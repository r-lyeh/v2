<h1 align="center"><a href="#">F·W·K</a></h1>
<p align="center">
3D game engine/framework in C.<br/>
<br/>
DEV REPOSITORY.<br/>
</p>

## About

- 3D game engine v2, written in C.
- Major overhaul from [old v1 engine](https://github.com/fwk3d/v1).
- In general, v2 is faster, smaller and stronger than v1.
- v2 is still WIP, though.

<details>
<summary><h2>Changelog</h2></summary>

- Smaller demos.
- Smaller codebase.
- Smaller repository size.
- Faster warm-up/boot times.
- Smoother experience. Higher framerates, less CPU usage.
- New ext/ plugin system. Github community-driven, self-discovery.
- New backends: SDL3, DearImgui, ImPlot, Luajit, OpenAL, etc.
- New redesigned UI. Docking and multi-viewports ready.
- New redesigned v2 Game modules: Loop, App, 3D Audio, Script, Shader...
- New bindings generator. All modules and APIs are 1:1 exposed to Lua scripts.
- Simplified build process. MAKE will link automatically ext/ dependencies.
- Simplified code structure. Old split/joint concepts are gone now.
- Simplified cook process. Asset tools/ folder no longer required.
- Simplified APIs in many cases. Smaller is better.
- Simplified implementations for many modules: cook, file, memory, panic, logger...
- Editor, Rendering and Scene modules decoupled from other APIs.
- Editor and Engine assets can be optionally embedded into output binary now.
- And more.

During the process, many things dropped compared to v1:

- Dropped pure C engine concept: the engine is still C, but there are C++ dependencies now.
- Dropped Linux, OSX and Emscripten targets. Windows only so far.
- Dropped gcc, clang, clang-cl and tcc compilers. Visual Studio only so far.
- Dropped Python bindings. Lua/Teal only so far.
- No single-header distributions.
- No fused zipfiles.
- No self-generated documentation.
- And more.

</details>

## Features
Soon.

## Gallery
Soon.

## Unlicense
This software is released into the [public domain](https://unlicense.org/). Also dual-licensed as [0-BSD](https://opensource.org/licenses/0BSD) or [MIT (No Attribution)](https://github.com/aws/mit-0) for those countries where public domain is a concern (sigh). Any contribution to this repository is implicitly subjected to the same release conditions aforementioned.

## Links
Still looking for alternatives? [amulet](https://github.com/ianmaclarty/amulet), [aroma](https://github.com/leafo/aroma/), [astera](https://github.com/tek256/astera), [blendelf](https://github.com/jesterKing/BlendELF), [bullordengine](https://github.com/MarilynDafa/Bulllord-Engine), [candle](https://github.com/EvilPudding/candle), [cave](https://github.com/kieselsteini/cave), [chickpea](https://github.com/ivansafrin/chickpea), [corange](https://github.com/orangeduck/Corange), [cute](https://github.com/RandyGaul/cute_framework), [dos-like](https://github.com/mattiasgustavsson/dos-like), [ejoy2d](https://github.com/ejoy/ejoy2d), [exengine](https://github.com/exezin/exengine), [gunslinger](https://github.com/MrFrenik/gunslinger), [hate](https://github.com/excessive/hate), [island](https://github.com/island-org/island), [juno](https://github.com/rxi/juno), [l](https://github.com/Lyatus/L), [lgf](https://github.com/Planimeter/lgf), [limbus](https://github.com/redien/limbus), [love](https://github.com/love2d/love/), [lovr](https://github.com/bjornbytes/lovr), [mini3d](https://github.com/mini3d/mini3d), [mintaro](https://github.com/mackron/mintaro), [mio](https://github.com/ccxvii/mio), [olive.c](https://github.com/tsoding/olive.c), [opensource](https://github.com/w23/OpenSource), [ouzel](https://github.com/elnormous/ouzel/), [pez](https://github.com/prideout/pez), [pixie](https://github.com/mattiasgustavsson/pixie), [punity](https://github.com/martincohen/Punity), [r96](https://github.com/badlogic/r96), [ricotech](https://github.com/dbechrd/RicoTech), [rizz](https://github.com/septag/rizz), [tigr](https://github.com/erkkah/tigr), [yourgamelib](https://github.com/duddel/yourgamelib)

<a href="https://github.com/fwk3d/v2/issues"><img alt="Issues" src="https://img.shields.io/github/issues-raw/fwk3d/v2.svg"/></a> <a href="https://discord.gg/UpB7nahEFU"><img alt="Discord" src="https://img.shields.io/discord/270565488365535232?color=5865F2&label=chat&logo=discord&logoColor=white"/></a>
