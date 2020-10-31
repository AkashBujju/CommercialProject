@echo off
cls
pushd src
gcc -I..\external\glfw\mingw\include -I..\external\glad\include -I..\external\freetype\include -L..\external\glfw\mingw\lib -L..\external\freetype\libs\mingw ..\external\glad\src\glad.c ..\external\stb_image.c *.c -o ..\bin\main.exe -lglfw3 -lfreetype
popd
echo -- Compilation is done!
echo -- Execute 'run.bat'