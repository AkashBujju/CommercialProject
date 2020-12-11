#include "model_properties_gui.h"

extern GLuint rectangle_program;
extern GLuint text_program;
extern GLuint model_properties_background_texture;
extern GLuint button_texture;
extern GLuint ht_button_texture;
extern GLuint close_button_texture;
extern GLuint move_tag_texture;
extern GLuint check_texture;

void init_model_properties_gui(ModelPropertiesGUI *model_properties_gui, Font *small_font, Font *medium_font, Font *big_font) {
	model_properties_gui->show = 0;

	/* Move tag */
	load_rectangle_2d(&model_properties_gui->move_tag, rectangle_program, move_tag_texture, 0.06f, 0.06f);
	translate_rectangle_2d(&model_properties_gui->move_tag, 0, 0);
	/* Move tag */

	/* Background */
	load_rectangle_2d(&model_properties_gui->background, rectangle_program, model_properties_background_texture, 0.5f, 0.7f);
	translate_rectangle_2d(&model_properties_gui->background, model_properties_gui->move_tag.position.x + model_properties_gui->background.dimensions.x / 2.0f - model_properties_gui->move_tag.dimensions.x / 2.0f, model_properties_gui->move_tag.position.y - model_properties_gui->background.dimensions.y / 2.0f);
	/* Background */

	/* Close Button */
	init_button(&model_properties_gui->close_button, medium_font, "X", rectangle_program, close_button_texture, ht_button_texture, 0.04f, 0.06f);
	set_button_position(&model_properties_gui->close_button, model_properties_gui->background.position.x + (model_properties_gui->background.dimensions.x / 2.0f) * 0.95f - model_properties_gui->close_button.box.dimensions.x / 2.0f, model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.95f - model_properties_gui->close_button.box.dimensions.y / 2.0f);
	/* Close Button */

	/* Model Name */
	init_text(&model_properties_gui->model_name_text, medium_font, "name: na", 0, 0, 1, 1, 0);
	set_text_position(&model_properties_gui->model_name_text, model_properties_gui->background.position.x - (model_properties_gui->model_name_text.normalized_dims.x / 2.0f), model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.85f);
	/* Model Name */

	/* Model Index */
	init_text(&model_properties_gui->model_index_text, medium_font, "index: na", 0, 0, 1, 1, 0);
	set_text_position(&model_properties_gui->model_index_text, model_properties_gui->background.position.x - (model_properties_gui->model_index_text.normalized_dims.x / 2.0f), model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.7f);
	/* Model Index */

	/* Model location indicator */
	init_text(&model_properties_gui->model_location_indicator_text, small_font, "location", 0, 0, 1, 1, 0);
	set_text_position(&model_properties_gui->model_location_indicator_text, model_properties_gui->background.position.x - (model_properties_gui->background.dimensions.x / 2.0f) * 0.9f, model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.45f);
	/* Model location indicator */

	/* Model location */
	init_text(&model_properties_gui->model_location_text, medium_font, "na", 0, 0, 1, 1, 0);
	set_text_position(&model_properties_gui->model_location_text, model_properties_gui->background.position.x - (model_properties_gui->background.dimensions.x / 2.0f) * 0.9f, model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.30f);
	/* Model location */

	/* Check location indicator */
	init_text(&model_properties_gui->check_location_indicator_text, small_font, "translate", 0, 0, 1, 1, 0);
	set_text_position(&model_properties_gui->check_location_indicator_text, model_properties_gui->background.position.x - (model_properties_gui->background.dimensions.x / 2.0f) * 0.9f, model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.1f);
	/* Check location indicator */

	/* Check translation */
	init_checkbox(&model_properties_gui->check_translation, rectangle_program, ht_button_texture, check_texture, 0.04f, 0.06f);
	translate_checkbox(&model_properties_gui->check_translation, model_properties_gui->background.position.x - (model_properties_gui->background.dimensions.x / 2.0f) * 0.9f + model_properties_gui->check_translation.box.dimensions.x / 2.0f, model_properties_gui->background.position.y - (model_properties_gui->background.dimensions.y / 2.0f) * 0.05f);
	/* Check translation */

	/* Check scale indicator */
	init_text(&model_properties_gui->check_scale_indicator_text, small_font, "scale", 0, 0, 1, 1, 0);
	set_text_position(&model_properties_gui->check_scale_indicator_text, model_properties_gui->background.position.x - (model_properties_gui->background.dimensions.x / 2.0f) * 0.5f, model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.1f);
	/* Check scale indicator */

	/* Check scale */
	init_checkbox(&model_properties_gui->check_scale, rectangle_program, ht_button_texture, check_texture, 0.04f, 0.06f);
	translate_checkbox(&model_properties_gui->check_scale, model_properties_gui->background.position.x - (model_properties_gui->background.dimensions.x / 2.0f) * 0.5f + model_properties_gui->check_scale.box.dimensions.x / 2.0f, model_properties_gui->background.position.y - (model_properties_gui->background.dimensions.y / 2.0f) * 0.05f);
	/* Check scale */
}

static void update_model_properties_location_text(ModelPropertiesGUI *model_properties_gui, float norm_x, float norm_y, float norm_z) {
	char pos[50], tmp[10];
	sprintf(tmp, "%+.3f", norm_x);
	strcpy(pos, tmp);
	strcat(pos, " ");
	sprintf(tmp, "%+.3f", norm_y);
	strcat(pos, tmp);
	strcat(pos, " ");
	sprintf(tmp, "%+.3f", norm_z);
	strcat(pos, tmp);
	strcat(pos, " ");

	strcpy(model_properties_gui->model_location_text.text, pos);
	update_text(&model_properties_gui->model_location_text);
}

void set_instanced_model_info_to_properties_gui(ModelPropertiesGUI *model_properties_gui, InstancedModel *instanced_model, uint32_t model_index) {
	/* Model Name */
	char model_name[50];
	strcpy(model_name, "name: ");
	strcat(model_name, instanced_model->model_name);
	strcpy(model_properties_gui->model_name_text.text, model_name);
	update_text(&model_properties_gui->model_name_text);
	set_text_position(&model_properties_gui->model_name_text, model_properties_gui->background.position.x - (model_properties_gui->model_name_text.normalized_dims.x / 2.0f), model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.85f);
	/* Model Name */

	/* Model Index */
	char mi[50], model_index_str[10];
	sprintf(model_index_str, "%d", model_index);
	strcpy(mi, "index: ");
	strcat(mi, model_index_str);
	strcpy(model_properties_gui->model_index_text.text, mi);
	update_text(&model_properties_gui->model_index_text);
	set_text_position(&model_properties_gui->model_index_text, model_properties_gui->background.position.x - (model_properties_gui->model_index_text.normalized_dims.x / 2.0f), model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.7f);
	/* Model Index */

	/* Model location */
	update_model_properties_location_text(model_properties_gui, instanced_model->positions[model_index].x, instanced_model->positions[model_index].y, instanced_model->positions[model_index].z);
	/* Model location */
}

void translate_move_tag_and_update_properties_gui(ModelPropertiesGUI *model_properties_gui, float norm_x, float norm_y) {
	if(model_properties_gui->moving) {
		translate_rectangle_2d(&model_properties_gui->move_tag, norm_x, norm_y);
		translate_rectangle_2d(&model_properties_gui->background, model_properties_gui->move_tag.position.x + model_properties_gui->background.dimensions.x / 2.0f - model_properties_gui->move_tag.dimensions.x / 2.0f, model_properties_gui->move_tag.position.y - model_properties_gui->background.dimensions.y / 2.0f);
		set_button_position(&model_properties_gui->close_button, model_properties_gui->background.position.x + (model_properties_gui->background.dimensions.x / 2.0f) * 0.95f - model_properties_gui->close_button.box.dimensions.x / 2.0f, model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.95f - model_properties_gui->close_button.box.dimensions.y / 2.0f);
		set_text_position(&model_properties_gui->model_name_text, model_properties_gui->background.position.x - (model_properties_gui->model_name_text.normalized_dims.x / 2.0f), model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.85f);
		set_text_position(&model_properties_gui->model_index_text, model_properties_gui->background.position.x - (model_properties_gui->model_index_text.normalized_dims.x / 2.0f), model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.7f);
		set_text_position(&model_properties_gui->model_location_indicator_text, model_properties_gui->background.position.x - (model_properties_gui->background.dimensions.x / 2.0f) * 0.9f, model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.45f);
		set_text_position(&model_properties_gui->model_location_text, model_properties_gui->background.position.x - (model_properties_gui->background.dimensions.x / 2.0f) * 0.9f, model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.30f);
		set_text_position(&model_properties_gui->check_location_indicator_text, model_properties_gui->background.position.x - (model_properties_gui->background.dimensions.x / 2.0f) * 0.9f, model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.1f);
		translate_checkbox(&model_properties_gui->check_translation, model_properties_gui->background.position.x - (model_properties_gui->background.dimensions.x / 2.0f) * 0.9f + model_properties_gui->check_translation.box.dimensions.x / 2.0f, model_properties_gui->background.position.y - (model_properties_gui->background.dimensions.y / 2.0f) * 0.05f);
		set_text_position(&model_properties_gui->check_scale_indicator_text, model_properties_gui->background.position.x - (model_properties_gui->background.dimensions.x / 2.0f) * 0.5f, model_properties_gui->background.position.y + (model_properties_gui->background.dimensions.y / 2.0f) * 0.1f);
		translate_checkbox(&model_properties_gui->check_scale, model_properties_gui->background.position.x - (model_properties_gui->background.dimensions.x / 2.0f) * 0.5f + model_properties_gui->check_scale.box.dimensions.x / 2.0f, model_properties_gui->background.position.y - (model_properties_gui->background.dimensions.y / 2.0f) * 0.05f);
	}
}

void handle_mouse_movement_properties_gui(ModelPropertiesGUI *model_properties_gui, Vector2 *mouse_norm_pos) {
	if(model_properties_gui->show) {
		uint8_t clicked = in_rect(mouse_norm_pos, &model_properties_gui->move_tag.position, &model_properties_gui->move_tag.dimensions);
		if(clicked)
			model_properties_gui->moving = !model_properties_gui->moving;
	}
}

void handle_mouse_click_properties_gui(ModelPropertiesGUI *model_properties_gui, Vector2 *mouse_norm_pos) {
	if(model_properties_gui->show) {
		button_clicked(&model_properties_gui->close_button, mouse_norm_pos);
		uint8_t hit_trans = checkbox_clicked(&model_properties_gui->check_translation, mouse_norm_pos);
		uint8_t hit_scale = checkbox_clicked(&model_properties_gui->check_scale, mouse_norm_pos);

		if(hit_trans)
			model_properties_gui->check_scale.checked = 0;
		else if(hit_scale)
			model_properties_gui->check_translation.checked = 0;
	}
}

void draw_model_properties_gui(ModelPropertiesGUI *model_properties_gui, float current_time) {
	if(model_properties_gui->show) {
		show_text(&model_properties_gui->model_name_text, text_program);
		show_text(&model_properties_gui->model_index_text, text_program);
		show_text(&model_properties_gui->model_location_text, text_program);
		show_text(&model_properties_gui->model_location_indicator_text, text_program);
		show_text(&model_properties_gui->check_location_indicator_text, text_program);
		show_text(&model_properties_gui->check_scale_indicator_text, text_program);
		draw_checkbox(&model_properties_gui->check_translation);
		draw_checkbox(&model_properties_gui->check_scale);
		draw_button(&model_properties_gui->close_button, text_program, current_time);
		draw_rectangle_2d(&model_properties_gui->background);
		draw_rectangle_2d(&model_properties_gui->move_tag);
	}
}
