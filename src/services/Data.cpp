#include <iostream>
#include <string>
#include "Data.h"
#include "../models/AppState.h"
#include "../httplib.h"
#include "../json.hpp"
#include "Spotify.h"

std::string access_token;

using json = nlohmann::json;

Song getCurrentSong()
{
    if (access_token.empty())
        access_token = refreshAccessToken();

    httplib::Client cli("https://api.spotify.com");

    httplib::Headers headers = {
        {"Authorization", "Bearer " + access_token}};

    auto res = cli.Get("/v1/me/player/currently-playing", headers);

    if (res && res->status == 200)
    {
        try
        {
            // Parse the response body string into JSON
            json data = json::parse(res->body);

            Song song;
            song.name = data["item"]["name"];
            song.artist = data["item"]["artists"][0]["name"];
            song.progress_ms = data["progress_ms"];
            song.duration_ms = data["item"]["duration_ms"];
            song.is_playing = data["is_playing"];
            song.id = data["item"]["id"];
            return song;
        }
        catch (const json::parse_error &e)
        {
            std::cerr << "JSON Parsing Error: " << e.what() << std::endl;
        }
    }
    else if (res && res->status == 204)
    {
        std::cerr << "No content. The user is not currently playing any song." << std::endl;
    }
    else
    {
        std::cerr << "HTTP request failed. ";
        if (res)
        {
            std::cerr << "Status code: " << res->status << std::endl;
        }
        else
        {
            std::cerr << "Error code: " << (int)res.error() << std::endl;
        }
    }

    return Song();
}
