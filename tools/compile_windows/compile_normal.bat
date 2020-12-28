@echo off
cls
gcc ..\src\%1.c -o ..\bin\%1.exe
echo -- Done compiling
echo -- Execute sh run.sh %1 args