#version 150

uniform sampler2DRect video;
uniform vec2 offset;
uniform vec2 videoSize;
uniform vec2 canvasDims;

// from vert
in vec2 texCoordVarying;

// output
out vec4 outputColor;

// ======================================
// Helper Functions
// ======================================
// Utility functions

float map(float value, float min1, float max1, float min2, float max2) {
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void main (void)
{
    vec2 scale_factor;
    
    if(videoSize.y < canvasDims.y){
        scale_factor.y = videoSize.y / canvasDims.y;
    } else {
         scale_factor.y = canvasDims.y / videoSize.y;
    }
    
    if(videoSize.x < canvasDims.x){
        scale_factor.x = videoSize.x / canvasDims.x;
    } else {
        scale_factor.x = canvasDims.x / videoSize.x;
    }
    

    vec2 texCoordMapped = vec2(texCoordVarying.x * scale_factor.x,
                               texCoordVarying.y * scale_factor.y);


    vec2 vidCoord = texCoordMapped + mod(offset,videoSize);

    //    if(vidCoord.x < 0){
    //        vidCoord.x += videoSize.x;
    //    } else if(vidCoord.x > videoSize.x){
    //        vidCoord.x = 0;
    //    }
    vidCoord.x = texCoordMapped.x;


    if(vidCoord.y <= 0.0){
        vidCoord.y += videoSize.y;
    } else if(vidCoord.y >= videoSize.y){
        vidCoord.y -= videoSize.y;
    }

    // blend with image
    vec4 color = texture(video, vidCoord);
    
    // re map BACK to canvas
    outputColor = color;

}
