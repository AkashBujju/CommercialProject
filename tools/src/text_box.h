#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include "text.h"
#include "rectangle_2d.h"

typedef struct TextBox {
	Text text;
	Rectangle2D box;
	Rectangle2D cursor;
	GLuint highlighted_box_texture;
	GLuint normal_box_texture;
	uint8_t is_active;
	uint8_t toggle_cursor;
	int16_t cursor_index;
	float timer;
} TextBox;

void init_textbox(TextBox *text_box, Font *font, char* string, GLuint rectangle_2d_program, GLuint box_texture_id, GLuint highlighted_box_texture_id, GLuint cursor_texture_id);
void handle_cursor_movement(TextBox *text_box, int key);
void handle_text_input(TextBox *text_box, char c);
void handle_textbox_click(TextBox *text_box, Vector2 *norm_mouse_pos);
void set_textbox_position(TextBox *text_box, float norm_x, float norm_y);
void draw_textbox(TextBox *text_box, GLuint text_program, float current_time);

#endif
