@echo off
cls
pushd src
gcc -I..\external\glfw\mingw\include -I..\external\glad\include -L..\external\glfw\mingw\lib ..\external\glad\src\glad.c *.c -o ..\bin\main.exe -lglfw3
popd
echo -- Compilation is done!
echo -- Execute 'run.bat'
