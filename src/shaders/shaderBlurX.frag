#version 150

uniform sampler2DRect tex0;
uniform float blurAmnt;
uniform vec2 canvasDims;

in vec2 texCoordVarying;
out vec4 outputColor;

// Gaussian weights from http://dev.theomader.com/gaussian-kernel-calculator/

void main()
{
    
    float maxDim = max(canvasDims.x, canvasDims.y);
    vec2 normTexCoord = texCoordVarying / maxDim;
    
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	
	color += 0.000229 * texture(tex0, texCoordVarying + vec2(blurAmnt * -4.0, 0.0));
	color += 0.005977 * texture(tex0, texCoordVarying + vec2(blurAmnt * -3.0, 0.0));
	color += 0.060598 * texture(tex0, texCoordVarying + vec2(blurAmnt * -2.0, 0.0));
	color += 0.241732 * texture(tex0, texCoordVarying + vec2(blurAmnt * -1.0, 0.0));
    
	color += 0.382928 * texture(tex0, texCoordVarying + vec2(0.0, 0));
	
	color += 0.241732 * texture(tex0, texCoordVarying + vec2(blurAmnt * 1.0, 0.0));
	color += 0.060598 * texture(tex0, texCoordVarying + vec2(blurAmnt * 2.0, 0.0));
	color += 0.005977 * texture(tex0, texCoordVarying + vec2(blurAmnt * 3.0, 0.0));
	color += 0.000229 * texture(tex0, texCoordVarying + vec2(blurAmnt * 4.0, 0.0));
    
    outputColor = color;
}
