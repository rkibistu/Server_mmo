#version 330

// Input
layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

// Output value to fragment shader
out vec3 world_position;
out vec3 world_normal;
out vec2 v_texCoord;

void main()
{
    // Compute world space vertex position and normal
    world_position = vec3(Model * vec4(a_pos,1.0));
    world_normal = mat3(Model) * (a_normal);

    // Texture
    v_texCoord = a_texCoord;
   
    gl_Position = Projection * View * Model * vec4(a_pos, 1.0);
}
