#ifndef TOOLS_TEXT_H
#define TOOLS_TEXT_H

#define TEXT_LIMIT 256

#include <glad/glad.h>
#include "../../src/text.h"
#include <stdint.h>

typedef struct Text {
	Font *font;
	char text[TEXT_LIMIT];
	uint16_t length;
	Vector2 position;
	Vector2 normalized_dims;
	Vector3 color;
} Text;

#undef TEXT_LIMIT

void init_text(Text *text, Font *font, char* string, float norm_x, float norm_y, float r, float g, float b);
void show_text(Text *text, GLuint program);
void update_text(Text *text);
void translate_text_by(Text *text, float norm_dx, float norm_dy);
void set_text_position(Text *text, float norm_x, float norm_y);

#endif
