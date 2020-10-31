#include "model.h"
#include "utils.h"
#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

void load_model(Model *model, GLuint program, const char* model_filename) {
	model->program = program;

	FILE *file = fopen(model_filename, "r");
	if(!file) {
		printf("File %s not found\n", model_filename);
	}
	else {
		/* Retriving number of floats */
		uint32_t num_floats;
		fscanf(file, "%u", &num_floats);
		fclose(file);
		/* Retriving number of floats */

		/* @Note: Change this if normals and other info are added */
		/* 3 for the co-ordinates and 3 for the normals := 6 */
		model->num_vertices = num_floats / 6;

		float *vertices = (float*)malloc(sizeof(float) * num_floats);
		read_floats_from_file(model_filename, vertices);

		glGenVertexArrays(1, &model->vao);
		glGenBuffers(1, &model->vbo);
		glBindVertexArray(model->vao);
		glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * num_floats, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		free(vertices); /* @Note: Is this okay at this point? */

		init_vector(&model->scale, 1, 1, 1);
		init_vector(&model->position, 0, 0, 0);
		/* @Note: Change the below to custom values later */
		model->width = 1;
		model->height = 1;
		model->depth = 1;
	}
}

void translate_model(Model *model, float x, float y, float z) {
	model->position.x = x;
	model->position.y = y;
	model->position.z = z;
}

void scale_model(Model *model, float x, float y, float z) {
	model->scale.x = x;
	model->scale.y = y;
	model->scale.z = z;

	model->width *= x;
	model->height *= y;
	model->depth *= z;
}

void draw_model(Model *model) {
	glUseProgram(model->program);
	make_identity(&model->model);

	/* Translation & Scaling */
	translate_matrix(&model->model, model->position.x, model->position.y, model->position.z);
	scale(&model->model, model->scale.x, model->scale.y, model->scale.z);
	/* Translation & Scaling */

	set_matrix4(model->program, "model", &model->model);
	glBindVertexArray(model->vao);
	glDrawArrays(GL_TRIANGLES, 0, model->num_vertices);
}
