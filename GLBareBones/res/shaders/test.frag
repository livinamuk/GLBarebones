#version 420 core

layout (binding = 0) uniform sampler2D ALB_Texture0;

in vec2 TexCoord;
out vec4 FragColor;

void main()
{
	vec3 color = texture(ALB_Texture0, vec2(TexCoord.x, TexCoord.y)).rgb;
	FragColor = vec4(color, 1);//vec4(1.0f, 0.5f, 0.2f, 1.0f);
}  