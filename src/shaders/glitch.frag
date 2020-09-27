#version 150

// ======================================
// Inputs
// ======================================

uniform sampler2DRect img;

uniform vec2 canvasDims;
uniform int searchDist;
uniform float repThresh;
uniform float spatialNoiseScale;
uniform float time;
uniform float temporalNoiseScale;
uniform int bDist;
uniform int upper;
uniform float upper_edge_offset;
uniform float btm_edge_offset;

in vec2 texCoordVarying; // not normalized (in px)

out vec4 outputColor; // Range: [0, 1]


// ======================================
// Helper Functions
// ======================================

// Utility functions
int clamp(int val, int mi, int ma) {
	return max(min(val, ma), mi);
}
float clamp(float val, float mi, float ma) {
	return max(min(val, ma), mi);
}
float map(float value, float inMin, float inMax, float outMin, float outMax) {
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}
vec2 map(vec2 value, vec2 inMin, vec2 inMax, vec2 outMin, vec2 outMax) {
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}
vec3 map(vec3 value, vec3 inMin, vec3 inMax, vec3 outMin, vec3 outMax) {
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}
vec4 map(vec4 value, vec4 inMin, vec4 inMax, vec4 outMin, vec4 outMax) {
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

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
// Produces Fractal Brownian Motion ("clouds")
#define FBM_OCTAVES 5
float fbm(vec3 p, float frequency, float lacunarity, float addition)
{
    float t = 0.0;
    float amplitude = 1.0;
    float amplitudeSum = 0.0;
    for(int k = 0; k < FBM_OCTAVES; ++k)
    {
        t += min(snoise(p * frequency)+addition, 1.0) * amplitude;
        amplitudeSum += amplitude;
        amplitude *= 0.5;
        frequency *= lacunarity;
    }
    return t/amplitudeSum;
}

// ======================================
// Main Function
// ======================================

void main()
{

	// Find the normalized text coordinate
	float maxDim = max(canvasDims.x, canvasDims.y);
	vec2 normTexCoord = texCoordVarying / maxDim;

    // What is the search distance? (apply noise)
    float noise = snoise(vec3(texCoordVarying.y*spatialNoiseScale, time*temporalNoiseScale, 0));
    float scale = abs(noise);
    int direction = noise > 0 ? 1 : -1;
    int search = int(float(searchDist)*scale);
    // int search = searchDist;

    outputColor = texture(img, texCoordVarying);
    float prevK = 0;
	// Loop over all pixels to the left, within some distance
    for (int i = 1; i < search; i++) {
        // Where will we sample from?
        //uncomment for x direction
        //vec2 sampleLocation = texCoordVarying + vec2(i*direction, 0);
        //if (sampleLocation.x < 0 || sampleLocation.x >= canvasDims.x) break;
        
        //uncomment for y direction
        vec2 sampleLocation = texCoordVarying + vec2(0, i*direction);
        //if (sampleLocation.y < 0 || sampleLocation.y >= canvasDims.y) break;
        
        if (sampleLocation.y < 0) {
            sampleLocation.y += upper_edge_offset;
        }
        
        if(sampleLocation.y >= canvasDims.y){
            sampleLocation.y -= btm_edge_offset;
            //break;
        }
        

        
        //uncomment for both directions
        //vec2 sampleLocation = texCoordVarying + vec2(i*direction, i*direction);
        //if (sampleLocation.y < 0 || sampleLocation.y >= canvasDims.y) break;
        //if (sampleLocation.x < 0 || sampleLocation.x >= canvasDims.x) break;
        
        // What is the color at the sample?
        vec4 sampleColor = texture(img, sampleLocation);
        // What is the brightness of the sample? (TODO: improve formula)

        float brightness = (sampleColor.r+sampleColor.g+sampleColor.b)/3.0;

        // How far away is the sample, normalized to the search distance?
        float dist = float(i);
        float normalizedDist = dist / float(search);

        // Calculate a value to determine whether our pixel will be 
        // represented by this sample
        float k = brightness / (bDist == 1 ? dist : 1.0);

        // If this value is greater than a threshold and greater than previous
        // k's, set the output color to it
        
        
        if (k > repThresh) {// && k > prevK) {
            prevK = k;
            outputColor = sampleColor;
        }
    }


}



