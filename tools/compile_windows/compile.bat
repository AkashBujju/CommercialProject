@echo off
cls

pushd ..\bin
data_load.exe
popd
set src_files=..\src\meta_output.c ..\src\model_loader_gui.c ..\src\model_properties_gui.c ..\src\helper_models.c ..\src\check_box.c ..\src\exp_lines.c

mkdir ..\bin
cd
gcc -I..\..\external\glfw\mingw\include -I..\..\external\glad\include -I..\..\external\freetype\include -L..\..\external\glfw\mingw\lib\windows -L..\..\external\freetype\libs\mingw\windows ..\..\external\glad\src\glad.c ..\..\external\stb_image.c %src_files% ..\..\src\light.c ..\..\src\instanced_model.c ..\src\tmp_bone.c ..\src\text.c ..\src\rectangle_2d.c ..\src\button.c ..\src\text_box.c ..\..\src\text.c ..\..\src\math.c ..\..\src\model.c ..\..\src\shader.c ..\..\src\utils.c ..\src\%1.c -o ..\bin\%1.exe -lglfw3 -lfreetype
popd
echo -- Done compiling
echo -- Execute 'run.bat %1 args'