#ifndef BUTTON_H
#define BUTTON_H

#include "text.h"
#include "rectangle_2d.h"

typedef struct Button {
	Text text;
	Rectangle2D box;
	GLuint highlighted_box_texture;
	GLuint normal_box_texture;
} Button;

void draw_button(Button *button, GLuint text_program);
void init_button(Button *button, Font *font, char* string, GLuint rectangle_2d_program, GLuint box_texture_id, GLuint highlighted_box_texture_id, float norm_w, float norm_h);

#endif
