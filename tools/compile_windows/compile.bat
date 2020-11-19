@echo off
cls
pushd ../src
clang -I..\..\external\glfw\mingw\include -I..\..\external\glad\include -I..\..\external\freetype\include -L..\..\external\glfw\mingw\lib\windows -L..\..\external\freetype\libs\mingw\windows ..\..\external\glad\src\glad.c ..\..\external\stb_image.c ../../math.c ../../model.c ../../shader.c ../../utils.c -o ..\bin\%1.exe -lglfw3 -lfreetype
popd
echo -- Done compiling
echo -- Execute 'run.bat %1 args'
