#include "instanced_model.h"
#include "utils.h"
#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_instanced_model(InstancedModel *instanced_model, GLuint program, const char* model_filename, uint32_t num_models) {
	if(num_models >= MAX_MODELS) {
		printf("WARNING: num_models(%d) >= MAX_MODELS(%d)\n", num_models, MAX_MODELS);
		printf("WARNING: Setting num_models to %d\n", MAX_MODELS);
		num_models = MAX_MODELS;
	}

	instanced_model->program = program;
	instanced_model->num_models = num_models;

	FILE *file = fopen(model_filename, "r");
	if(!file) {
		printf("ERROR: File %s not found\n", model_filename);
		printf("ERROR: Not created model!!!\n");
	}
	else {
		/* Retriving number of floats */
		uint32_t num_floats;
		fscanf(file, "%u", &num_floats);
		fclose(file);
		/* Retriving number of floats */

		/* @Note: Change this if normals and other info are added */
		/* 3 for the co-ordinates and 3 for the normals := 6 */
		instanced_model->num_vertices = num_floats / 6;

		float *vertices = (float*)malloc(sizeof(float) * num_floats);
		read_floats_from_file(model_filename, vertices);

		glGenVertexArrays(1, &instanced_model->vao);
		glGenBuffers(1, &instanced_model->vbo);
		glBindVertexArray(instanced_model->vao);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_floats, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		/* @Note: Is this okay at this point? */
		/* free(vertices); */

		for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
			/* @Note: Change the below to custom values later */
			init_vector(&instanced_model->scales[i], 1, 1, 1);
			init_vector(&instanced_model->positions[i], 0, 0, 0);
			instanced_model->width[i] = 1;
			instanced_model->height[i] = 1;
			instanced_model->depth[i] = 1;
			instanced_model->angle_in_degree[i] = 0;
			init_vector(&instanced_model->colors[i], 1, 1, 1);
			init_vector(&instanced_model->rotation_axes[i], 0, 0, 0);
		}
	}
}

void translate_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z) {
	if(model_index >= instanced_model->num_models) {
		printf("WARNING: In translate_model(): model_index(%d) >= num_models(%d)\n", model_index, instanced_model->num_models);
	}
	else {
		instanced_model->positions[model_index].x = x;
		instanced_model->positions[model_index].y = y;
		instanced_model->positions[model_index].z = z;
	}
}

void set_color_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float r, float g, float b) {
	if(model_index >= instanced_model->num_models) {
		printf("WARNING: In set_color_instanced_model(): model_index(%d) >= num_models(%d)\n", model_index, instanced_model->num_models);
	}
	else {
		instanced_model->colors[model_index].x = r;
		instanced_model->colors[model_index].y = g;
		instanced_model->colors[model_index].z = b;
	}
}

void draw_instanced_model(InstancedModel *instanced_model) {
	glUseProgram(instanced_model->program);

	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		make_identity(&instanced_model->models[i]);
	}

	/* Translation & Scaling */
	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		translate_matrix(&instanced_model->models[i], instanced_model->positions[i].x,
				instanced_model->positions[i].y, instanced_model->positions[i].z);
	}

	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		scale(&instanced_model->models[i], instanced_model->scales[i].x,
				instanced_model->scales[i].y, instanced_model->scales[i].z);
	}
	/* Translation & Scaling */

	/* Rotation */
	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		Vector3 tmp;
		init_vector(&tmp, instanced_model->positions[i].x, instanced_model->positions[i].y, instanced_model->positions[i].z);
		translate_matrix(&instanced_model->models[i], 0, 0, 0);
		rotate(&instanced_model->models[i], &instanced_model->rotation_axes[i], instanced_model->angle_in_degree[i]);
		translate_matrix(&instanced_model->models[i], tmp.x, tmp.y, tmp.z);
	}
	/* Rotation */

	/* Setting the Model matrix */
	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		char model_name[15], index[5];
		sprintf(index, "%d", i);
		strcpy(model_name, "models[");
		strcat(model_name, index);
		strcat(model_name, "]");
		set_matrix4(instanced_model->program, model_name, &instanced_model->models[i]);
	}
	/* Setting the Model matrix */

	/* Setting the color */
	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		char color_name[30], index[5];
		sprintf(index, "%d", i);
		strcpy(color_name, "objectColors[");
		strcat(color_name, index);
		strcat(color_name, "]");
		set_vector3(instanced_model->program, color_name, &instanced_model->colors[i]);
	}
	/* Setting the color */

	glBindVertexArray(instanced_model->vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, instanced_model->num_vertices, instanced_model->num_models);
}
