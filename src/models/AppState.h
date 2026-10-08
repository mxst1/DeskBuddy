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

#endif // APPSTATE_H