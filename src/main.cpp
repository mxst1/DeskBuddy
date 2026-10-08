#include <iostream>
#include "models/AppState.h"
#include "services/Data.h"
#include <thread>
#include <chrono>
#include <algorithm>

int main()
{
    while (true)
    {
        Song song = getCurrentSong();

        int remaining = song.duration_ms - song.progress_ms;

        int waitMs = std::min(remaining, 5000);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(waitMs));

        std::cout << "Song: " << song.name << " - " << song.artist << std::endl;
        std::cout << "Progress: " << song.progress_ms << " ms" << " / " << song.duration_ms << " ms" << std::endl;
    }

    return 0;
}
