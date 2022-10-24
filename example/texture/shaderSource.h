#pragma once
#pragma region vertex shader and fragment shader

static const char *vertexShaderText =
    "#version 110\n"

    "uniform mat4 u_mvp;\n"

    "attribute  vec3 a_pos;\n"
    "attribute  vec2 a_uv;\n"

    "varying vec2 v_uv;\n"

    "void main()\n"
    "{\n"
    "    gl_Position = u_mvp * vec4(a_pos, 1.0);\n"
    "    v_uv = a_uv;\n"
    "}\n";

static const char *fragmentShaderText =
    "#version 110\n"
    "uniform sampler2D u_diffuse_texture;"
    "varying vec2 v_uv;\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = texture2D(u_diffuse_texture,v_uv);\n"
    "}\n";

#pragma endregion
