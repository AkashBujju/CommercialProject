clear
clang -I../../external/glfw/mingw/include -I../../external/glad/include -I../../external/freetype/include -L../../external/glfw/mingw/lib/mac -L../../external/freetype/libs/mingw/mac ../../external/glad/src/glad.c ../../external/stb_image.c ../src/$1.c ../../src/math.c ../../src/model.c ../../src/shader.c ../../src/utils.c ../../src/light.c -o ../bin/$1.out -lglfw3 -lfreetype -framework OpenGL -framework Cocoa -framework IOKit
echo -- Done compiling
echo -- Execute 'sh run.sh %1 args'
