#include "button.h"

void init_button(Button *button, Font *font, char* string, GLuint rectangle_2d_program, GLuint box_texture_id, GLuint highlighted_box_texture_id, float norm_w, float norm_h) {
	button->highlighted_box_texture = highlighted_box_texture_id;
	button->normal_box_texture = box_texture_id;

	load_rectangle_2d(&button->box, rectangle_2d_program, box_texture_id, norm_w, norm_h);

	init_text(&button->text, font, string, 0, 0, 1, 1, 0);
	set_text_position(&button->text, button->box.position.x - (button->text.normalized_dims.x / 2.0f), button->box.position.y - button->text.normalized_dims.y);
}

void draw_button(Button *button, GLuint text_program) {
	show_text(&button->text, text_program);
	draw_rectangle_2d(&button->box);
}
