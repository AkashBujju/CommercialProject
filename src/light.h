#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include "math.h"

typedef struct Light {
	GLuint program;
	GLuint vao, vbo;
	Matrix4 model;
	Vector3 position;
	Vector3 scale;
	float width;
	float height;
	float depth;
	uint32_t num_vertices;
} Light;

void load_light(Light *light, GLuint program, const char* model_filename);
void translate_light(Light *light, float x, float y, float z);
void scale_light(Light *light, float x, float y, float z);
void draw_light(Light *light);

#endif
