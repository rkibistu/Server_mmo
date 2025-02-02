#version 330 core

layout (location = 0) in vec3 a_Pos;
layout (location = 1) in vec2 a_texCoord;

uniform mat4 u_mvp;

varying vec2 v_texCoord;

void main()
{
   vec4 localPos = vec4(a_Pos.x, a_Pos.y, a_Pos.z, 1.0);
   gl_Position = u_mvp * localPos;

   v_texCoord = a_texCoord;
}
