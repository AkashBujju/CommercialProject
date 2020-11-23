#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexturePos;
out vec2 TexCoords;

uniform vec2 pos;

void main() {
	gl_Position = vec4(aPos.xy + pos, 0.0, 1.0);
	TexCoords = aTexturePos;
}
