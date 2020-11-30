#include "button.h"

void init_button(Button *button, Font *font, char* string, GLuint rectangle_2d_program, GLuint box_texture_id, GLuint highlighted_box_texture_id, float norm_w, float norm_h) {
	button->highlighted_box_texture = highlighted_box_texture_id;
	button->normal_box_texture = box_texture_id;
	button->timer = 0;
	button->clicked = 0;

	load_rectangle_2d(&button->box, rectangle_2d_program, box_texture_id, norm_w, norm_h);

	init_text(&button->text, font, string, 0, 0, 1, 1, 0);
	set_text_position(&button->text, button->box.position.x - (button->text.normalized_dims.x / 2.0f), button->box.position.y - button->text.normalized_dims.y);
}

void button_clicked(Button *button, Vector2 *norm_mouse_pos) {
	uint8_t clicked = in_rect(norm_mouse_pos, &button->box.position, &button->box.dimensions);
	button->clicked = clicked;
}

void set_button_position(Button *button, float norm_x, float norm_y) {
	translate_rectangle_2d(&button->box, norm_x, norm_y);
	set_text_position(&button->text, button->box.position.x - (button->text.normalized_dims.x / 2.0f), button->box.position.y - button->text.normalized_dims.y);
}

void translate_button_by(Button *button, float norm_dx, float norm_dy) {
	translate_rectangle_2d_by(&button->box, norm_dx, norm_dy);
	translate_text_by(&button->text, norm_dx, norm_dy);
}

void draw_button(Button *button, GLuint text_program, float current_time) {
	if(!button->clicked)
		button->timer = current_time;

	if(button->clicked) {
		if((current_time - button->timer) <= 0.3f) {
			button->box.texture_id = button->highlighted_box_texture;
		}
		else {
			button->clicked = !button->clicked;
			button->box.texture_id = button->normal_box_texture;
		}
	}

	show_text(&button->text, text_program);
	draw_rectangle_2d(&button->box);
}
