#version 330 core
out vec4 FragColor;

uniform sampler2D text;

in vec2 TexCoord;
//in vec3 colour;
void main()
{
    
    FragColor = texture(text, TexCoord);
}