//
//  VideoEffects.cpp
//  video_effects
//
//  Created by Nicole Yi Messier on 4/20/20.
//

#include "VideoEffects.h"
#include "Markers.h"
#include "Labels.h"

using namespace Uber;

VideoEffects::VideoEffects(){
    
}

 VideoEffects::~VideoEffects(){
    
}

void VideoEffects::setup(){

    
    //! setup blurr
    shaderBlurX.load("shaders/shaderBlurX");
    shaderBlurY.load("shaders/shaderBlurY");

    canvas_dims = AppSettings::one().getHeroCanvasSize(); 
    
    fboBlurOnePass.allocate(canvas_dims.x, canvas_dims.y);
    fboBlurOnePass.begin(); ofClear(0, 0, 0, 0); fboBlurOnePass.end();
    tools::setObjectLabel( fboBlurOnePass, "VideoEffects: Blur One Pass Fbo" );
    
    //! setup filters
    shaderFilter.load("shaders/filters");
    
    //! setup glitch
    shaderGlitch.load("shaders/glitch");
    glitchPass.allocate(canvas_dims.x, canvas_dims.y); 
    glitchPass.begin(); ofClear(0, 0, 0, 0); glitchPass.end();
    tools::setObjectLabel( glitchPass, "VideoEffects: Glitch Pass Fbo" );
    
    //! setup trail
    shaderTrail.load("shaders/trails");
    trailPass.allocate(canvas_dims.x, canvas_dims.y, GL_RGBA);
    trailPass.src->begin(); ofClear(0, 0, 0, 0); trailPass.src->end();
    
    //! setup pan
    shaderPan.load("shaders/pan");
    
    //! setup spatial noise
    shaderSpatial.load("shaders/spatial_noise");
    
    //! add filter order
    filter_order.insert(pair<int, DrawStates>(0, PAN));
    filter_order.insert(pair<int, DrawStates>(1, BLUR));
    filter_order.insert(pair<int, DrawStates>(2, TRAILS));
    filter_order.insert(pair<int, DrawStates>(3, FILTERS));
    filter_order.insert(pair<int, DrawStates>(4, SPATIAL_NOISE));
    filter_order.insert(pair<int, DrawStates>(5, GLITCH));
    
    //! create fbos
    int max_states = static_cast<int>(NUM_DRAW_STATES);
    for(int i=0; i < max_states; i++){
        ofFbo fbo;
        fbo.allocate(canvas_dims.x, canvas_dims.y);
        fbo.begin(); ofClear(0, 0, 0, 0); fbo.end();
        filter_fbo.insert(pair<DrawStates, ofFbo>(static_cast<DrawStates>(i), fbo));
        tools::setObjectLabel( fbo, "VideoEffects: Filter Fbo" );
    }


    //! set state
    //setDrawState(DrawStates::TRAILS);
    setDrawState(filter_order.at(static_cast<int>(NUM_DRAW_STATES) - 1));
}

void VideoEffects::update( const ofTexture &vid )
{
   	tools::ScopedMarker m("VideoEffects: Update");
    
    // Shader effects
    int max_states = static_cast<int>(NUM_DRAW_STATES);
    for(int i=0; i < max_states; i++){
        switch(filter_order.at(i))
        {
            case PAN: { panVideo(vid); break;}
            case TRAILS: { trailsVideo(filter_fbo.at(filter_order.at(i-1)), vid); break;}
            case BLUR: {
                blurVideo(filter_fbo.at(filter_order.at(i-1)));
                break;
            }
            case FILTERS: { filterVideo(filter_fbo.at(filter_order.at(i-1))); break;}
            case SPATIAL_NOISE: { spatialVideo(filter_fbo.at(filter_order.at(i-1))); break;}
            case GLITCH: { glitchVideo(filter_fbo.at(filter_order.at(i-1))); break;}
            default: break;
        }
    }

    
}

void  VideoEffects::draw(){
   	tools::ScopedMarker m("VideoEffects: Draw");
    
    if(drawVideo){
        
        switch(drawState){
            case PAN: { drawPan(); break;}
            case TRAILS: { drawTrails(); break;}
            case BLUR: {
                drawBlur();
                break;
            }
            case FILTERS: { drawFilters(); break;}
            case GLITCH: { drawGlitch(); break;}
            default: break;
        }
        
    }
    
    if(drawEffect){
        //!Draw debug effect
        drawEachEffect();
    }
    
} 

#pragma mark TRAILS
void         VideoEffects::trailsVideo( const ofFbo &fbo_pass, const ofTexture &vid )
{
   	tools::ScopedMarker m("VideoEffects: Trails Video");
    
    trailPass.dst->begin();
    shaderTrail.begin();
    shaderTrail.setUniformTexture( "tex0" , trailPass.src->getTexture(), 0);
    shaderTrail.setUniformTexture( "video" , fbo_pass.getTexture(), 1);
    shaderTrail.setUniform2f("canvasDims", canvas_dims);
    shaderTrail.setUniform1f("alpha", alpha);
    shaderTrail.setUniform1f("iFrame", frameCounter);
    shaderTrail.setUniform1f("prototype_val1", prototype_val1);
    shaderTrail.setUniform1f("prototype_val2", prototype_val2);
    shaderTrail.setUniform1f("prototype_val3", prototype_val3);
    shaderTrail.setUniform1f("frame_factor", frame_factor);
    
    shaderTrail.setUniform2f("offset", videoOffset);
    shaderTrail.setUniform2f("videoSize", glm::vec2(vid.getWidth(), vid.getHeight()));
    
    
    
    //vid.draw(0, 0, ofGetWidth(), ofGetHeight());
    trailPass.src->getTexture().draw(0, 0);
    
    shaderTrail.end();
    trailPass.dst->end();
    frameCounter++;
    trailPass.swap();
    
    filter_fbo.at(TRAILS).begin();
    {
        ofSetColor(0);
        ofDrawRectangle(0, 0, canvas_dims.x, canvas_dims.y);
        
        ofSetColor(255);
        fbo_pass.draw(0, 0, canvas_dims.x, canvas_dims.y);
        
        ofSetColor(255, 255*trails_percent);
        //ofSetColor(255);
        //panPass.draw(0, 0, ofGetWidth(), ofGetHeight());
        trailPass.dst->draw(0, 0, canvas_dims.x, canvas_dims.y);
    }
    filter_fbo.at(TRAILS).end();
}

void VideoEffects::drawTrails(){
    
    filter_fbo.at(TRAILS).draw(0, 0, canvas_dims.x, canvas_dims.y);
    
}

#pragma mark GLITCH

void VideoEffects::glitchVideo(ofFbo &fbo_pass){
   	tools::ScopedMarker m("VideoEffects: Glitch Video");

    // Draw the shader to the render fbo
    glitchPass.begin();
    ofClear(0, 0, 0, 0);
    shaderGlitch.begin();
    
    // Provide textures to the shader
    shaderGlitch.setUniformTexture("img", fbo_pass.getTexture(), 0);
    
    // Provide constants to the shader
    shaderGlitch.setUniform2f("canvasDims", canvas_dims);
    shaderGlitch.setUniform1i("searchDist", searchDist);
    shaderGlitch.setUniform1f("repThresh", repThresh);
    shaderGlitch.setUniform1f("spatialNoiseScale", spatialNoiseScale);
    shaderGlitch.setUniform1f("time", ofGetElapsedTimef());
    shaderGlitch.setUniform1f("temporalNoiseScale",temporalNoiseScale);
    shaderGlitch.setUniform1i("bDist", int(bDist));
    shaderGlitch.setUniform1f("upper_edge_offset", upper_edge_offset);
    shaderGlitch.setUniform1f("btm_edge_offset", upper_edge_offset);

    // Draw the texture at target 0 to make the shader to run
    //filterPass.draw(0, 0, ofGetWidth(), ofGetHeight());
    fbo_pass.draw(0, 0, canvas_dims.x, canvas_dims.y);
    
    shaderGlitch.end();
    glitchPass.end();
}

void VideoEffects::drawGlitch(){
    
   glitchPass.draw(0, 0, canvas_dims.x, canvas_dims.y);
}


#pragma mark FILTER

void  VideoEffects::drawFilters(){
    filter_fbo.at(FILTERS).draw(0, 0, canvas_dims.x, canvas_dims.y);
}

void VideoEffects::filterVideo(ofFbo &fbo_pass){
   	tools::ScopedMarker m("VideoEffects: Filter Video");
    
    filter_fbo.at(FILTERS).begin();
    
    shaderFilter.begin();
    
    shaderFilter.setUniformTexture( "tex0" , fbo_pass.getTexture(), 0);
    shaderFilter.setUniform3f("avgluma", 0.62,0.62,0.62);
    shaderFilter.setUniform1f("saturation", saturation);
    shaderFilter.setUniform1f("contrast", contrast);
    shaderFilter.setUniform1f("brightness", brightness);
    shaderFilter.setUniform1f("alpha", alpha);
    
    fbo_pass.draw(0, 0, canvas_dims.x, canvas_dims.y);
    
    shaderFilter.end();
    filter_fbo.at(FILTERS).end();
}

#pragma mark BLURR
void  VideoEffects::drawBlur(){
    filter_fbo.at(BLUR).draw(0, 0, canvas_dims.x, canvas_dims.y);
}

void VideoEffects::blurVideo(ofFbo &fbo_pass){
   	tools::ScopedMarker m("VideoEffects: Blur Video");

    //----------------------------------------------------------
    fboBlurOnePass.begin();
    
    shaderBlurX.begin();
    shaderBlurX.setUniform1f("blurAmnt", blurX);
    shaderBlurX.setUniform2f("canvasDims", canvas_dims);
    
    fbo_pass.draw(0, 0, canvas_dims.x, canvas_dims.y);
    
    shaderBlurX.end();
    
    fboBlurOnePass.end();
    
    //----------------------------------------------------------
    filter_fbo.at(BLUR).begin();
    
    shaderBlurY.begin();
    shaderBlurY.setUniform1f("blurAmnt", blurY);
    shaderBlurY.setUniform2f("canvasDims", canvas_dims);
    
    fboBlurOnePass.draw(0, 0, canvas_dims.x, canvas_dims.y);
    
    shaderBlurY.end();
    
    filter_fbo.at(BLUR).end();
    
    //----------------------------------------------------------
}

#pragma mark SPATIAL

void VideoEffects::spatialVideo(ofFbo &fbo){
   	tools::ScopedMarker m("VideoEffects: Spatial Video");
    
    // SHADERRRR BOI
    filter_fbo.at(SPATIAL_NOISE).begin();
    shaderSpatial.begin();
    
    shaderSpatial.setUniformTexture( "tex0" , fbo.getTexture(), 0);
    shaderSpatial.setUniform2f("canvasDims", canvas_dims);
    
    // per pixel noise
    shaderSpatial.setUniform1f("spatialNoiseScale", spatialNoiseScale_pan);
    shaderSpatial.setUniform1f("time", ofGetElapsedTimef());
    shaderSpatial.setUniform1f("noiseScale",noiseScale);
    
    
    fbo.getTexture().draw(0, 0, canvas_dims.x, canvas_dims.y);
    
    shaderSpatial.end();
    filter_fbo.at(SPATIAL_NOISE).end();
}

#pragma mark PAN

void VideoEffects::panVideo( const ofTexture &vid )
{
   	tools::ScopedMarker m("VideoEffects: Pan Video");

    // TIME SCALE
    float time = ofGetElapsedTimef();
    float newTime = time*time_gain;
    
    // NOISE
    float noise = ofNoise(noise_xpos,
                          noise_ypos,
                          newTime);
    float noise_map = ofMap(noise, 0, 1, 0, max_noise);
    
    // SIN
    float sine = sin(time);
    float sin_map = ofMap(sine, -1, 1, 0, max_sin);
    
    // TOTAL SPEED
    float total_speed = speed*linear_percent + sin_map*sin_percent + noise_map*noise_percent;
    
    // SPEED DIRECTION
    if(move_down){
        total_speed = -total_speed;
    }
    
    // OFFSET
    videoOffset.x += total_speed;
    videoOffset.y += total_speed;
    
    
    // SHADERRRR BOI
    filter_fbo.at(PAN).begin();
    shaderPan.begin();

    shaderPan.setUniformTexture( "video" , vid, 0);
    shaderPan.setUniform2f("canvasDims", canvas_dims);
    shaderPan.setUniform2f("offset", videoOffset);
    shaderPan.setUniform2f("videoSize", glm::vec2(vid.getWidth(), vid.getHeight()));
    
    vid.draw(0, 0, canvas_dims.x, canvas_dims.y);
    
    shaderPan.end();
    filter_fbo.at(PAN).end();
}

void VideoEffects::drawPan(){
    filter_fbo.at(PAN).draw(0, 0, canvas_dims.x, canvas_dims.y);
}

#pragma mark DRAW STATES

void VideoEffects::setDrawState(DrawStates drawState){
    this->drawState = drawState;
    
    switch(drawState){
        case TRAILS: { break;}
        case BLUR: { break;}
        case FILTERS: { break;}
        case GLITCH: { break;}
        default: break;
    }
    
}

string VideoEffects::getStateName(){
    switch(drawState){
        case TRAILS: { return "TRAILS"; }
        case BLUR: { return "BLUR";}
        case FILTERS: { return "FILTERS";}
        case SPATIAL_NOISE: { return "SPATIAL_NOISE";}
        case GLITCH: { return "GLITCH";}
        default: break;
    }
}

void VideoEffects::nextDrawState(){
    
    int num_state = static_cast<int>(drawState);
    int max_states = static_cast<int>(NUM_DRAW_STATES);
    
    if(num_state < (max_states - 1)){
        num_state++;
    } else {
        num_state = 0;
    }
    
    setDrawState(static_cast<DrawStates>(num_state));
}

void VideoEffects::drawEachEffect(){
    
    float ratio = ofGetWidth() / ofGetHeight();
    float width = ratio * effect_height;
    
    //! draw black rectangle
    ofPushStyle();
    ofSetColor(0);
    float height = 25.0f;
    ofDrawRectangle(0.0f,  ofGetHeight() - effect_height - height, ofGetWidth(), height);
    ofPopStyle();
    
    for(int i=0; i < static_cast<int>(NUM_DRAW_STATES); i++){
        glm::vec2 pos;
        pos.x = width*i;
        pos.y = ofGetHeight() - effect_height;
        
        // draw border for each effect
        ofPushStyle();
        ofSetColor(255);
        ofNoFill();
        ofDrawRectangle(pos.x, pos.y, width, effect_height);
        ofPopStyle();
        
        switch(filter_order.at(i)){
            case PAN: {
                filter_fbo.at(PAN).draw(pos.x, pos.y, width, effect_height);
                ofDrawBitmapString("PAN", pos.x, pos.y);
                break;}
            case TRAILS: {
                ofSetColor(0);
                ofDrawRectangle(pos.x, pos.y, width, effect_height);
                
                ofSetColor(255);
                filter_fbo.at(TRAILS).draw(pos.x, pos.y, width, effect_height);
                ofDrawBitmapString("TRAILS", pos.x, pos.y);

                break;
            }
            case BLUR: {
                filter_fbo.at(BLUR).draw(pos.x, pos.y, width, effect_height);
                ofDrawBitmapString("BLUR", pos.x, pos.y);
                break;}
            case FILTERS: {
                filter_fbo.at(FILTERS).draw(pos.x, pos.y, width, effect_height);
                ofDrawBitmapString("FILTERS", pos.x, pos.y);
                break;
            }
            case SPATIAL_NOISE: {
                filter_fbo.at(SPATIAL_NOISE).draw(pos.x, pos.y, width, effect_height);
                ofDrawBitmapString("SPATIAL NOISE", pos.x, pos.y);
                break;
            }
            case GLITCH: {
                glitchPass.draw(pos.x, pos.y, width, effect_height);
                ofDrawBitmapString("GLITCH", pos.x, pos.y);
                break;
            }
            case NUM_DRAW_STATES: {
                break;
            }
            default: break;
        }
        
        
    }
    
}

#pragma mark VIDEO

void VideoEffects::clearFbos(){
    //! clear blurr
    fboBlurOnePass.begin(); ofClear(0, 0, 0, 0); fboBlurOnePass.end();
    filter_fbo.at(BLUR).begin(); ofClear(0, 0, 0, 0); filter_fbo.at(BLUR).end();
    
    //! clear filters
    filter_fbo.at(FILTERS).begin(); ofClear(0, 0, 0, 0); filter_fbo.at(FILTERS).end();
    
    //! clear glitch
    glitchPass.begin(); ofClear(0, 0, 0, 0); glitchPass.end();
    
    //! clear pan
    filter_fbo.at(PAN).begin(); ofClear(0, 0, 0, 0); filter_fbo.at(PAN).end();
    
    //! clear trail
    trailPass.clear();
    filter_fbo.at(TRAILS).begin(); ofClear(0, 0, 0, 0); filter_fbo.at(TRAILS).end();
    
    //! clear spatial noise filter
    filter_fbo.at(SPATIAL_NOISE).begin(); ofClear(0, 0, 0, 0); filter_fbo.at(SPATIAL_NOISE).end();
    
    frameCounter = 0;
    
}

#pragma mark TEXTURE MAPS

void VideoEffects::drawStream(ScreenType screen_type) {
    tools::ScopedMarker m( "VideoEffects: Draw Stream" );

    switch (screen_type) {
    case ScreenType::HERO_SCREEN: {
        glitchPass.getTexture().draw(0.0f, 0.0f); 
        break;
    }
    case ScreenType::OVERHEAD_SCREEN: {
        glm::vec2 oh_size = AppSettings::one().getInteractiveArea(); 

        ofPushMatrix();
        ofTranslate(oh_size.x, 0.0f);
        ofRotate(90.0f);

        glm::vec2 pos = glm::vec2(oh_size.y, 0.0f);
        glm::vec2 size = glm::vec2(-oh_size.y, oh_size.x);
        float new_width = size.y / (float)num_oh_repeat;

        for (int i = 0; i < num_oh_repeat; i++) {

            //uncomment for debg
            //(i % 2) ? ofSetColor(255., 0., 0., 100.) : ofSetColor(0., 255., 0., 100.);
            //ofDrawRectangle(pos.x, pos.y, size.x, new_width);

            glitchPass.getTexture().draw(pos.x, pos.y, size.x, new_width);

            pos.y += new_width;
        }

        ofPopMatrix();

        break;
    }
    case ScreenType::RECEPTION_SCREEN: {
        ofPushMatrix();

        ofTranslate(AppSettings::one().getReceptionCanvasSize().x, 0.0f);
        ofRotate(90.0f);

        glitchPass.getTexture().draw(0.0f, 0.0f, 
                                    AppSettings::one().getReceptionCanvasSize().y,
                                    AppSettings::one().getReceptionCanvasSize().x);

        ofPopMatrix();
        break;
    }

    default: break;
    }
}
