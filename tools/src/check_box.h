#ifndef CHECK_BOX_H
#define CHECK_BOX_H

#include "rectangle_2d.h"

typedef struct CheckBox {
	Rectangle2D box;
	Rectangle2D check;
	GLuint box_texture;
	GLuint check_texture;
	uint8_t checked;
} CheckBox;

void init_checkbox(CheckBox *check_box, GLuint rectangle_2d_program, GLuint box_texture, GLuint check_texture, float norm_w, float norm_h);
void draw_checkbox(CheckBox *check_box);
void translate_checkbox_by(CheckBox *check_box, float norm_dx, float norm_dy);
void translate_checkbox(CheckBox *check_box, float norm_dx, float norm_dy);
uint8_t checkbox_clicked(CheckBox *check_box, Vector2 *norm_mouse_pos);

#endif
