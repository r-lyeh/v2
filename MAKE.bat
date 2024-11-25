#!/bin/bash 2>nul || goto :windows

# linux + osx -----------------------------------------------------------------
cd `dirname $0`

# tidy environment
if [ "$1" = "tidy" ]; then
    rm a.out 2> /dev/null
    rm engine.o 2> /dev/null
    exit
fi

# sync
if [ "$1" = "sync" ]; then
    git reset --hard HEAD^1 && git pull
    sh MAKE.bat tidy
    exit
fi

export args=
export cc=cc
export o=-o

if [ "$(uname)" != "Darwin" ]; then
fi

if [ "$(uname)" = "Darwin" ]; then
fi

!cc! !o! hello.exe hello.c -I code code/engine.c !args! || exit 1

exit


:: -----------------------------------------------------------------------------
:windows

@echo off
cd /d "%~dp0"

if "%1"=="ext" (
    rem if exist code\ext\ext-fmod\x64\*.dll   copy code\ext\ext-fmod\x64\*.dll   > nul
    rem if exist code\ext\ext-assimp\x64\*.dll copy code\ext\ext-assimp\x64\*.dll > nul
    call ext\MAKE.bat
    exit /b
)

rem tidy environment
if "%1"=="tidy" (
    del *.def                       > nul 2> nul
    del *.mem                       > nul 2> nul
    del *.exp                       > nul 2> nul
    del *.lib                       > nul 2> nul
    del *.exe                       > nul 2> nul
    del *.obj                       > nul 2> nul
    del *.o                         > nul 2> nul
    del *.a                         > nul 2> nul
    del *.pdb                       > nul 2> nul
    del *.ilk                       > nul 2> nul
    del *.log                       > nul 2> nul
    rd /q /s .vs                    > nul 2> nul
    del cook*.csv                   > nul 2> nul
    del cook.zip                    > nul 2> nul
    del *.dll                       > nul 2> nul
    del .settings.ini               > nul 2> nul
    del .log.txt                    > nul 2> nul
    del ";*"                        > nul 2> nul
    rd /q /s lib                    > nul 2> nul
    rd /q /s .embed                 > nul 2> nul

    for /R art %%i in (.*) do del "%%i" > nul 2> nul
    for /R lab %%i in (.*) do del "%%i" > nul 2> nul
    for /R demos %%i in (.*) do del "%%i" > nul 2> nul
    for /R code %%i in (.*) do del "%%i" > nul 2> nul
    for /R ext %%i in (.*) do del "%%i" > nul 2> nul
    exit /b
)

rem sync
if "%1"=="sync" (
    git reset --hard HEAD~1 && git pull
    call MAKE.bat tidy
    exit /b
)

rem Compiler detection
if "%cc%"=="" (
    echo Detecting VS 2022/2019/2017/2015/2013 x64 ...
    (set "cc=cl" && where /q cl.exe) || (

               if exist "%ProgramFiles%/microsoft visual studio/2022/community/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles%/microsoft visual studio/2022/community/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=22"
        ) else if exist "%ProgramFiles%/microsoft visual studio/2022/enterprise/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles%/microsoft visual studio/2022/enterprise/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=22"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2019/community/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2019/community/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=19"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2019/enterprise/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2019/enterprise/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=19"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2017/community/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2017/community/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=17"
        ) else if exist "%ProgramFiles(x86)%/microsoft visual studio/2017/enterprise/VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%ProgramFiles(x86)%/microsoft visual studio/2017/enterprise/VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=17"
        ) else if exist "%VS170COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%VS170COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=22"
        ) else if exist "%VS160COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%VS160COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=19"
        ) else if exist "%VS150COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" (
                  @call "%VS150COMNTOOLS%/../../VC/Auxiliary/Build/vcvarsall.bat" amd64 >nul && set "vs=17"
        ) else if exist "%VS140COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsall.bat" (
                  @call "%VS140COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsall.bat" amd64 >nul && set "vs=15"
        ) else if exist "%VS120COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsall.bat" (
                  @call "%VS120COMNTOOLS%/../../VC/bin/x86_amd64/vcvarsall.bat" amd64 >nul && set "vs=13"
        ) else (
            echo Detecting Mingw64 ...
            (set "cc=gcc" && where /q gcc.exe) || (
                echo Detecting TCC ... && set "cc=tcc"
            )
        )
    )
)

if "%cc%"=="cl" (
    set o=/Fe:
    set args=/Zi /MT /EHsc /nologo
) else (
    set o=-o
    set args=
)

setlocal enableDelayedExpansion

rem PAUSE only if double-clicked from Windows explorer
rem (((echo.%cmdcmdline%)|%WINDIR%\system32\find.exe /I "%~0")>nul)&&pause
set "needs_pause=0"
(((echo .%cmdcmdline% | find /i "/c")>nul) && set "needs_pause=1")
rem (((echo .%cmdcmdline% | find /i "visual studio")>nul) && set "needs_pause=1")
(((echo .%cmdcmdline% | find /i "VsDevCmd")>nul) && set "needs_pause=0")
rem If /I "%COMSPEC%" == "%CMDCMDLINE%" set needs_pause=0

set tier="devel"

:parse_args
if not "%1"=="" (
           if "%1"=="embed" (
        pushd embed
        call make
        popd
    ) else if "%1"=="cook" (
        call make code\cook.c && cook

        where /q bundler.exe && (
        del .art.zip >nul 2>nul
        del filelist >nul 2>nul
        for /R demos %%i in (.*) do echo %%i >> filelist
        for /R ext %%i in (.*) do echo %%i >> filelist
        bundler a .art.zip @filelist -u lz4f
        del filelist >nul 2>nul
        )

    ) else if exist "%1" (
        set objs=!objs! %1
    ) else if "%1"=="asan" (
        set args=!args! /fsanitize=address
    ) else if "%1"=="debug" (
        set "tier=debug"
    ) else if "%1"=="devel" (
        set "tier=devel"
    ) else if "%1"=="release" (
        set "tier=release"
    ) else if "%1"=="retail" (
        set "tier=retail"
    ) else if "%1"=="o0" (
        set "tier=debug"
    ) else if "%1"=="O0" (
        set "tier=debug"
    ) else if "%1"=="o1" (
        set "tier=devel"
    ) else if "%1"=="O1" (
        set "tier=devel"
    ) else if "%1"=="o2" (
        set "tier=release"
    ) else if "%1"=="O2" (
        set "tier=release"
    ) else if "%1"=="o3" (
        set "tier=retail"
    ) else if "%1"=="O3" (
        set "tier=retail"
    ) else (
        set args=!args! %1
    )
    shift
    goto parse_args
)
if "!objs!"=="" (
    set objs=!o! hello.exe hello.c
)

if "!tier!" == "debug" (
    set args=!args! /DEBUG /Od
)
if "!tier!" == "devel" (
    set args=!args! /DNDEBUG=1
)
if "!tier!" == "release" (
    set args=!args! /DNDEBUG=2 /Os /Ox /O2 /GL /GF
)
if "!tier!" == "retail" (
    set args=!args! /DNDEBUG=3 /Os /Ox /O2 /GL /GF /Oy /Gw /arch:AVX2
    set libs=!libs! /OPT:ICF /LTCG
)

if not exist embed\bindings\api.h (
    rem echo Generating FFI bindings...
    rem call embed\bindings\MAKE.bat
)

rem generate ext/ext.h file
pushd ext & call make & popd

rem collect include folders iteratively
for /D %%i in (ext\*) do if exist "%%i\inc"     set exts=!exts! -I%%i\inc
for /D %%i in (ext\*) do if exist "%%i\include" set exts=!exts! -I%%i\include
for /D %%i in (ext\*) do if exist "%%i\x64"     set libs=!libs! /LIBPATH:%%i\x64
set args=!exts! !args!

rem since it takes a lot of time, compile dependencies at least once a day. hopefully ok for most users.
rem get system date and remove '/' chars on it. also remove spaces present on some configs.
set datestr=%date%
set datestr=%datestr:/=%
set datestr=%datestr:-=%
set datestr=%datestr:.=%
set datestr=%datestr: =%

set old=%time%

rem generate FFI bindings
if not exist nativecc-!datestr!.obj (
set defs=-D__forceinline= -DSDL_DISABLE_ANALYZE_MACROS -DINT_MAX=~0u
echo /* !datestr!: Auto-generated file. Do not edit */ > embed\engine.i
echo vs%vs%: !cc! /nologo !defs! -DAPI= -DHAS_BITFIELDS=0 -Icode -I. -EP code\engine.h -Icode\3rd\nil !args!
             !cc! /nologo !defs! -DAPI= -DHAS_BITFIELDS=0 -Icode -I. -EP code\engine.h -Icode\3rd\nil !args! |^
findstr /R /C:"[^ ]" |^
find /V "static " |^
find /V "__pragma(" |^
find /V "#pragma once" |^
find /V "#pragma warning" |^
find /V "#pragma comment" |^
find /V "#pragma region" |^
find /V "#pragma endregion" >> embed\engine.i
)

if not exist nativecc-!datestr!.obj (
echo vs%vs%: !cc! /Fo: nativecc-!datestr!.obj -Icode -I. /c code\native.cc !args!
             !cc! /Fo: nativecc-!datestr!.obj -Icode -I. /c code\native.cc !args! || set rc=1
)

if not exist native-!datestr!.obj (
echo vs%vs%: !cc! /Fo: native-!datestr!.obj -Icode -I. /c code\native.c !args!
             !cc! /Fo: native-!datestr!.obj -Icode -I. /c code\native.c !args! || set rc=1
)

echo vs%vs%: !cc! -Icode -I. !objs! nativecc-!datestr!.obj native-!datestr!.obj code\engine.c !args! /link !libs!
             !cc! -Icode -I. !objs! nativecc-!datestr!.obj native-!datestr!.obj code\engine.c !args! /link !libs! || set rc=1

set now=%time%
echo !old!
echo !now!
set "old=!old::0=: !"
set "now=!now::0=: !"
set cs0=!old:~0,2!*3600*100+!old:~3,2!*60*100+!old:~6,2!*100+!old:~9,2!
set cs1=!now:~0,2!*3600*100+!now:~3,2!*60*100+!now:~6,2!*100+!now:~9,2!
set /A diff=!cs1!-^(!cs0!^)
echo !diff:~0,-2!.!diff:~-2!s

:eof
if "!needs_pause!"=="1" pause

cmd /c exit !rc!
