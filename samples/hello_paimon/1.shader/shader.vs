#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

void main()
{
    // original
    // gl_Position = vec4(aPos, 1.0);
    
    // exercise1
    // gl_Position = vec4(aPos.x, -aPos.y, aPos.z, 1.0f);
    
    // exercise2
    // gl_Position = vec4(0.5f + aPos.x, aPos.y, aPos.z, 1.0f);
    
    // exercixe3
    gl_Position = vec4(aPos, 1.0f);
    ourColor = aPos;
}
