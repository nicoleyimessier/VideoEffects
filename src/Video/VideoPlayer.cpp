#include "VideoPlayer.h"

#include "VideoPlayerHap.h"
#include "VideoPlayerWmf.h"

Ref<VideoPlayer> VideoPlayer::create( Backend backend )
{
    switch( backend ) {
    case Backend::WMF:
        return CreateRef<VideoPlayerWmf>();
    case Backend::HAP:
        return CreateRef<VideoPlayerHap>();
    }

    return nullptr;
}
