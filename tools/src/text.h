#ifndef TOOLS_TEXT_H
#define TOOLS_TEXT_H

#include <glad/glad.h>
#include "../../src/text.h"
#include <stdint.h>

typedef struct Text {
	Font *font;
	char text[256];
	uint16_t length;
	Vector2 position;
	Vector2 normalized_dims;
	Vector3 color;
} Text;

void init_text(Text *text, Font *font, char* string, float norm_x, float norm_y, float r, float g, float b);
void show_text(Text *text, Font *font, GLuint program);
void set_text(Text *text, char* string);
void set_text_position(Text *text, float norm_x, float norm_y);

#endif
