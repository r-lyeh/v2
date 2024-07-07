cd /d "%~dp0"

if not exist bin2c.exe cl bin2c.c /MT
if not exist bin2c.exe exit /b

del ..\.embed\res.c >nul 2>nul
md ..\.embed >nul 2>nul

setlocal EnableDelayedExpansion

for /R %%i in (*.ttf;*.otf;*.glsl;*api.h;engine.i*) do (
    rem normalize absolute path, by using forward slashes
    set "p=%%~fi"
    set "p=!p:%cd%=!"
    set "p=!p:\=/!"

    rem normalize id, by replacing period and slashes
    set "id=!p!"
    set "id=!id:/=_!"
    set "id=!id:.=_!"
 
    bin2c.exe %%i ..\.embed\!id! !id! && (
    echo #if __has_include^("!id!"^)>> ..\.embed\res.c
    echo #include "!id!">> ..\.embed\res.c
    echo #endif>> ..\.embed\res.c
    )
)

echo struct resource_t { const char *name, *data; unsigned size; } resources[] = {>> ..\.embed\res.c
for /R %%i in (*.ttf;*.otf;*.glsl;*api.h;engine.i*) do (
    rem normalize absolute path, by using forward slashes
    set "p=%%~fi"
    set "p=!p:%cd%=!"
    set "p=!p:\=/!"

    rem normalize id, by replacing period and slashes
    set "id=!p!"
    set "id=!id:/=_!"
    set "id=!id:.=_!"

    echo #if __has_include^("!id!"^)>> ..\.embed\res.c
    echo { "embed!p!", !id!, ^(unsigned^)sizeof^(!id!^) },>> ..\.embed\res.c
    echo #endif>> ..\.embed\res.c
)
    echo { NULL, NULL, 0u },>> ..\.embed\res.c
echo };>> ..\.embed\res.c
echo.>> ..\.embed\res.c

rem type ..\.embed\res.c

del *.exe >nul 2>nul
del *.obj >nul 2>nul
