if "%1"=="tidy" (
	del *.lib
	del *.exp
	del *.obj
	exit /b
)

rem since we're deploying .libs in our repo, optimize for space: /Os

cl /c imgui.cc -I ..\..\code -I ..\ext-backend-sdl3\inc -I inc -I inc\cimgui\imgui -DCODE /EHsc /O2 /MT /DNDEBUG /Os %* &&^
lib /nologo /out:x64\imgui.lib imgui.obj
