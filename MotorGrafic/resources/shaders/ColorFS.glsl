#version 330 core

out vec4 FragColor;

uniform vec3 u_color;

void main()
{
	FragColor = vec4(u_color, 1.0f);
	//FragColor = vec4(1,1,1, 1.0f);
}