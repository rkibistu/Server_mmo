#version 330 core


out vec4 FragColor;

varying vec2 v_texCoord;

uniform sampler2D u_Texture;

void main()
{
	vec4 texColor = texture2D(u_Texture, v_texCoord);

	if(texColor.a < 0.2)
		discard;
	FragColor = texColor;
	//FragColor = vec4(1.0f, 0.5f,0.2f, 1.0f);
}