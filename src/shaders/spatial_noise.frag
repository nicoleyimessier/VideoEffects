#version 150

uniform sampler2DRect tex0;
uniform vec2 offset;
uniform vec2 videoSize;
uniform vec2 canvasDims;

// noise
uniform float spatialNoiseScale;
uniform float time;
uniform float noiseScale;

// from vert
in vec2 texCoordVarying;

// output
out vec4 outputColor;

// ======================================
// Helper Functions
// ======================================
// Utility functions

// Return a random vec3 based on some vec3 as input
vec3 random3(vec3 c) {
    float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
    vec3 r;
    r.z = fract(512.0*j);
    j *= .125;
    r.x = fract(512.0*j);
    j *= .125;
    r.y = fract(512.0*j);
    return r-0.5;
}

// Produces noise based on a vec3 (returns value from [-1,1])
const float F3 =  0.3333333;
const float G3 =  0.1666667;
float snoise(vec3 p) {
    
    vec3 s = floor(p + dot(p, vec3(F3)));
    vec3 x = p - s + dot(s, vec3(G3));
    
    vec3 e = step(vec3(0.0), x - x.yzx);
    vec3 i1 = e*(1.0 - e.zxy);
    vec3 i2 = 1.0 - e.zxy*(1.0 - e);
    
    vec3 x1 = x - i1 + G3;
    vec3 x2 = x - i2 + 2.0*G3;
    vec3 x3 = x - 1.0 + 3.0*G3;
    
    vec4 w, d;
    
    w.x = dot(x, x);
    w.y = dot(x1, x1);
    w.z = dot(x2, x2);
    w.w = dot(x3, x3);
    
    w = max(0.6 - w, 0.0);
    
    d.x = dot(random3(s), x);
    d.y = dot(random3(s + i1), x1);
    d.z = dot(random3(s + i2), x2);
    d.w = dot(random3(s + 1.0), x3);
    
    w *= w;
    w *= w;
    d *= w;
    
    return dot(d, vec4(52.0));
}

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main (void)
{
    
    // return value between -1, 1
    //float noise = snoise(vec3(texCoordVarying.y*spatialNoiseScale,time*temporalNoiseScale,0));
    vec2 texNoise = texCoordVarying;
    float noise = snoise(vec3(0.0,texCoordVarying.x*spatialNoiseScale,0.0));
    float scale = abs(noise)*noiseScale;
    
    texNoise.y += scale;
    

    if(texNoise.y <= 0.0){
        texNoise.y += (texCoordVarying.y + 10);
    } else if(texNoise.y >= canvasDims.y){
        texNoise.y -= (texCoordVarying.y+10);
        //texNoise.y -= texCoordVarying.y;
    }

    // blend with image
    vec4 color = texture(tex0, texNoise);
    
    // re map BACK to canvas
    outputColor = color;
    
}

