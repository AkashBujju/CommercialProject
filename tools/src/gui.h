#ifndef GUI_H
#define GUI_H

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

typedef struct EditorGUI {
	Rectangle2D background_left;
	Text model_load_text;
	Button model_load_button;
	TextBox model_load_textbox;
	TransitionEffect trn;
} EditorGUI;

void init_editor_gui(EditorGUI *editor_gui, Font *text_font, float transition_speed, uint16_t max_move_count);
void draw_editor_gui(EditorGUI *editor_gui, float current_time);
void handle_mouse_click_gui(EditorGUI *editor_gui, Vector2 *norm_mouse_pos, int mouse_key);
void handle_keyinput_gui(EditorGUI *editor_gui, char c);
void handle_textinput_gui(EditorGUI *editor_gui, char c);
void handle_key_input_gui(EditorGUI *editor_gui, int key);
void handle_transition_gui(EditorGUI *editor_gui, uint8_t on_x, uint8_t on_y);

#endif
