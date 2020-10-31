#ifndef MODEL_H
#define MODEL_H

#include <glad\glad.h>
#include "math.h"

typedef struct Model {
	GLuint program;
	GLuint vao, vbo;
	Matrix4 model;
	uint32_t num_vertices;
} Model;

void load_model(Model *model, GLuint program, const char* model_filename);
void draw_model(Model *model);

#endif
