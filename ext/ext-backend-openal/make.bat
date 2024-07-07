if "%1"=="tidy" (
    del *.lib
    del *.exp
    del *.obj
    exit /b
)

rem since we're deploying .libs in our repo, optimize for space: /Os

cl /c src\stub.c -I inc\AL -I ..\ext-backend-sdl3\inc -DCODE /EHsc /O2 /MT /DNDEBUG /Os %* &&^
lib /nologo /out:x64\MojoAL.lib stub.obj
