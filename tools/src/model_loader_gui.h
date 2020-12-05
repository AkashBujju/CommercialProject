#ifndef MODEL_LOADER_GUI_H
#define MODEL_LOADER_GUI_H

#include "rectangle_2d.h"
#include "text_box.h"
#include "button.h"
#include "text.h"

enum TransitionState {
	FORWARD, REVERSE
};

typedef struct TransitionEffect {
	uint8_t in_transition;
	uint8_t activate;
	uint16_t max_move_count;
	uint16_t current_move_count;
	float transition_speed;
	enum TransitionState current_state;
} TransitionEffect;

typedef struct ModelLoaderGUI {
	Rectangle2D background_left;
	Text model_load_text;
	Text model_load_textbox_indicator_text;
	Button model_load_button;
	TextBox model_load_textbox;
	TransitionEffect trn;
} ModelLoaderGUI;

void init_model_loader_gui(ModelLoaderGUI *model_loader_gui, Font *small_font, Font *medium_font, Font *big_font, float transition_speed, uint16_t max_move_count);
void draw_model_loader_gui(ModelLoaderGUI *model_loader_gui, float current_time);
void handle_mouse_click_gui(ModelLoaderGUI *model_loader_gui, Vector2 *norm_mouse_pos);
void handle_keyinput_gui(ModelLoaderGUI *model_loader_gui, char c);
void handle_textinput_gui(ModelLoaderGUI *model_loader_gui, char c);
void handle_key_input_gui(ModelLoaderGUI *model_loader_gui, int key);
void handle_transition_gui(ModelLoaderGUI *model_loader_gui, uint8_t on_x, uint8_t on_y);

#endif
