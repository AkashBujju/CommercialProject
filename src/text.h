#ifndef TEXT_H
#define TEXT_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include "math.h"

struct Character {
	unsigned int texture_id;
	unsigned int advance;
	Vector2 size;
	Vector2 bearing;
};
typedef struct Character Character;

struct Font {
	Character characters[128];
	unsigned int vao;
	unsigned int vbo;
};
typedef struct Font Font;

void init_freetype(FT_Library *ft);
void init_font(Font *font, uint16_t font_size, const char* filepath, FT_Library *ft);
Vector2 get_char_offset(Font *font, float scale, const char* text, int16_t index);
float get_text_width(Font *font, float scale, const char* text);
float get_text_height(Font *font, float scale, const char* text);
void render_text(Font *font, unsigned int program, const char* text, float x, float y, float scale, float r, float g, float b);

#endif
