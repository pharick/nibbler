#version 330

flat in int fragSegmentNumber;
out vec4 outputColor;

void main()
{
    // Normalize fragSegmentNumber to a value between 0 and 1
    float t = mod(float(fragSegmentNumber), 100.0) / 100.0;

    // Generate a smooth gradient using t
    vec3 color = vec3(
        sin(t * 3.14159 * 2.0) * 0.5 + 0.5, // Red channel
        sin((t + 0.333) * 3.14159 * 2.0) * 0.5 + 0.5, // Green channel
        sin((t + 0.666) * 3.14159 * 2.0) * 0.5 + 0.5  // Blue channel
    );

    outputColor = vec4(color, 1.0);
}