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
extern GLuint rectangle_program;
extern GLuint text_program;
extern GLuint instanced_program;
extern GLuint instanced_helper_program;
extern GLuint dir_light_program;
extern GLuint spot_light_program;
extern GLuint background_left_texture;
extern GLuint model_properties_background_texture;
extern GLuint cursor_texture;
extern GLuint box_texture;
extern GLuint ht_box_texture;
extern GLuint button_texture;
extern GLuint ht_button_texture;
extern GLuint close_button_texture;
extern GLuint move_tag_texture;
extern GLuint check_texture;
extern Font consolas;
extern Font georgia_bold_12;
extern Font georgia_bold_16;
extern Font georgia_bold_20;

void load() {
	rectangle_program = compile_shader(combine_string(shaders_path, "rect_v_shader.shader"), combine_string(shaders_path, "texture_f_shader.shader"));
	text_program = compile_shader(combine_string(shaders_path, "v_text.shader"), combine_string(shaders_path, "f_text.shader"));
	instanced_program = compile_shader(combine_string(shaders_path, "instanced_v_shader.shader"), combine_string(shaders_path, "instanced_f_shader.shader"));
	instanced_helper_program = compile_shader(combine_string(shaders_path, "helper_model.vert"), combine_string(shaders_path, "helper_model.frag"));
	dir_light_program = compile_shader(combine_string(shaders_path, "v_dir_light.shader"), combine_string(shaders_path, "f_dir_light.shader"));
	spot_light_program = compile_shader(combine_string(shaders_path, "v_spot_light.shader"), combine_string(shaders_path, "f_spot_light.shader"));

	background_left_texture = make_texture(combine_string(assets_path, "png/test_1.png"));
	model_properties_background_texture = make_texture(combine_string(assets_path, "png/model_properties.png"));
	cursor_texture = make_texture(combine_string(assets_path, "png/cursor.png"));
	box_texture = make_texture(combine_string(assets_path, "png/text_box.png"));
	ht_box_texture = make_texture(combine_string(assets_path, "png/text_box_ht.png"));
	button_texture = make_texture(combine_string(assets_path, "png/button_box.png"));
	ht_button_texture = make_texture(combine_string(assets_path, "png/button_box_ht.png"));
	close_button_texture = make_texture(combine_string(assets_path, "png/close_button.png"));
	move_tag_texture = make_texture(combine_string(assets_path, "png/move_tag.png"));
	check_texture = make_texture(combine_string(assets_path, "png/check.png"));

	FT_Library ft_1;
	init_freetype(&ft_1);
	init_font(&consolas, 14, combine_string(assets_path, "fonts/consolas.ttf"), &ft_1);

	FT_Library ft_2;
	init_freetype(&ft_2);
	init_font(&georgia_bold_12, 12, combine_string(assets_path, "fonts/georgia_bold.ttf"), &ft_2);

	FT_Library ft_3;
	init_freetype(&ft_3);
	init_font(&georgia_bold_16, 16, combine_string(assets_path, "fonts/georgia_bold.ttf"), &ft_3);

	FT_Library ft_4;
	init_freetype(&ft_4);
	init_font(&georgia_bold_20, 20, combine_string(assets_path, "fonts/georgia_bold.ttf"), &ft_4);

	init_vector(&front, -0.386, -0.682, -0.620);
	init_vector(&position, 6.460, 9.800, 10.800);
	init_vector(&up, 0, 1, 0);
	make_identity(&view);	projection = perspective(45.0f, (float)window_width / window_height, 0.1f, 500.0f);
	text_projection = ortho(0, window_width, 0, window_height);
}

