#version 450
layout(location=0) out vec4 FragColor;

in vec4 v_Colour;
in vec2 vTexCoord;

void main()
{
	FragColor = v_Colour;
}
