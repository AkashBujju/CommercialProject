#ifndef BUTTON_H
#define BUTTON_H

#include "text.h"
#include "rectangle_2d.h"

typedef struct Button {
	Text text;
	Rectangle2D box;
	GLuint highlighted_box_texture;
	GLuint normal_box_texture;
	float timer;
	uint8_t clicked;
} Button;

void draw_button(Button *button, GLuint text_program, float current_time);
void set_button_position(Button *button, float norm_x, float norm_y);
void button_clicked(Button *button, Vector2 *norm_mouse_pos);
void translate_button_by(Button *button, float norm_dx, float norm_dy);
void init_button(Button *button, Font *font, char* string, GLuint rectangle_2d_program, GLuint box_texture_id, GLuint highlighted_box_texture_id, float norm_w, float norm_h);

#endif
