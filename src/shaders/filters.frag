#version 150

// Fragment shader for modifying image contrast by
// interpolation and extrapolation
//
// Author: Randi Rost
//
// Copyright (c) 2002: 3Dlabs, Inc.
//
// See 3Dlabs-License.txt for license information
//

const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);

uniform sampler2DRect tex0;
uniform vec3 avgluma;
uniform float saturation;
uniform float contrast;
uniform float brightness;
uniform float alpha;

in vec2 texCoordVarying;
out vec4 outputColor;

void main (void)
{
    vec3 texColor   = texture(tex0, texCoordVarying).rgb;
    vec3 intensity  = vec3(dot(texColor, LumCoeff));
    vec3 color      = mix(intensity, texColor, saturation);
    color           = mix(avgluma, color, contrast);
    color          *= brightness;
    outputColor    = vec4(color, alpha);
}

