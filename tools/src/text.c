#include "text.h"

extern uint16_t window_width;
extern uint16_t window_height;

void init_text(Text *text, Font *font, char* string, float norm_x, float norm_y, float r, float g, float b) {
	strcpy(text->text, string);
	text->length = strlen(string);
	text->font = font;
	text->position.x = f_normalize(norm_x, -1, +1, 0, window_width);
	text->position.y = f_normalize(norm_y, -1, +1, 0, window_height);
	text->color.x = r;
	text->color.y = g;
	text->color.z = b;
	text->normalized_dims.x = f_normalize(get_text_width(text->font, 1, text->text), 0, window_width, 0, 1);
	text->normalized_dims.y = f_normalize(get_text_height(text->font, 1, text->text), 0, window_height, 0, 1);
}

void set_text_position(Text *text, float norm_x, float norm_y) {
	text->position.x = f_normalize(norm_x, -1, +1, 0, window_width);
	text->position.y = f_normalize(norm_y, -1, +1, 0, window_height);
}

void update_text(Text *text) {
	text->length = strlen(text->text);
	text->normalized_dims.x = f_normalize(get_text_width(text->font, 1, text->text), 0, window_width, 0, 1);
	text->normalized_dims.y = f_normalize(get_text_height(text->font, 1, text->text), 0, window_height, 0, 1);
}

void translate_text_by(Text *text, float norm_dx, float norm_dy) {
	Vector2 text_pos;
	text_pos.x = f_normalize(text->position.x, 0, window_width, -1, +1);
	text_pos.y = f_normalize(text->position.y, 0, window_height, -1, +1);
	text_pos.x += norm_dx;
	text_pos.y += norm_dy;
	text->position.x = f_normalize(text_pos.x, -1, +1, 0, window_width);
	text->position.y = f_normalize(text_pos.y, -1, +1, 0, window_height);
}

void show_text(Text *text, GLuint program) {
	render_text(text->font, program, text->text, text->position.x, text->position.y, 1, text->color.x, text->color.y, text->color.z);
}
