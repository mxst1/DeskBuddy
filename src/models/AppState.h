#ifndef APPSTATE_H
#define APPSTATE_H
#include <string>

struct Song
{
    std::string name = "";
    std::string artist = "";
    int progress_ms = 0;
    int duration_ms = 0;
    bool is_playing = false;
    std::string id = "";
};

struct Weather
{
    std::string description = "";
    float temperature = 0.0;
    float high = 0.0;
    float low = 0.0;
};

#endif // APPSTATE_H