#version 460 core

// Redeclare built-in block required by ARB_separate_shader_objects
out gl_PerVertex {
  vec4 gl_Position;
};

out vec2 v_texcoord;

void main()
{
    // Use a single triangle that covers the entire screen
    // No vertex buffer needed - vertices are hardcoded
    // Triangle vertices in NDC:
    // v0: (-1, -1) -> texcoord (0, 0)
    // v1: ( 3, -1) -> texcoord (2, 0)
    // v2: (-1,  3) -> texcoord (0, 2)
    vec2 vertices[3] = vec2[3](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
    );
    
    vec2 texcoords[3] = vec2[3](
        vec2(0.0, 0.0),
        vec2(2.0, 0.0),
        vec2(0.0, 2.0)
    );
    
    gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
    v_texcoord = texcoords[gl_VertexID];
}
