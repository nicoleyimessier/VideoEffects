#pragma once

#include "ofConstants.h"
#include "ofEvents.h"
#include "ofTexture.h"
#include "Memory.h"

#if OF_USING_STD_FS
#if __cplusplus < 201703L
#include <experimental/filesystem>
namespace std {
namespace filesystem = experimental::filesystem;
}
#else
#include <filesystem>
#endif
#else
#if !_MSC_VER
#define BOOST_NO_CXX11_SCOPED_ENUMS
#define BOOST_NO_SCOPED_ENUMS
#endif
#include <boost/filesystem.hpp>
namespace std {
namespace filesystem = boost::filesystem;
}
#endif

class VideoPlayer {
  public:
    enum class Backend { HAP, WMF };

    virtual ~VideoPlayer() = default;

    virtual void load( const std::filesystem::path &path ) = 0;
    virtual void queue( const std::filesystem::path &path ) = 0;
    virtual void queue( const std::vector<std::filesystem::path> &paths ) = 0;
    virtual void close() = 0;
    virtual void clear() = 0;

    virtual void play() = 0;
    virtual void playNext() = 0;
    virtual void playPrevious() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;

    virtual void update() = 0;
    virtual void update( ofEventArgs &args ) = 0;
    virtual void draw() = 0;
    virtual void draw( float x, float y ) = 0;
    virtual void draw( float x, float y, float w, float h ) = 0;

    // virtual void drawDebug() = 0;
    // virtual void lock() = 0;
    // virtual void unlock() = 0;

    virtual Backend                      getBackend() const = 0;
    virtual const ofTexture &            getTexture() const = 0;
    virtual float                        getWidth() const = 0;
    virtual float                        getHeight() const = 0;
    virtual const std::filesystem::path &getFile() const = 0;

    __declspec( deprecated ) virtual ofTexture &getTexture() = 0; // Prefer using the const version.

    virtual bool isLoaded() const = 0;
    virtual bool isPaused() const = 0;
    virtual bool isPlaying() const = 0;
    virtual bool isDone() const = 0;
    virtual bool isLetterBoxed() const = 0;
    virtual bool isFlipped() const = 0;
    virtual bool isAsyncLoading() const = 0;
    virtual bool hasTexture() const = 0;

    virtual void setAsyncLoading( bool enabled = true ) = 0;
    virtual void setLetterBoxed( bool enabled = true ) = 0;
    virtual void setFlipped( bool enabled = true ) = 0;
    virtual void setAutomaticQueue( bool automaticQueue = true ) = 0; 

    virtual float getPosition() const = 0;
    virtual float getSpeed() const = 0;
    virtual float getDuration() const = 0;

    virtual ofLoopType getLoop() const = 0;
    virtual void       setLoop( ofLoopType state ) = 0;

    static Ref<VideoPlayer> create( Backend = Backend::HAP ); // HAP requires specially encoded videos.

  protected:
    VideoPlayer() = default;
};