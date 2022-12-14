#version 450
layout(location=0) out vec4 FragColor;

in vec4 v_Position;
in vec4 v_Colour;

uniform float u_Percentage;
uniform float u_Threshold;

void main()
{
	

	FragColor = v_Colour;
}
