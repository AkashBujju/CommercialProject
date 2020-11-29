#include "text_box.h"
#include <GLFW/glfw3.h>
#include "../../src/utils.h"

extern GLuint program_2d;
extern uint16_t window_width;
extern uint16_t window_height;

static void set_cursor_pos(TextBox *text_box) {
	uint16_t len = strlen(text_box->text.text);
	if(text_box->cursor_index >= len)
		text_box->cursor_index = len - 1;

	if(text_box->cursor_index < 0) {
		text_box->cursor_index = -1;
		text_box->cursor.position.y = text_box->box.position.y;
		text_box->cursor.position.x = f_normalize(text_box->text.position.x, 0, window_width, -1, +1);
	}
	else {
		Vector2 text_dims = get_char_offset(text_box->text.font, 1, text_box->text.text, text_box->cursor_index);
		text_dims.x = f_normalize(text_dims.x, 0, window_width, 0, 1);
		text_dims.y = f_normalize(text_dims.y, 0, window_height, 0, 1);

		text_dims.x += f_normalize(text_box->text.position.x, 0, window_width, -1, +1);
		text_dims.y = f_normalize(text_box->text.position.y, 0, window_height, -1, +1) + text_dims.y;

		translate_rectangle_2d(&text_box->cursor, text_dims.x, text_dims.y);
	}
}

static void remove_char(TextBox *text_box) {
	int16_t index = text_box->cursor_index;
	if(index >= 0) {
		memmove(&text_box->text.text[index], &text_box->text.text[index + 1], strlen(text_box->text.text) - index);
		text_box->cursor_index -= 1;
		set_cursor_pos(text_box);
	}
}

void init_textbox(TextBox *text_box, Font *font, char* string, GLuint rectangle_2d_program, GLuint box_texture_id, GLuint highlighted_box_texture_id, GLuint cursor_texture_id) {
	text_box->is_active = 0;
	text_box->timer = 0;
	text_box->toggle_cursor = 1;
	text_box->highlighted_box_texture = highlighted_box_texture_id;
	text_box->normal_box_texture = box_texture_id;

	load_rectangle_2d(&text_box->box, rectangle_2d_program, box_texture_id, 0.25f, 0.125f);
	load_rectangle_2d(&text_box->cursor, rectangle_2d_program, cursor_texture_id, text_box->box.dimensions.x * 0.02f, text_box->box.dimensions.y * 0.5f);

	init_text(&text_box->text, font, string, 0, 0, 1, 1, 0);
	set_text_position(&text_box->text, text_box->box.position.x - (text_box->box.dimensions.x / 2.0f) * 0.85f, text_box->box.position.y - text_box->text.normalized_dims.y);

	if(strlen(string) >= 1)
		text_box->cursor_index = 0;
	else
		text_box->cursor_index = -1;

	set_cursor_pos(text_box);
}

void handle_cursor_movement(TextBox *text_box, int key) {
	if(text_box->is_active) {
		if(key == GLFW_KEY_RIGHT) {
			text_box->cursor_index += 1;
			set_cursor_pos(text_box);
		}
		else if(key == GLFW_KEY_LEFT) {
			text_box->cursor_index -= 1;
			set_cursor_pos(text_box);
		}
		else if(key == GLFW_KEY_BACKSPACE) {
			remove_char(text_box);	
		}
	}
}

void handle_text_input(TextBox *text_box, char c) {
	if(text_box->is_active) {
		insert_char_into_string(text_box->text.text, c, text_box->cursor_index + 1);
		text_box->cursor_index += 1;
		set_cursor_pos(text_box);
	}
}

void handle_textbox_click(TextBox *text_box, Vector2 *norm_mouse_pos) {
	uint8_t clicked = in_rect(norm_mouse_pos, &text_box->box.position, &text_box->box.dimensions);
	text_box->is_active = clicked;
}

void set_textbox_position(TextBox *text_box, float norm_x, float norm_y) {

}

void draw_textbox(TextBox *text_box, GLuint text_program, float current_time) {
	show_text(&text_box->text, text_program);
	if(text_box->timer == 0)
		text_box->timer = current_time;

	if(text_box->is_active) {
		if((current_time - text_box->timer) >= 0.3f) {
			text_box->toggle_cursor = !text_box->toggle_cursor;	
			text_box->timer = current_time;
		}
		text_box->box.texture_id = text_box->highlighted_box_texture;
	}
	else {
		text_box->box.texture_id = text_box->normal_box_texture;
	}

	if(text_box->toggle_cursor)
		draw_rectangle_2d(&text_box->cursor);

	draw_rectangle_2d(&text_box->box);
}
