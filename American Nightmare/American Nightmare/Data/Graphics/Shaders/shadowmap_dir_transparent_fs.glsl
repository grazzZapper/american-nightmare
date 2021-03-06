#version 440 core

uniform sampler2D texture;

in vec2 textureUV;

void main () {

	vec4 color = texture2D(texture, vec2(textureUV.x, 1.f - textureUV.y));

	if (color.a < 1.f)
		discard;
}