#version 450
in vec3 a_Position;
in vec4 a_Colour;
in vec2 a_TexCoord;

uniform mat4x4 a_WorldMatrix;
uniform mat4x4 a_CameraMatrix;
uniform mat4x4 a_ProjMatrix;

out vec4 v_Colour;
out vec2 vTexCoord;

void main()
{
	vec4 coords = vec4(a_Position, 1);
	vec4 result = a_ProjMatrix * a_CameraMatrix * a_WorldMatrix * coords;
	gl_Position = result;

	v_Colour = a_Colour;
	vTexCoord = a_TexCoord;
}
