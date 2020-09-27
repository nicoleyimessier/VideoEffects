#include "VideoPlayerWmf.h"

#include "Markers.h"
#include "ofUtils.h"
#include "ofxTimeMeasurements.h"

#include <GLFW/glfw3.h>

VideoPlayerWmf::VideoPlayerWmf()
{
    mPlayer = std::make_shared<ofxWMFVideoPlayer>();
    mQueued = std::make_shared<ofxWMFVideoPlayer>();
}

VideoPlayerWmf::~VideoPlayerWmf()
{
    mQueued->close();
    mPlayer->close();
}

void VideoPlayerWmf::load( const std::filesystem::path &path )
{
    auto file = path.generic_path();
    if( !exists( file ) )
        file = std::filesystem::path( ofToDataPath( file, true ) ).generic_path();

    if( exists( file ) ) {
        load( mPlayer, file );

        setIndex( file );
    }
}

void VideoPlayerWmf::load( const std::shared_ptr<ofxWMFVideoPlayer> &player, const std::filesystem::path &path ) const
{
    if( exists( path ) ) {
        TS_START( "VideoPlayerWmf::load" );
        if( mUseAsyncLoading )
            player->loadMovieAsync( path.string() );
        else
            player->loadMovie( path.string() );
        TS_STOP( "VideoPlayerWmf::load" );
    }
}

void VideoPlayerWmf::allocateFbo()
{
    if( !mPlayer ) {
        mFbo.clear();
    }
    else if( !mFbo.isAllocated() || mFbo.getWidth() != mPlayer->getWidth() || mFbo.getHeight() != mPlayer->getHeight() ) {
        ofFboSettings s;
        s.width = int( mPlayer->getWidth() );
        s.height = int( mPlayer->getHeight() );
        if( s.width > 0 && s.height > 0 ) {
            s.useDepth = false;
            s.useStencil = false;
            s.wrapModeHorizontal = GL_CLAMP_TO_BORDER;
            s.wrapModeVertical = GL_CLAMP_TO_BORDER;
            mFbo.allocate( s );
        }
        else {
            mFbo.clear();
        }
    }
}

void VideoPlayerWmf::setIndex( const std::filesystem::path &path )
{
    const auto itr = std::find( mFiles.begin(), mFiles.end(), path );
    if( itr != mFiles.end() )
        mIndex = std::distance( mFiles.begin(), itr );
}

void VideoPlayerWmf::queue( const std::filesystem::path &path )
{
    auto file = path.generic_path();
    if( !exists( file ) )
        file = std::filesystem::path( ofToDataPath( file, true ) ).generic_path();

    if( exists( file ) )
        mFiles.push_back( file );
}

void VideoPlayerWmf::queue( const std::vector<std::filesystem::path> &paths )
{
    for( auto &path : paths )
        queue( path );
}

void VideoPlayerWmf::close()
{
    mPlayer->close();
}

void VideoPlayerWmf::clear()
{
    mFiles.clear();
    mIndex = 0;
}

void VideoPlayerWmf::play()
{
    if( mPlayer->isPaused() ) {
        mPlayer->setPaused( false );
    }
    else if( !mPlayer->isPlaying() ) {
        TS_START( "VideoPlayerWmf::play" );
        mPlayer->setLoopState( mLoop );
        mPlayer->play();

        // Make sure we have a matching fbo.
        allocateFbo();

        TS_STOP( "VideoPlayerWmf::play" );
    }
}

void VideoPlayerWmf::playNext()
{
    if( !mFiles.empty() ) {
        if( !mPlayer->isPlaying() ) {
            stop();
            load( mPlayer, mFiles[mIndex % mFiles.size()] );
            play();

            mQueued->stop();
            load( mQueued, mFiles[++mIndex % mFiles.size()] );
        }
        else if( mQueued->isLoaded() ) {
            stop();
            std::swap( mPlayer, mQueued );
            play();

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

void VideoPlayerWmf::playPrevious()
{
    if( !mFiles.empty() ) {
        if( !mPlayer->isPlaying() ) {
            stop();
            if( mIndex == 0 )
                mIndex = mFiles.size();
            load( mPlayer, mFiles[--mIndex % mFiles.size()] );
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

void VideoPlayerWmf::pause()
{
    if( mPlayer->isPlaying() && !mPlayer->isPaused() ) {
        mPlayer->setPaused( true );
    }
}

void VideoPlayerWmf::stop()
{
    if( mPlayer->isPlaying() || mPlayer->isPaused() ) {
        mPlayer->stop();
    }
}

void VideoPlayerWmf::update()
{
    ofEventArgs dummy;
    update( dummy );
}

void VideoPlayerWmf::update( ofEventArgs &args )
{
    tools::ScopedMarker m( "VideoPlayerWmf:Update" );
    mPlayer->update();

    allocateFbo();

    if( mFbo.isAllocated() ) {
        TS_START( "VideoPlayerWmf::update" );

        /*  if( mPlayer->isPlaying() ) */ {
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
        }

        TS_STOP( "VideoPlayerWmf::update" );
    }

    if( isDone() && mAutomaticQueue ) {
        playNext();
    }
}

void VideoPlayerWmf::draw()
{
    allocateFbo();

    if( mFbo.isAllocated() ) {
        TS_START( "VideoPlayerWmf::draw" );

        tools::ScopedMarker m( "VideoPlayerWmf:Draw" );

        const auto width = ofGetViewportWidth();
        const auto height = ofGetViewportHeight();

        const auto texture = mFbo.getTexture();

        const auto aspect = mFbo.getWidth() / mFbo.getHeight();
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

        TS_STOP( "VideoPlayerWmf::draw" );
    }
}

void VideoPlayerWmf::draw( float x, float y, float w, float h )
{
    allocateFbo();

    if( mFbo.isAllocated() ) {
        TS_START( "VideoPlayerWmf::draw" );

        tools::ScopedMarker m( "VideoPlayerWmf:Draw" );

        const auto texture = mFbo.getTexture();
        texture.draw( x, y, w, h );

        TS_STOP( "VideoPlayerWmf::draw" );
    }
}

const ofTexture &VideoPlayerWmf::getTexture() const
{
    return mFbo.getTexture();
}

ofTexture &VideoPlayerWmf::getTexture()
{
    return mFbo.getTexture();
}

float VideoPlayerWmf::getWidth() const
{
    return mFbo.getWidth();
}

float VideoPlayerWmf::getHeight() const
{
    return mFbo.getHeight();
}

const std::filesystem::path &VideoPlayerWmf::getFile() const
{
    if( !mFiles.empty() )
        return mFiles[mIndex % mFiles.size()];

    return {};
}

bool VideoPlayerWmf::isLoaded() const
{
    return mPlayer->isLoaded();
}

bool VideoPlayerWmf::isPaused() const
{
    return mPlayer->isPaused();
}

bool VideoPlayerWmf::isPlaying() const
{
    return mPlayer->isPlaying();
}

bool VideoPlayerWmf::isDone() const
{
    return mLoop == OF_LOOP_NONE && mPlayer->getIsMovieDone();
}

void VideoPlayerWmf::setAsyncLoading( bool enabled )
{
    mUseAsyncLoading = enabled;
}

float VideoPlayerWmf::getPosition() const
{
    return mPlayer->getPosition();
}

float VideoPlayerWmf::getSpeed() const
{
    return mPlayer->getFrameRate(); // Same as HAP's speed?
}

float VideoPlayerWmf::getDuration() const
{
    return mPlayer->getDuration();
}

ofLoopType VideoPlayerWmf::getLoop() const
{
    return mLoop;
}

void VideoPlayerWmf::setLoop( ofLoopType loop )
{
    mLoop = loop;
}