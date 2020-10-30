@echo off
cls
pushd src
gcc %1.c -o ..\bin\%1.exe
popd
echo -- Done compiling
echo -- Execute 'run.bat %1 args'
