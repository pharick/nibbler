#version 330

in vec3 vertexPosition;
out vec4 outputColor;

void main()
{
    vec3 normalizedPos = vertexPosition * 0.5 + 0.5;
    outputColor = vec4(normalizedPos, 1.0);
}