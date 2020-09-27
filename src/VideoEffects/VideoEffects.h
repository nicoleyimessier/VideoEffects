//
//  VideoEffects.hpp
//  video_effects
//
//  Created by Nicole Yi Messier on 4/20/20.
//

#include "ofMain.h"
#include "pingPongBuffer.h"
#include "UberStructs.h"
#include "AppSettings.h"


class VideoEffects {
public:
    VideoEffects();
    ~VideoEffects();
    
    void setup();
    void update(const ofTexture &vid);
    void draw();
    
    // --- BLURR --- //
    void blurVideo(ofFbo &fbo_pass);
    void drawBlur(); 
    
    ofShader shaderBlurX;
    ofShader shaderBlurY;
    
    ofFbo fboBlurOnePass;
    
    float blurX = 0.0f;
    float blurY = 0.0f;
    
    // --- FILTERS --- //
    void filterVideo(ofFbo &fbo_pass);
    void drawFilters();
    
    float saturation = 1.0;
    float contrast = 1.0;
    float brightness = 1.0;
    float alpha = 1.0;
    
    ofShader shaderFilter;
    
    // --- GLITCH --- //
    void glitchVideo(ofFbo &fbo_pass);
    void drawGlitch();

    const ofFbo &getGlitchPass() const { return glitchPass; }
    
    int searchDist = 100; // px
    float repThresh = 1.0; // representation threshold
    float spatialNoiseScale = 1.0;
    float temporalNoiseScale = 1.0;
    bool bDist = true;
    float upper_edge_offset = 0.0f;
    float btm_edge_offset = 0.0f;

    ofShader shaderGlitch;
    ofFbo glitchPass;
    
    // --- TRAIL --- //
    void trailsVideo(const ofFbo &fbo_pass, const ofTexture &vid);
    void drawTrails();
    
    ofShader shaderTrail;
    pingPongBuffer trailPass;
    float frameCounter = 0;
    
    // lum vector
    float prototype_val1 = 1.;
    float prototype_val2 = 1.;
    float prototype_val3 = 1.;
    float frame_factor = 99.0f;
    float trails_percent = 1.0f;


    // --- PAN --- //
    void panVideo( const ofTexture &vid );
    void drawPan();
    
    bool move_down = false;
    
    //! speed percentages
    float sin_percent = 1.0f;
    float noise_percent = 1.0f;
    float linear_percent = 1.0f;
    
    //! offset
    float max_sin = 5.0f;
    float angle = 1.0f;
    float speed = 0.001f;
    glm::vec2 videoOffset = glm::vec2(0.0f, 0.0f);
    
    //! noise function
    float time_gain = 1.0f;
    float noise_xpos = 1.0f;
    float noise_ypos = 1.0f;
    float max_noise = 5.0f;
    
    ofShader shaderPan;
    
    // --- SPATIAL NOISE --- //
    float spatialNoiseScale_pan = 1.0f;
    float noiseScale = 1.0f;
    
    ofShader shaderSpatial;
    
    void spatialVideo(ofFbo &fbo);
    
    // --- DRAW STATES ---- //
    enum DrawStates {
        PAN,
        TRAILS,
        BLUR,
        FILTERS,
        SPATIAL_NOISE,
        GLITCH,
        NUM_DRAW_STATES
    };
    
    DrawStates drawState;
    void setDrawState(DrawStates drawState);
    void nextDrawState();
    string getStateName();
    
    float effect_height = 300.0f; 
    bool drawEffect = false;
    bool drawVideo = true;
    
    //! utils
    void drawEachEffect();
    
    
    // --- ORDER --- //
    map<int, DrawStates> filter_order;
    map<DrawStates, ofFbo> filter_fbo;
    
    // --- VIDEO ---- //filter_fbo.at(DrawStates::BLUR)
    void clearFbos();
   
    // --- TEXTURE MAPS --- // 
    glm::vec2 canvas_dims; 

    float rotate = 0.0f; 
    float translate_x = 0.0f; 
    float translate_y = 0.0f;

    int num_oh_repeat = 4; 

    // --- DRAW --- // 
    void drawStream(Uber::ScreenType screen_type);
};

/*
switch(drawState){
    case PAN: { break;}
    case TRAILS: { break;}
    case BLUR: { break;}
    case FILTERS: { break;}
    case SPATIAL_NOISE: { break;}
    case GLITCH: { break;}
    default: break;
}
*/

