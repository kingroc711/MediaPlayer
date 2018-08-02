//
// Created by king on 18-7-16.
//

#include "AudioQueue.h"

AudioQueue::AudioQueue() {
    pthread_mutex_init(&mutexPacket, NULL);
    pthread_cond_init(&condPacket, NULL);
}

AudioQueue::~AudioQueue() {
    pthread_mutex_destroy(&mutexPacket);
    pthread_cond_destroy(&condPacket);
}

int AudioQueue::putAvpacket(AVPacket *packet) {
    pthread_mutex_lock(&mutexPacket);

    queuePacket.push(packet);
    LOGD("push packet %d.\n", queuePacket.size());

    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

int AudioQueue::getAvpacket(AVPacket *packet) {

    while(1){
        pthread_mutex_lock(&mutexPacket);
        if(queuePacket.size() > 0){
            AVPacket *avPacket = queuePacket.front();
            if(av_packet_ref(packet, avPacket) == 0){
                queuePacket.pop();
                av_packet_free(&avPacket);
                av_free(avPacket);
                LOGD("get packet Success queue size : %d\n", queuePacket.size());
            }else{
                LOGD("get packet ref fail.");
            }
            pthread_mutex_unlock(&mutexPacket);
        }else{
            pthread_cond_wait(&condPacket, &mutexPacket);
        }
    }

    return 0;
}
