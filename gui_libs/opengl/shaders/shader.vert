#version 330

layout (location = 0) in vec3 position;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;

out vec3 vertexPosition;

void main()
{
    vec4 pos = projectionMatrix * modelMatrix * vec4(position, 1.0);
    gl_Position = pos;
    vertexPosition = pos.xyz;
}
