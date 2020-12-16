#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include "../../src/instanced_model.h"

#define MAX_EXP_LINES 100

typedef struct ExpLines {
	GLuint program, vao[MAX_EXP_LINES];
	uint32_t num_lines;
} ExpLines;

void draw_exp_lines(ExpLines *exp_lines);
void add_exp_line(ExpLines *exp_lines, float x1, float y1, float z1, float x2, float y2, float z2, float r, float g, float b);
void init_exp_lines(ExpLines *exp_lines);

#endif
