//
// Created by king on 18-7-6.
//

#ifndef MEDIAPLAYER_ANDROIDLOG_H
#define MEDIAPLAYER_ANDROIDLOG_H
#include <android/log.h>

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO, "cnplayer", FORMAT, ##__VA_ARGS__);
#define LOGD(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG, "cnplayer", FORMAT, ##__VA_ARGS__);
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR, "cnplayer", FORMAT, ##__VA_ARGS__);


#endif //MEDIAPLAYER_ANDROIDLOG_H
