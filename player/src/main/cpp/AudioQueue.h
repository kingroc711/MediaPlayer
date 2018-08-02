//
// Created by king on 18-7-16.
//

#ifndef MEDIAPLAYER_AUDIOQUEUE_H
#define MEDIAPLAYER_AUDIOQUEUE_H

#include <queue>
#include <pthread.h>
#include "AndroidLog.h"

extern "C"
{
#include <libavformat/avformat.h>
}


class AudioQueue {

public:
    std::queue<AVPacket*>queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;

public:
    AudioQueue();
    ~AudioQueue();

    int putAvpacket(AVPacket* packet);
    int getAvpacket(AVPacket* packet);

};


#endif //MEDIAPLAYER_AUDIOQUEUE_H
