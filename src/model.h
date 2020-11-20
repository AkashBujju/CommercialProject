#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include "math.h"

typedef struct Model {
	GLuint program, vao, vbo;
	Matrix4 model;
	Vector3 position, scale, rotation_axes;
	float angle_in_degree;
	float width, height, depth;
	uint32_t num_vertices;
} Model;

void load_model(Model *model, GLuint program, const char* model_filename);
void translate_model(Model *model, float x, float y, float z);
void scale_model(Model *model, float x, float y, float z);
void rotate_model(Model *model, float x, float y, float z, float degree);
void draw_model(Model *model);

#endif
