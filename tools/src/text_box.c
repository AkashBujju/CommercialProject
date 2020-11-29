#include "text_box.h"

extern GLuint program_2d;
extern uint16_t window_width;
extern uint16_t window_height;

static void set_cursor_pos(TextBox *text_box, Font *font, uint16_t index) {
	/* Start: Nov 29 2020 */
	Vector2 text_pos = get_char_offset(font, 1, text_box->text.text, index);
	text_box->cursor.position.x = f_normalize(text_pos.x, 0, window_width, -1, 1) + text_box->text.position.x;
	text_box->cursor.position.y = f_normalize(text_pos.y, 0, window_height, -1, 1) + text_box->text.position.y;
	print_vector2(&text_pos);
}

void init_textbox(TextBox *text_box, Font *font, char* string, GLuint rectangle_2d_program, GLuint box_texture_id, GLuint cursor_texture_id) {
	load_rectangle_2d(&text_box->box, rectangle_2d_program, box_texture_id, 0.25f, 0.125f);
	load_rectangle_2d(&text_box->cursor, rectangle_2d_program, cursor_texture_id, text_box->box.dimensions.x * 0.05f, text_box->box.dimensions.y * 0.5f);

	init_text(&text_box->text, font, string, 0, 0, 1, 0.65f, 0);
	set_text_position(&text_box->text, text_box->box.position.x - (text_box->box.dimensions.x / 2.0f) * 0.85f, text_box->box.position.y - text_box->text.normalized_dims.y);

	translate_rectangle_2d(&text_box->cursor, text_box->box.position.x - (text_box->box.dimensions.x / 2.0f) * 0.85f, text_box->box.position.y - text_box->text.normalized_dims.y);
	set_cursor_pos(text_box, font, 3);
}


void set_textbox_position(TextBox *text_box, float norm_x, float norm_y) {

}

void draw_textbox(TextBox *text_box, Font *font, GLuint text_program) {
	show_text(&text_box->text, font, text_program);
	draw_rectangle_2d(&text_box->cursor);
	draw_rectangle_2d(&text_box->box);
}
