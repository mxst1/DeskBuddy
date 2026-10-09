#ifndef DATA_H
#define DATA_H
#include "../models/AppState.h"

#ifdef __cplusplus
extern "C" {
#endif

Song getCurrentSong();
Weather getCurrentWeather();
bool downloadAlbumArt(const char *url, const char *path);

#ifdef __cplusplus
}
#endif

#endif // DATA_H