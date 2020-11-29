#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include "text.h"
#include "rectangle_2d.h"

typedef struct TextBox {
	Text text;
	Rectangle2D box;
	Rectangle2D cursor;
} TextBox;

void init_textbox(TextBox *text_box, Font *font, char* string, GLuint rectangle_2d_program, GLuint box_texture_id, GLuint cursor_texture_id);
void draw_textbox(TextBox *text_box, Font *font, GLuint text_program);

#endif
