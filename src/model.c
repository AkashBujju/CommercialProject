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

		model->num_vertices = num_floats / 3; /* @Note: Change this if normals and other info are added */

		float *vertices = (float*)malloc(sizeof(float) * num_floats);
		read_floats_from_file(model_filename, vertices);

		glGenVertexArrays(1, &model->vao);
		glGenBuffers(1, &model->vbo);
		glBindVertexArray(model->vao);
		glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * num_floats, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		free(vertices);
	}
}

void draw_model(Model *model) {
	glUseProgram(model->program);
	make_identity(&model->model);
	set_matrix4(model->program, "model", &model->model);
	glBindVertexArray(model->vao);
	glDrawArrays(GL_TRIANGLES, 0, model->num_vertices);
}
