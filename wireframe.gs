#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.4;
  
uniform mat4 projection;

void GenerateTriangle()
{
    gl_Position = projection * gl_in[2].gl_Position;
    EmitVertex();
    gl_Position = projection * gl_in[0].gl_Position;
    EmitVertex();
    EndPrimitive();

    gl_Position = projection * gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = projection * gl_in[1].gl_Position;
    EmitVertex();
    EndPrimitive();

    gl_Position = projection * gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = projection * gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();
}

void GenerateLine(int index)
{
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = projection * (gl_in[index].gl_Position + 
                                vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateTriangle();
}  
