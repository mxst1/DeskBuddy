#ifndef APPSTATE_H
#define APPSTATE_H
#include <stdbool.h>

typedef struct Song
{
    char name[256];
    char artist[256];
    long progress_ms;
    long duration_ms;
    bool is_playing;
    char id[128];
    char album_art_url[512];
} Song;

typedef struct Weather
{
    char description[128];
    float temperature;
    float high;
    float low;
} Weather;

#endif // APPSTATE_H