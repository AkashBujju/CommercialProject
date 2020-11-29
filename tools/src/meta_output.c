#include "rectangle_2d.h"
#include "text.h"
#include "../../src/shader.h"
#include "../../src/utils.h"
#include "../../src/math.h"

extern char* assets_path;
extern char* shaders_path;
extern uint16_t window_width;
extern uint16_t window_height;
extern Matrix4 view;
extern Matrix4 projection;
extern Matrix4 text_projection;
extern Vector3 front;
extern Vector3 position;
extern Vector3 up;
extern GLuint program_2d;
extern GLuint text_program;
extern GLuint gui_tex_1;
extern GLuint cursor_texture;
extern GLuint box_texture;
extern GLuint ht_box_texture;
extern Font consolas;
extern Font georgia_bold;

void load() {
	program_2d = compile_shader(combine_string(shaders_path, "rect_v_shader.shader"), combine_string(shaders_path, "texture_f_shader.shader"));
	text_program = compile_shader(combine_string(shaders_path, "v_text.shader"), combine_string(shaders_path, "f_text.shader"));

	gui_tex_1 = make_texture(combine_string(assets_path, "png/test_1.png"));
	cursor_texture = make_texture(combine_string(assets_path, "png/cursor.png"));
	box_texture = make_texture(combine_string(assets_path, "png/text_box.png"));
	ht_box_texture = make_texture(combine_string(assets_path, "png/text_box_ht.png"));

	FT_Library ft_1;
	init_freetype(&ft_1);
	init_font(&consolas, combine_string(assets_path, "fonts/consolas.ttf"), &ft_1);

	FT_Library ft_2;
	init_freetype(&ft_2);
	init_font(&georgia_bold, combine_string(assets_path, "fonts/georgia_bold.ttf"), &ft_2);

	init_vector(&front, 0.000, 0.000, -1.000);
	init_vector(&position, 0.000, 0.000, 10.000);
	init_vector(&up, 0, 1, 0);
	make_identity(&view);	projection = perspective(45.0f, (float)window_width / window_height, 0.1f, 500.0f);
	text_projection = ortho(0, window_width, 0, window_height);
}

