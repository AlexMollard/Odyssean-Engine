#version 330 core
out vec4 FragColor;

in vec3 colour;
void main()
{
    
    FragColor = vec4(colour.r, colour.g, colour.b, 1.0f);
}