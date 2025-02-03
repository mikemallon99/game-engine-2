#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 6) out;

const float MAGNITUDE = 1.5;

out vec3 fColor;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void GenerateGizmo()
{
    fColor = vec3(1.0, 0.0, 0.0);
    gl_Position = projection * view * model * gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = projection * view * model * (gl_in[0].gl_Position + vec4(MAGNITUDE, 0.0, 0.0, 0.0));
    EmitVertex();
    EndPrimitive();
    
    fColor = vec3(0.0, 1.0, 0.0);
    gl_Position = projection * view * model * gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = projection * view * model * (gl_in[0].gl_Position + vec4(0.0, MAGNITUDE, 0.0, 0.0));
    EmitVertex();
    EndPrimitive();

    fColor = vec3(0.0, 0.0, 1.0);
    gl_Position = projection * view * model * gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = projection * view * model * (gl_in[0].gl_Position + vec4(0.0, 0.0, MAGNITUDE, 0.0));
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateGizmo();
}  
