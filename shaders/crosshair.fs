#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform vec3 crosshairColor;
uniform float lineThickness;

void main()
{
    float center_x = 0.5;
    float center_y = 0.5;

    float width_x = 0.02;
    float width_y = 0.02;

    float horizontalLine = smoothstep(center_y - lineThickness, center_y + lineThickness, TexCoords.y);
    float verticalLine = smoothstep(center_x - lineThickness, center_x + lineThickness, TexCoords.x);
    
    float crosshair = 0.0;
    if (
        center_y - lineThickness < TexCoords.y && 
        TexCoords.y < center_y + lineThickness &&
        center_x - width_x < TexCoords.x && 
        TexCoords.x < center_x + width_x 

    ) {
        crosshair = 1.0;
    } 

    if (
        center_x - lineThickness < TexCoords.x && 
        TexCoords.x < center_x + lineThickness &&
        center_y - width_y < TexCoords.y && 
        TexCoords.y < center_y + width_y 
    ) {
        crosshair = 1.0;
    } 

    FragColor = mix(vec4(0.0, 0.0, 0.0, 0.0), vec4(crosshairColor, 1.0), crosshair * 0.8);
}
