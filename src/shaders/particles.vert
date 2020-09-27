#version 150

// these are for the programmable pipeline system
uniform mat4 modelViewProjectionMatrix;

in vec4 position;

in vec4 instance; // per instance, xy = position, zw = size
in vec4 color;    // per instance

out vec4 colorVarying;

void main()
{
    colorVarying = color;
    colorVarying.a = 1.0 - position.x; // Adjust opacity from head to tail.

    vec4 p = position;
    p.xy = p.xy * instance.zw + instance.xy;

    gl_Position = modelViewProjectionMatrix * p;
}