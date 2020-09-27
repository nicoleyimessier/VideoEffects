#version 150

in vec4 colorVarying;

out vec4 fragColor;

void main()
{
    // Use pre-multiplied alpha.
    fragColor.rgb = colorVarying.rgb * colorVarying.a;
    fragColor.a = colorVarying.a;
}