#ifndef RECTANGLE_2D_H
#define RECTANGLE_2D_H

#include <glad/glad.h>
#include "../../src/math.h"

typedef struct Rectangle2D {
	GLuint vao, vbo, program;
	GLuint texture_id;
	Vector2 position;
	Vector2 dimensions;
} Rectangle2D;

void load_rectangle_2d(Rectangle2D *rectangle_2d, GLuint program, GLuint texture_id, float w, float h);
void translate_rectangle_2d(Rectangle2D *rct, float x, float y);
void draw_rectangle_2d(Rectangle2D *rectangle_2d);

#endif
