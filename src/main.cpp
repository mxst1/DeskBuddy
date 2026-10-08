#include <iostream>
#include "models/AppState.h"
#include "services/Data.h"
#include <thread>
#include <chrono>
#include <algorithm>

int main()
{
    std::thread weatherThread([]()
                              {
                                  while (true)
                                  {
                                      Weather weather = getCurrentWeather();
                                      std::cout << "Weather: " << weather.temperature << " - "
                                                << weather.description << "\nHigh: " 
                                                << weather.high << " Low: " << weather.low 
                                                << std::endl;

                                      std::this_thread::sleep_for(
                                          std::chrono::hours(1));
                                  } });

    while (true)
    {
        Song song = getCurrentSong();

        int remaining = song.duration_ms - song.progress_ms;

        int waitMs;
        if (song.duration_ms <= 0)
            waitMs = 5000;
        else
            waitMs = std::min(remaining, 5000);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(waitMs));

        std::cout << "Song: " << song.name << " - " << song.artist << std::endl;
        std::cout << "Progress: " << song.progress_ms << " ms" << " / " << song.duration_ms << " ms" << std::endl;
    }

    weatherThread.join();
    return 0;
}
