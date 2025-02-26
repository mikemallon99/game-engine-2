#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    float alpha = texture(text, TexCoords).r;  // Assuming grayscale texture (only the red channel matters)
    vec4 sampled = vec4(1.0, 1.0, 1.0, alpha); // White color with the alpha value from texture
    color = vec4(1.0, 1.0, 1.0, 1.0) * sampled; // Apply textColor to the sampled texture
}