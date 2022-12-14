#version 450
layout(location=0) out vec4 FragColor;

in vec2 v_Position;
in vec4 v_Colour;

uniform float u_Percentage;

void main()
{
	float centre_distance = distance(v_Position, vec2(0.0f));

	vec4 outcolour = v_Colour;
	outcolour.a = 1 - step(u_Percentage, centre_distance * centre_distance);
	
	FragColor = outcolour;
}
