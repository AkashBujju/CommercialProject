#ifndef MODEL_PROPERTIES_GUI_H
#define MODEL_PROPERTIES_GUI_H

#include "../../src/instanced_model.h"
#include "rectangle_2d.h"
#include "text_box.h"
#include "button.h"
#include "text.h"
#include "check_box.h"

typedef struct ModelPropertiesGUI {
	Rectangle2D background;
	Rectangle2D move_tag;
	Text model_name_text;
	Text model_index_text;
	Text model_location_indicator_text;
	Text model_location_text;
	Text check_location_indicator_text;
	Text check_scale_indicator_text;
	CheckBox check_translation;
	CheckBox check_scale;
	Button close_button;

	uint8_t show;
	uint8_t moving;
} ModelPropertiesGUI;

void set_instanced_model_info_to_properties_gui(ModelPropertiesGUI *model_properties_gui, InstancedModel *instanced_model, uint32_t model_index);
void handle_mouse_click_properties_gui(ModelPropertiesGUI *model_properties_gui, Vector2 *mouse_norm_pos);
void handle_mouse_movement_properties_gui(ModelPropertiesGUI *model_properties_gui, Vector2 *mouse_norm_pos);
void translate_move_tag_and_update_properties_gui(ModelPropertiesGUI *model_properties_gui, float norm_x, float norm_y);
void init_model_properties_gui(ModelPropertiesGUI *model_properties_gui, Font *small_font, Font *medium_font, Font *big_font);
void draw_model_properties_gui(ModelPropertiesGUI *model_properties_gui, float current_time);

#endif
