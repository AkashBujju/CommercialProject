#include "model_loader_gui.h"

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

void init_model_loader_gui(ModelLoaderGUI *model_loader_gui, Font *small_font, Font *medium_font, Font *big_font, float transition_speed, uint16_t max_move_count) {
	/* Rectangle2D */
	load_rectangle_2d(&model_loader_gui->background_left, rectangle_program, background_left_texture, 0.4f, 1.94f);
	translate_rectangle_2d(&model_loader_gui->background_left, -0.98f + model_loader_gui->background_left.dimensions.x / 2.0f, 0);
	/* Rectangle2D */

	/* Text */
	init_text(&model_loader_gui->model_load_text, medium_font, "", -0.25f, +0.5f, 1, 1, 0);
	strcpy(model_loader_gui->model_load_text.text, "Load Model");
	update_text(&model_loader_gui->model_load_text);
	set_text_position(&model_loader_gui->model_load_text, model_loader_gui->background_left.position.x - model_loader_gui->model_load_text.normalized_dims.x / 2.0f, (model_loader_gui->background_left.position.y + model_loader_gui->background_left.dimensions.y / 2) * 0.9f);
	/* Text */

	/* Text Box */
	init_textbox(&model_loader_gui->model_load_textbox, medium_font, "", rectangle_program, box_texture, ht_box_texture, cursor_texture);
	set_textbox_position(&model_loader_gui->model_load_textbox, model_loader_gui->background_left.position.x - model_loader_gui->background_left.dimensions.x * 0.42f + model_loader_gui->model_load_textbox.box.dimensions.x * 0.5f, model_loader_gui->background_left.position.y + (model_loader_gui->background_left.dimensions.y / 2) * 0.75f);
	/* Text Box */

	/* Text */
	init_text(&model_loader_gui->model_load_textbox_indicator_text, small_font, "", -0.25f, +0.5f, 1, 1, 0);
	strcpy(model_loader_gui->model_load_textbox_indicator_text.text, "model name");
	update_text(&model_loader_gui->model_load_textbox_indicator_text);
	set_text_position(&model_loader_gui->model_load_textbox_indicator_text, model_loader_gui->model_load_textbox.box.position.x - (model_loader_gui->model_load_textbox.box.dimensions.x / 2.0f) * 0.95f, model_loader_gui->model_load_textbox.box.position.y + (model_loader_gui->model_load_textbox.box.dimensions.y / 2.0f) * 1.2f);
	/* Text */

	/* Model Load button */
	init_button(&model_loader_gui->model_load_button, medium_font, "Ok", rectangle_program, button_texture, ht_button_texture, 0.06f, 0.09f);
	set_button_position(&model_loader_gui->model_load_button, model_loader_gui->model_load_textbox.box.position.x + (model_loader_gui->model_load_textbox.box.dimensions.x / 2.0f) * 1.4f, model_loader_gui->model_load_textbox.box.position.y);
	/* Model Load button */

	init_transition_effect(&model_loader_gui->trn, max_move_count, transition_speed);
}

void handle_textinput_gui(ModelLoaderGUI *model_loader_gui, char c) {
	handle_text_input(&model_loader_gui->model_load_textbox, c);
}

void handle_key_input_gui(ModelLoaderGUI *model_loader_gui, int key) {
	handle_cursor_movement(&model_loader_gui->model_load_textbox, key);
}

void handle_mouse_click_gui(ModelLoaderGUI *model_loader_gui, Vector2 *norm_mouse_pos) {
	handle_textbox_click(&model_loader_gui->model_load_textbox, norm_mouse_pos);
	button_clicked(&model_loader_gui->model_load_button, norm_mouse_pos);
}

void handle_transition_gui(ModelLoaderGUI *model_loader_gui, uint8_t on_x, uint8_t on_y) {
	if(model_loader_gui->trn.activate && !model_loader_gui->trn.in_transition)
		model_loader_gui->trn.in_transition = 1;

	if(model_loader_gui->trn.in_transition && model_loader_gui->trn.current_state == FORWARD) {
		if(model_loader_gui->trn.current_move_count <= model_loader_gui->trn.max_move_count) {
			translate_rectangle_2d_by(&model_loader_gui->background_left, model_loader_gui->trn.transition_speed * -on_x, model_loader_gui->trn.transition_speed * -on_y);
			translate_textbox_by(&model_loader_gui->model_load_textbox, model_loader_gui->trn.transition_speed * -on_x, model_loader_gui->trn.transition_speed * -on_y);
			translate_text_by(&model_loader_gui->model_load_text, model_loader_gui->trn.transition_speed * -on_x, model_loader_gui->trn.transition_speed * -on_y);
			translate_text_by(&model_loader_gui->model_load_textbox_indicator_text, model_loader_gui->trn.transition_speed * -on_x, model_loader_gui->trn.transition_speed * -on_y);
			translate_button_by(&model_loader_gui->model_load_button, model_loader_gui->trn.transition_speed * -on_x, model_loader_gui->trn.transition_speed * -on_y);
			model_loader_gui->trn.current_move_count += 1;
		}
		else {
			model_loader_gui->trn.in_transition = 0;
			model_loader_gui->trn.current_move_count = 0;
			model_loader_gui->trn.activate = 0;
			model_loader_gui->trn.current_state = REVERSE;
		}
	}
	else if(model_loader_gui->trn.in_transition && model_loader_gui->trn.current_state == REVERSE) {
		if(model_loader_gui->trn.current_move_count <= model_loader_gui->trn.max_move_count) {
			translate_rectangle_2d_by(&model_loader_gui->background_left, model_loader_gui->trn.transition_speed * on_x, model_loader_gui->trn.transition_speed * on_y);
			translate_textbox_by(&model_loader_gui->model_load_textbox, model_loader_gui->trn.transition_speed * on_x, model_loader_gui->trn.transition_speed * on_y);
			translate_text_by(&model_loader_gui->model_load_text, model_loader_gui->trn.transition_speed * on_x, model_loader_gui->trn.transition_speed * on_y);
			translate_text_by(&model_loader_gui->model_load_textbox_indicator_text, model_loader_gui->trn.transition_speed * on_x, model_loader_gui->trn.transition_speed * -on_y);
			translate_button_by(&model_loader_gui->model_load_button, model_loader_gui->trn.transition_speed * on_x, model_loader_gui->trn.transition_speed * on_y);
			model_loader_gui->trn.current_move_count += 1;
		}
		else {
			model_loader_gui->trn.in_transition = 0;
			model_loader_gui->trn.current_move_count = 0;
			model_loader_gui->trn.activate = 0;
			model_loader_gui->trn.current_state = FORWARD;
		}
	}
}

void draw_model_loader_gui(ModelLoaderGUI *model_loader_gui, float current_time) {
	show_text(&model_loader_gui->model_load_text, text_program);
	show_text(&model_loader_gui->model_load_textbox_indicator_text, text_program);
	draw_textbox(&model_loader_gui->model_load_textbox, text_program, current_time);
	draw_button(&model_loader_gui->model_load_button, text_program, current_time);
	draw_rectangle_2d(&model_loader_gui->background_left);
}
