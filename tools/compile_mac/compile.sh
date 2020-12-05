clear

src_files=""
if [ $1 == "model_editor" ]; then
	cd ../bin
	./data_load.out
	cd ../compile_mac

	src_files+="../src/meta_output.c ../src/model_loader_gui.c ../src/model_properties_gui.c"
	echo Compiling extra $src_files
fi

clang -I../../external/glfw/mingw/include -I../../external/glad/include -I../../external/freetype/include -L../../external/glfw/mingw/lib/mac -L../../external/freetype/libs/mingw/mac ../../external/glad/src/glad.c ../../external/stb_image.c ../src/$1.c ../../src/math.c ../../src/model.c ../../src/shader.c ../../src/utils.c ../../src/light.c ../../src/instanced_model.c ../src/tmp_bone.c ../src/text.c $src_files ../src/rectangle_2d.c ../src/button.c ../src/text_box.c ../../src/text.c -o ../bin/$1.out -lglfw3 -lfreetype -framework OpenGL -framework Cocoa -framework IOKit
printf "\n"
echo -- Done compiling
echo -- Note: run compile.sh from compile_mac/compile_windows folder
echo -- Execute sh run.sh $1 args
printf "\n"
