#include "helper_models.h"
#include "../../src/utils.h"
#include "../../src/shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char* assets_path;

void load_instanced_helper_model(InstancedHelperModel *instanced_helper_model, GLuint program, const char* model_filename) {
	instanced_helper_model->program = program;
	instanced_helper_model->num_models = 0;

	FILE *file = fopen(model_filename, "r");
	if(!file) {
		printf("ERROR: File %s not found\n", model_filename);
		printf("ERROR: Not created model!!!\n");
	}
	else {
		/* @TODO: Come up with a better way to read the file */
		/* Reading num_floats and sub_model_count from file */
		uint32_t num_floats;
		uint16_t sub_model_count;
		char tmp[50];
		fscanf(file, "%s", tmp); /* sub_model_count string */
		fscanf(file, "%hu", &sub_model_count);
		fscanf(file, "%s", tmp); /* num_floats string */
		fscanf(file, "%u", &num_floats);
		fclose(file);
		/* Reading num_floats and sub_model_count from file */

		float *vertices = (float*)malloc(sizeof(float) * num_floats);
		Vector3 extreme_min, extreme_max;
		get_model_data_from_file(model_filename, &extreme_min, &extreme_max, vertices);

		float model_width = extreme_max.x - extreme_min.x;
		float model_height = extreme_max.y - extreme_min.y;
		float model_depth = extreme_max.z - extreme_min.z;

		/* @Note: Change this if normals and other info are added */
		/* 3 for the co-ordinates and 3 for the normals := 6 */
		instanced_helper_model->num_vertices = num_floats / 6;

		for(uint32_t i = 0; i < MAX_INSTANCED_HELPER_MODELS; ++i) {
		 	/* @Note: Change the below to custom values later */
		 	init_vector(&instanced_helper_model->scales[i], 1, 1, 1);
		 	init_vector(&instanced_helper_model->positions[i], 0, 0, 0);
		 	init_vector(&instanced_helper_model->colors[i], 0.5f, 0.5f, 0.5f);
			instanced_helper_model->bounding_boxes[i].width = model_width;
			instanced_helper_model->bounding_boxes[i].height = model_height;
			instanced_helper_model->bounding_boxes[i].depth = model_depth;
			make_identity(&instanced_helper_model->models[i]);
			/* @Note: Change the width, height and depth to be the bounding of the model */
		 	instanced_helper_model->angle_in_degree[i] = 0;
		 	init_vector(&instanced_helper_model->rotation_axes[i], 0, 0, 0);
		}

		/* Instance VBOs */
		glGenBuffers(1, &instanced_helper_model->instanceModelVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceModelVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4) * MAX_INSTANCED_HELPER_MODELS, instanced_helper_model->models, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &instanced_helper_model->instanceColorVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceColorVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * MAX_INSTANCED_HELPER_MODELS, instanced_helper_model->colors, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
		/* Instance VBOs */

		/* Position */
		glGenVertexArrays(1, &instanced_helper_model->vao);
		glGenBuffers(1, &instanced_helper_model->vbo);
		glBindVertexArray(instanced_helper_model->vao);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->vbo);
		/* @Note: Should we change this to dynamic draw later? */
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_floats, vertices, GL_STATIC_DRAW);
		/* @TODO: We are using only the 1st three floats from each vertex. So, should we make seperate files and load this from there */
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		/* Position */

		/* Instanced Model Attrib */
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceModelVBO);
    	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(2, 1);

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceModelVBO);
    	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(4 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceModelVBO);
    	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(8 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(4, 1);

		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceModelVBO);
    	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(12 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(5, 1);
		/* Instanced Model Attrib */

		/* Instanced Colors */
		glEnableVertexAttribArray(6);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceColorVBO);
    	glVertexAttribPointer(6, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(6, 1);
		/* Instanced Colors */
	}
}

void add_helper_model(InstancedHelperModel *instanced_helper_model, float x, float y, float z) {
	if(instanced_helper_model->num_models >= MAX_INSTANCED_HELPER_MODELS) {
		printf("WARNING: num_helper_models(%d) >= MAX_INSTANCED_HELPER_MODELS(%d)\n", instanced_helper_model->num_models, MAX_INSTANCED_HELPER_MODELS);
		printf("WARNING: Setting num_models to %d\n", MAX_INSTANCED_HELPER_MODELS);
	}
	else {
		instanced_helper_model->num_models += 1;
		translate_instanced_helper_model(instanced_helper_model, instanced_helper_model->num_models - 1, x, y, z);
	}
}

void set_color_instanced_helper_model(InstancedHelperModel *instanced_helper_model, uint32_t model_index, float r, float g, float b) {
	if(model_index >= instanced_helper_model->num_models) {
		printf("WARNING: In set_color_instanced_helper_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_helper_model->num_models);
	}
	else {
		instanced_helper_model->colors[model_index].x = r;
		instanced_helper_model->colors[model_index].y = g;
		instanced_helper_model->colors[model_index].z = b;

		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceColorVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector3) * instanced_helper_model->num_models, instanced_helper_model->colors);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void translate_instanced_helper_model(InstancedHelperModel *instanced_helper_model, uint32_t model_index, float x, float y, float z) {
	if(model_index >= instanced_helper_model->num_models) {
		printf("WARNING: In translate_instanced_helper_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_helper_model->num_models);
	}
	else {
		instanced_helper_model->positions[model_index].x = x;
		instanced_helper_model->positions[model_index].y = y;
		instanced_helper_model->positions[model_index].z = z;
	}
}

void scale_instanced_helper_model(InstancedHelperModel *instanced_helper_model, uint32_t model_index, float x, float y, float z) {
	if(model_index >= instanced_helper_model->num_models) {
		printf("WARNING: In scale_instanced_helper_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_helper_model->num_models);
	}
	else {
		instanced_helper_model->scales[model_index].x *= x;
		instanced_helper_model->scales[model_index].y *= y;
		instanced_helper_model->scales[model_index].z *= z;
	}
}

void rotate_instanced_helper_model(InstancedHelperModel *instanced_helper_model, uint32_t model_index, float x, float y, float z, float degree) {
	if(model_index >= instanced_helper_model->num_models) {
		printf("WARNING: In rotate_instanced_helper_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_helper_model->num_models);
	}
	else {
		instanced_helper_model->angle_in_degree[model_index] = degree;
		instanced_helper_model->rotation_axes[model_index].x = x;
		instanced_helper_model->rotation_axes[model_index].y = y;
		instanced_helper_model->rotation_axes[model_index].z = z;
	}
}

static void make_glsl_string(char *des, uint16_t index, char *var_name, char *member_name) {
	strcpy(des, var_name);
	strcat(des, "[");

	char index_str[15];
	sprintf(index_str, "%u", index);

	strcat(des, index_str);
	strcat(des, "].");
	strcat(des, member_name);
}

void draw_instanced_helper_model(InstancedHelperModel *instanced_helper_model, InstancedDirLight *instanced_dir_light) {
	glUseProgram(instanced_helper_model->program);

	/* Updating in-struct Model Matrix */
	for(uint32_t i = 0; i < instanced_helper_model->num_models; ++i) {
		make_identity(&instanced_helper_model->models[i]);

		translate_matrix(&instanced_helper_model->models[i], instanced_helper_model->positions[i].x, instanced_helper_model->positions[i].y, instanced_helper_model->positions[i].z);
		scale(&instanced_helper_model->models[i], instanced_helper_model->scales[i].x, instanced_helper_model->scales[i].y, instanced_helper_model->scales[i].z);
		Vector3 tmp_position;
		init_vector(&tmp_position, 
				instanced_helper_model->positions[i].x, 
				instanced_helper_model->positions[i].y, 
				instanced_helper_model->positions[i].z);
		translate_matrix(&instanced_helper_model->models[i], 0, 0, 0);
		rotate(&instanced_helper_model->models[i], &instanced_helper_model->rotation_axes[i], instanced_helper_model->angle_in_degree[i]);
		translate_matrix(&instanced_helper_model->models[i], tmp_position.x, tmp_position.y, tmp_position.z);
	}
	/* Updating in-struct Model Matrix */

	/* Updating ModelVBO */
	glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceModelVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Matrix4) * instanced_helper_model->num_models, instanced_helper_model->models);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/* Updating ModelVBO */

	/* Directional Lights */
	set_integer(instanced_helper_model->program, "num_dir_lights", instanced_dir_light->num_models);
	for(uint32_t di = 0; di < instanced_dir_light->num_models; ++di) {
		char var_name[50];
		make_glsl_string(var_name, di, "dir_lights", "position");
		set_vector3(instanced_helper_model->program, var_name, &instanced_dir_light->positions[di]);
	}
	/* Directional Lights */

	glBindVertexArray(instanced_helper_model->vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, instanced_helper_model->num_vertices, instanced_helper_model->num_models);
}
