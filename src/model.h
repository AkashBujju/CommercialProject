#ifndef MODEL_H
#define MODEL_H

#include <glad\glad.h>
#include "math.h"

typedef struct Model {
	GLuint program;
	GLuint vao, vbo;
	Matrix4 model;
	Vector3 position;
	Vector3 scale;
	float width;
	float height;
	float depth;
	uint32_t num_vertices;
} Model;

void load_model(Model *model, GLuint program, const char* model_filename);
void translate_model(Model *model, float x, float y, float z);
void scale_model(Model *model, float x, float y, float z);
void draw_model(Model *model);

#endif
