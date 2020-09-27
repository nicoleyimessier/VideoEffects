#pragma once

#include "VideoPlayer.h"
#include "ofxHapPlayer.h"

class VideoPlayerHap : public VideoPlayer {
  public:
    VideoPlayerHap();
    ~VideoPlayerHap();

    void load( const std::filesystem::path &path ) override;
    void queue( const std::filesystem::path &path ) override;
    void queue( const std::vector<std::filesystem::path> &paths ) override;
    void close() override;
    void clear() override;

    void play() override;
    void playNext() override;
    void playPrevious() override;
    void pause() override;
    void stop() override;

    void update() override;
    void update( ofEventArgs &args ) override;
    void draw() override;
    void draw( float x, float y ) override { draw( x, y, getWidth(), getHeight() ); }
    void draw( float x, float y, float w, float h ) override;

    Backend                      getBackend() const override { return Backend::HAP; }
    const ofTexture &            getTexture() const override;
    float                        getWidth() const override;
    float                        getHeight() const override;
    const std::filesystem::path &getFile() const override;

    __declspec( deprecated ) ofTexture &getTexture() override; // Prefer using the const version.

    bool isLoaded() const override;
    bool isPaused() const override;
    bool isPlaying() const override;
    bool isDone() const override;
    bool isLetterBoxed() const override { return mIsLetterBoxed; }
    bool isFlipped() const override { return mIsFlipped; } // TODO
    bool isAsyncLoading() const override { return false; }
    bool hasTexture() const override { return mFbo.isAllocated(); }

    void  setAsyncLoading( bool enabled ) override {} // TODO
    void  setLetterBoxed( bool enabled = true ) override { mIsLetterBoxed = enabled; }
    void  setFlipped( bool enabled = true ) override { mIsFlipped = enabled; } // TODO
    void  setAutomaticQueue( bool automaticQueue = true ) override { mAutomaticQueue = automaticQueue; }
    float getPosition() const override;
    float getSpeed() const override;
    float getDuration() const override;

    ofLoopType getLoop() const override;
    void       setLoop( ofLoopType loop ) override;

  private:
    void load( const std::shared_ptr<ofxHapPlayer> &player, const std::filesystem::path &path ) const;

    std::shared_ptr<ofxHapPlayer>      mPlayer;
    std::shared_ptr<ofxHapPlayer>      mQueued;
    std::vector<std::filesystem::path> mFiles;
    size_t                             mIndex = 0;
    ofLoopType                         mLoop = OF_LOOP_NONE;
    ofFbo                              mFbo;
    bool                               mIsPlaying = false;
    bool                               mIsLetterBoxed = true;
    bool                               mIsFlipped = false;
    bool                               mAutomaticQueue = true;
    std::filesystem::path              mPath;
};
