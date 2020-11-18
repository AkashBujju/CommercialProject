clear
cd ../src
clang -I../external/glfw/mingw/include -I../external/glad/include -I../external/freetype/include -L../external/glfw/mingw/lib/mac -L../external/freetype/libs/mingw/mac ../external/glad/src/glad.c ../external/stb_image.c *.c -o ../bin/main.out -lglfw3 -lfreetype -framework OpenGL -framework Cocoa -framework IOKit 
cd ..
