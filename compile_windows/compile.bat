@echo off
cls
mkdir ..\bin
pushd ..\src
gcc -I..\external\glfw\mingw\include -I..\external\glad\include -I..\external\freetype\include -L..\external\glfw\mingw\lib\windows -L..\external\freetype\libs\mingw\windows ..\external\glad\src\glad.c ..\external\stb_image.c *.c -o ..\bin\main.exe -lglfw3 -lfreetype
popd
echo -- Compilation is done!
echo -- Execute 'run.bat'
