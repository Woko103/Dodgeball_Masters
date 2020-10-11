#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// Texture sampler 1
uniform sampler2D texture1;
// Texture sampler 2
uniform sampler2D texture2;

void main()
{
       FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
};