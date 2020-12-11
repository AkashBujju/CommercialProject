#include "check_box.h"

void init_checkbox(CheckBox *check_box, GLuint rectangle_2d_program, GLuint box_texture, GLuint check_texture, float norm_w, float norm_h) {
	check_box->box_texture = box_texture;
	check_box->check_texture = check_texture;
	check_box->checked = 0;

	load_rectangle_2d(&check_box->box, rectangle_2d_program, box_texture, norm_w, norm_h);
	load_rectangle_2d(&check_box->check, rectangle_2d_program, check_texture, norm_w, norm_h);
}

uint8_t checkbox_clicked(CheckBox *check_box, Vector2 *norm_mouse_pos) {
	uint8_t clicked = in_rect(norm_mouse_pos, &check_box->box.position, &check_box->box.dimensions);
	if(clicked)
		check_box->checked = !check_box->checked;

	return clicked;
}

void translate_checkbox_by(CheckBox *check_box, float norm_dx, float norm_dy) {
	translate_rectangle_2d_by(&check_box->box, norm_dx, norm_dy);
	translate_rectangle_2d_by(&check_box->check, norm_dx, norm_dy);
}

void translate_checkbox(CheckBox *check_box, float norm_dx, float norm_dy) {
	translate_rectangle_2d(&check_box->box, norm_dx, norm_dy);
	translate_rectangle_2d(&check_box->check, norm_dx, norm_dy);
}

void draw_checkbox(CheckBox *check_box) {
	if(check_box->checked)
		draw_rectangle_2d(&check_box->check);
	draw_rectangle_2d(&check_box->box);
}
