#include "gui.h"

extern GLuint rectangle_program;
extern GLuint text_program;
extern GLuint background_left_texture;
extern GLuint box_texture;
extern GLuint button_texture;
extern GLuint ht_button_texture;
extern GLuint ht_box_texture;
extern GLuint cursor_texture;

void init_transition_effect(TransitionEffect *transition_effect, uint16_t max_move_count, float transition_speed) {
	transition_effect->max_move_count = max_move_count;
	transition_effect->transition_speed = transition_speed;
	transition_effect->current_move_count = 0;
	transition_effect->in_transition = 0;
	transition_effect->activate = 0;
	transition_effect->current_state = FORWARD;
}

void init_editor_gui(EditorGUI *editor_gui, Font *text_font, float transition_speed, uint16_t max_move_count) {
	/* Rectangle2D */
	load_rectangle_2d(&editor_gui->background_left, rectangle_program, background_left_texture, 0.4f, 1.94f);
	translate_rectangle_2d(&editor_gui->background_left, -0.98f + editor_gui->background_left.dimensions.x / 2.0f, 0);
	/* Rectangle2D */

	/* Text */
	init_text(&editor_gui->model_load_text, text_font, "", -0.25f, +0.5f, 1, 1, 0);
	set_text(&editor_gui->model_load_text, "Load Model");
	set_text_position(&editor_gui->model_load_text, editor_gui->background_left.position.x - editor_gui->model_load_text.normalized_dims.x / 2.0f, (editor_gui->background_left.position.y + editor_gui->background_left.dimensions.y / 2) * 0.9f);
	/* Text */

	/* Text Box */
	init_textbox(&editor_gui->model_load_textbox, text_font, "Enter..", rectangle_program, box_texture, ht_box_texture, cursor_texture);
	set_textbox_position(&editor_gui->model_load_textbox, editor_gui->background_left.position.x - editor_gui->background_left.dimensions.x * 0.42f + editor_gui->model_load_textbox.box.dimensions.x * 0.5f, editor_gui->background_left.position.y + (editor_gui->background_left.dimensions.y / 2) * 0.75f);
	/* Text Box */

	/* Model Load button */
	init_button(&editor_gui->model_load_button, text_font, "Ok", rectangle_program, button_texture, ht_button_texture, 0.06f, 0.09f);
	set_button_position(&editor_gui->model_load_button, editor_gui->model_load_textbox.box.position.x + (editor_gui->model_load_textbox.box.dimensions.x / 2.0f) * 1.4f, editor_gui->model_load_textbox.box.position.y);
	/* Model Load button */

	init_transition_effect(&editor_gui->trn, max_move_count, transition_speed);
}

void handle_textinput_gui(EditorGUI *editor_gui, char c) {
	handle_text_input(&editor_gui->model_load_textbox, c);
}

void handle_key_input_gui(EditorGUI *editor_gui, int key) {
	handle_cursor_movement(&editor_gui->model_load_textbox, key);
}

void handle_mouse_click_gui(EditorGUI *editor_gui, Vector2 *norm_mouse_pos, int mouse_key) {
	handle_textbox_click(&editor_gui->model_load_textbox, norm_mouse_pos);
	button_clicked(&editor_gui->model_load_button, norm_mouse_pos);
}

void handle_transition_gui(EditorGUI *editor_gui, uint8_t on_x, uint8_t on_y) {
	if(editor_gui->trn.activate && !editor_gui->trn.in_transition)
		editor_gui->trn.in_transition = 1;

	if(editor_gui->trn.in_transition && editor_gui->trn.current_state == FORWARD) {
		if(editor_gui->trn.current_move_count <= editor_gui->trn.max_move_count) {
			translate_rectangle_2d_by(&editor_gui->background_left, editor_gui->trn.transition_speed * -on_x, editor_gui->trn.transition_speed * -on_y);
			translate_textbox_by(&editor_gui->model_load_textbox, editor_gui->trn.transition_speed * -on_x, editor_gui->trn.transition_speed * -on_y);
			translate_text_by(&editor_gui->model_load_text, editor_gui->trn.transition_speed * -on_x, editor_gui->trn.transition_speed * -on_y);
			translate_button_by(&editor_gui->model_load_button, editor_gui->trn.transition_speed * -on_x, editor_gui->trn.transition_speed * -on_y);
			editor_gui->trn.current_move_count += 1;
		}
		else {
			editor_gui->trn.in_transition = 0;
			editor_gui->trn.current_move_count = 0;
			editor_gui->trn.activate = 0;
			editor_gui->trn.current_state = REVERSE;
		}
	}
	else if(editor_gui->trn.in_transition && editor_gui->trn.current_state == REVERSE) {
		if(editor_gui->trn.current_move_count <= editor_gui->trn.max_move_count) {
			translate_rectangle_2d_by(&editor_gui->background_left, editor_gui->trn.transition_speed * on_x, editor_gui->trn.transition_speed * on_y);
			translate_textbox_by(&editor_gui->model_load_textbox, editor_gui->trn.transition_speed * on_x, editor_gui->trn.transition_speed * on_y);
			translate_text_by(&editor_gui->model_load_text, editor_gui->trn.transition_speed * on_x, editor_gui->trn.transition_speed * on_y);
			translate_button_by(&editor_gui->model_load_button, editor_gui->trn.transition_speed * on_x, editor_gui->trn.transition_speed * on_y);
			editor_gui->trn.current_move_count += 1;
		}
		else {
			editor_gui->trn.in_transition = 0;
			editor_gui->trn.current_move_count = 0;
			editor_gui->trn.activate = 0;
			editor_gui->trn.current_state = FORWARD;
		}
	}
}

void draw_editor_gui(EditorGUI *editor_gui, float current_time) {
	show_text(&editor_gui->model_load_text, text_program);
	draw_textbox(&editor_gui->model_load_textbox, text_program, current_time);
	draw_button(&editor_gui->model_load_button, text_program, current_time);
	draw_rectangle_2d(&editor_gui->background_left);
}
