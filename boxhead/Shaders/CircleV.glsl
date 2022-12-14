#version 450
in vec3 a_Position;
in vec4 a_Colour;

out vec4 v_Position;
out vec4 v_Colour;

uniform float u_Ratio = 1.0f;

void main()
{
	vec4 coords = vec4(a_Position, 1);
	coords.y /= u_Ratio;

	gl_Position = coords;

	v_Position = a_Position;
	v_Colour = a_Colour;
}
