#version 450
layout(location=0) out vec4 FragColor;

uniform sampler2D u_Texture;

in vec4 v_Colour;
in vec2 vTexCoord;

void main()
{
	FragColor = texture(u_Texture, vTexCoord);
}
