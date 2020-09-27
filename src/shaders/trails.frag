#version 150

// referencing https://www.shadertoy.com/view/XlGXzw


// inputs
uniform sampler2DRect tex0; // src
uniform vec2 canvasDims;
uniform float alpha;
uniform float iFrame;
uniform float prototype_val1;
uniform float prototype_val2;
uniform float prototype_val3;
uniform float frame_factor;
uniform sampler2DRect video;

// from vert
in vec2 texCoordVarying;

// output
out vec4 outputColor;

void main (void)
{
    // get normalized position
    vec2 uv = texCoordVarying / canvasDims;
    vec2 texel = 1.0 / canvasDims;
    
    float step_y = texel.y;
    vec2 s = vec2(0.0, -step_y);
    vec2 n = vec2(0.0, step_y);
    
    vec4 im_n = texture(tex0, (uv + n)*canvasDims);
    vec4 im = texture(tex0, uv*canvasDims);
    vec4 im_s = texture(tex0, (uv + s)*canvasDims);
    
    // use luminance for sorting
//    float len_n = dot(im_n, vec4(0.299, 0.587, 0.114, 0.));
//    float len = dot(im, vec4(0.299, 0.587, 0.114, 0.));
//    float len_s = dot(im_s, vec4(0.299, 0.587, 0.114, 0.));
    
    float len_n = dot(im_n, vec4(prototype_val1, prototype_val2, prototype_val3, 0.));
    float len = dot(im, vec4(prototype_val1, prototype_val2, prototype_val3, 0.));
    float len_s = dot(im_s, vec4(prototype_val1, prototype_val2, prototype_val3, 0.));
    
//    if(int(mod(float(iFrame) + texCoordVarying.y, 2.0)) == 0) {
//        if ((len_s > len)) {
//            im = im_s;
//        }
//    } else {
//        if ((len_n < len)) {
//            im = im_n;
//        }
//    }
    
    im = im;
    // blend with image
    vec4 color;
//    if(iFrame<1) {
//        color = texture(video, texCoordVarying);
//    } else {
//        //color = (texture(video, texCoordVarying) + im * blend_factor ) / 100.;
//        color = (texture(video, vidCoord) + im * 99. ) / 100.;
//        //color = texture(video, vidCoord);
//    }
    
    //mod(iFrame, blend_factor) == 0
    vec4 base = vec4(0.0, 0.0, 0.0, 0.0);
    
    if(iFrame<1) {
        color = texture(video, texCoordVarying);
        color = vec4(color.r, color.g, color.b, 0.5);
    } else {
        vec4 base = vec4(color.r, color.g, color.b, mod(iFrame, frame_factor));
        color = (texture(video, texCoordVarying) + im * 99. ) / 100.;
        
        //color = (texture(video, vidCoord) + im * 99. ) / 100.;
        //color = texture(video, vidCoord);
    }
    
    outputColor = color + base;

}

