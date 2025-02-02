#version 330 core

layout (location = 0) in vec3 a_Pos;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
   vec4 localPos = vec4(a_Pos.x, a_Pos.y, a_Pos.z, 1.0);
   gl_Position = Projection * View * Model * localPos;
}
