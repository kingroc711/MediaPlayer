//
// Created by king on 18-7-16.
//

#include "AudioQueue.h"

AudioQueue::AudioQueue() {
    pthread_mutex_init(&this->mutexPacket, NULL);
    pthread_cond_init(&this->condPacket, NULL);
    this->dataSize = 0;
}

AudioQueue::~AudioQueue() {
    pthread_mutex_destroy(&this->mutexPacket);
    pthread_cond_destroy(&this->condPacket);

    if(this->queuePacket.size() > 0){
        AVPacket *avPacket = this->queuePacket.front();
        this->queuePacket.pop();
        av_packet_free(&avPacket);
        av_free(avPacket);
    }
}

void AudioQueue::putAvpacket(AVPacket *packet) {
    pthread_mutex_lock(&this->mutexPacket);
    this->queuePacket.push(packet);
    this->dataSize = this->dataSize + packet->size;

    pthread_cond_signal(&this->condPacket);
    pthread_mutex_unlock(&this->mutexPacket);
}

int AudioQueue::getAvpacket(AVPacket **packet) {

        pthread_mutex_lock(&this->mutexPacket);
        if(this->queuePacket.size() > 0){
            *packet = this->queuePacket.front();
            this->dataSize = this->dataSize - (*packet)->size;
            this->queuePacket.pop();
            pthread_mutex_unlock(&this->mutexPacket);
            return 0;
        }else{
            pthread_cond_wait(&this->condPacket, &this->mutexPacket);
            return -1;
        }
}

int AudioQueue::size() {
    return this->queuePacket.size();
}

unsigned int AudioQueue::getDataSize() {
    return this->dataSize;
}
