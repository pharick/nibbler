#version 330

flat in int fragSegmentNumber;
out vec4 outputColor;

vec3 colors[6] = vec3[6](
    vec3(1.0, 0.0, 0.0), // Red
    vec3(0.0, 1.0, 0.0), // Green
    vec3(0.0, 0.0, 1.0), // Blue
    vec3(1.0, 1.0, 0.0), // Yellow
    vec3(1.0, 0.0, 1.0), // Magenta
    vec3(0.0, 1.0, 1.0)  // Cyan
);

void main()
{
    int colorIndex = fragSegmentNumber % 6;
    outputColor = vec4(colors[colorIndex], 1.0);
}