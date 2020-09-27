#include "VideoPlayerHap.h"

#include "Markers.h"

VideoPlayerHap::VideoPlayerHap()
{
    mPlayer = std::make_shared<ofxHapPlayer>();
    mQueued = std::make_shared<ofxHapPlayer>();
}

VideoPlayerHap::~VideoPlayerHap()
{
    mQueued->close();
    mPlayer->close();
}

void VideoPlayerHap::load( const std::filesystem::path &path )
{
    load( mPlayer, path );
    
}

void VideoPlayerHap::load( const std::shared_ptr<ofxHapPlayer> &player, const std::filesystem::path &path ) const
{
    std::string file = path.string();
    if( !std::filesystem::exists( path ) )
        file = ofToDataPath( file );

    if( std::filesystem::exists( file ) ) {
        player->load( file );
        //mPath = path; 
    }
}

void VideoPlayerHap::queue( const std::filesystem::path &path )
{
    mFiles.push_back( path );
}

void VideoPlayerHap::queue( const std::vector<std::filesystem::path> &paths )
{
    mFiles.insert( mFiles.end(), paths.begin(), paths.end() );
}

void VideoPlayerHap::close()
{
    mPlayer->close();
}

void VideoPlayerHap::clear()
{
    mFiles.clear();
    mIndex = 0;
}

void VideoPlayerHap::play()
{
    if( !mPlayer->isLoaded() ) {
        ofSleepMillis( 10 );
        return play();
    }
    mQueued->setLoopState( mLoop );
    mPlayer->setLoopState( mLoop );
    mPlayer->play();

    // Make sure we have a matching fbo.
    if( !mFbo.isAllocated() || mFbo.getWidth() != mPlayer->getWidth() || mFbo.getHeight() != mPlayer->getHeight() ) {
        ofFboSettings s;
        s.width = mPlayer->getWidth();
        s.height = mPlayer->getHeight();
        if( s.width == 0. || s.height == 0 ) {
            ofLogError() << " video height or width is zero!";
            return; 
        }

        s.useDepth = false;
        s.useStencil = false;
        s.wrapModeHorizontal = GL_CLAMP_TO_BORDER;
        s.wrapModeVertical = GL_CLAMP_TO_BORDER;
        mFbo.allocate( s );
    }
}

void VideoPlayerHap::playNext()
{
    if( !mFiles.empty() ) {
        if( !mPlayer->isPlaying() ) {
            load( mPlayer, mFiles[mIndex % mFiles.size()] );
            play();

            mQueued->stop();
            load( mQueued, mFiles[++mIndex % mFiles.size()] );
        }
        else if( mQueued->isLoaded() ) {
            std::swap( mPlayer, mQueued );
            play();

            mQueued->stop();
            load( mQueued, mFiles[++mIndex % mFiles.size()] );
        }
        else {
            load( mPlayer, mFiles[++mIndex % mFiles.size()] );
            play();

            mQueued->stop();
            load( mQueued, mFiles[++mIndex % mFiles.size()] );
        }
    }
}

void VideoPlayerHap::playPrevious()
{
    if( !mFiles.empty() ) {
        if( !mPlayer->isPlaying() ) {
            load( mPlayer, mFiles[mIndex % mFiles.size()] );
            play();

            mQueued->stop();
            if( mIndex == 0 )
                mIndex = mFiles.size();
            load( mQueued, mFiles[--mIndex % mFiles.size()] );
        }
        else if( mQueued->isLoaded() ) {
            std::swap( mPlayer, mQueued );
            play();

            mQueued->stop();
            if( mIndex == 0 )
                mIndex = mFiles.size();
            load( mQueued, mFiles[--mIndex % mFiles.size()] );
        }
        else {
            if( mIndex == 0 )
                mIndex = mFiles.size();
            load( mPlayer, mFiles[--mIndex % mFiles.size()] );
            play();

            mQueued->stop();
            if( mIndex == 0 )
                mIndex = mFiles.size();
            load( mQueued, mFiles[--mIndex % mFiles.size()] );
        }
    }
}

void VideoPlayerHap::pause()
{
    mPlayer->setPaused( true );
}

void VideoPlayerHap::stop()
{
    mPlayer->stop();
}

void VideoPlayerHap::update()
{
    ofEventArgs dummy;
    update( dummy );
}

void VideoPlayerHap::update( ofEventArgs &args )
{
    tools::ScopedMarker m( "VideoPlayerHap:Update" );

    mPlayer->update( args );
    const auto w = mPlayer->getWidth();
    const auto h = mPlayer->getHeight();

    mFbo.begin();
    ofPushStyle();
    ofClear( 0, 0, 0, 0 );
    ofSetColor( 255 );
    ofDisableAlphaBlending();
    if( mIsFlipped )
        mPlayer->draw( 0, h, w, -h );
    else
        mPlayer->draw( 0, 0, w, h );
    ofPopStyle();
    mFbo.end();

    if( isDone() && mAutomaticQueue ) {
        playNext();
    }
}

void VideoPlayerHap::draw()
{
    tools::ScopedMarker m( "VideoPlayerHap:Draw" );

    const auto width = ofGetViewportWidth();
    const auto height = ofGetViewportHeight();

    if( mFbo.isAllocated() ) {
        const auto texture = mFbo.getTexture();

        const auto aspect = mPlayer->getWidth() / mPlayer->getHeight();
        if( ( mIsLetterBoxed && aspect > 1.0f ) || ( !mIsLetterBoxed && aspect < 1.0f ) ) {
            const float w = height * aspect;
            const float x = 0.5f * ( width - w );
            texture.draw( x, 0, w, height );
        }
        else {
            const float h = width / aspect;
            const float y = 0.5f * ( height - h );
            texture.draw( 0, y, width, h );
        }
    }
}

void VideoPlayerHap::draw( float x, float y, float w, float h )
{
    tools::ScopedMarker m( "VideoPlayerHap:Draw" );

    if( mFbo.isAllocated() ) {
        const auto texture = mFbo.getTexture();
        texture.draw( x, y, w, h );
    }
}

const ofTexture &VideoPlayerHap::getTexture() const
{
    return mFbo.getTexture();
}

ofTexture &VideoPlayerHap::getTexture()
{
    return mFbo.getTexture();
}

float VideoPlayerHap::getWidth() const
{
    return mPlayer->getWidth();
}

float VideoPlayerHap::getHeight() const
{
    return mPlayer->getHeight();
}

const std::filesystem::path &VideoPlayerHap::getFile() const
{
    if( !mFiles.empty() )
        return mFiles[mIndex % mFiles.size()];

    return std::filesystem::path();
}

bool VideoPlayerHap::isLoaded() const
{
    return mPlayer->isLoaded();
}

bool VideoPlayerHap::isPaused() const
{
    return mPlayer->isPaused();
}

bool VideoPlayerHap::isPlaying() const
{
    return mPlayer->isPlaying();
}

bool VideoPlayerHap::isDone() const
{
    return mPlayer->getIsMovieDone();
}

float VideoPlayerHap::getPosition() const
{
    return mPlayer->getPosition();
}

float VideoPlayerHap::getSpeed() const
{
    return mPlayer->getSpeed();
}

float VideoPlayerHap::getDuration() const
{
    return mPlayer->getDuration();
}

ofLoopType VideoPlayerHap::getLoop() const
{
    return mPlayer->getLoopState();
}

void VideoPlayerHap::setLoop( ofLoopType loop )
{
    mLoop = loop;
    mPlayer->setLoopState( mLoop );
    mQueued->setLoopState( mLoop );
}
