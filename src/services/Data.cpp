#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include "Data.h"
#include "../models/AppState.h"
#include "../libs/httplib.h"
#include "../libs/json.hpp"
#include "Spotify.h"

std::string access_token;

using json = nlohmann::json;

namespace
{
    template <size_t Size>
    void copyString(char (&destination)[Size], const std::string &source)
    {
        std::strncpy(destination, source.c_str(), Size - 1);
        destination[Size - 1] = '\0';
    }

    std::string weatherDescriptionFromCode(int code)
    {
        switch (code)
        {
        case 0:
            return "Clear sky";
        case 1:
            return "Mainly clear";
        case 2:
            return "Partly cloudy";
        case 3:
            return "Overcast";
        case 45:
            return "Fog";
        case 48:
            return "Depositing rime fog";
        case 51:
            return "Light drizzle";
        case 53:
            return "Moderate drizzle";
        case 55:
            return "Dense drizzle";
        case 56:
            return "Light freezing drizzle";
        case 57:
            return "Dense freezing drizzle";
        case 61:
            return "Slight rain";
        case 63:
            return "Moderate rain";
        case 65:
            return "Heavy rain";
        case 66:
            return "Light freezing rain";
        case 67:
            return "Heavy freezing rain";
        case 71:
            return "Slight snowfall";
        case 73:
            return "Moderate snowfall";
        case 75:
            return "Heavy snowfall";
        case 77:
            return "Snow grains";
        case 80:
            return "Slight rain showers";
        case 81:
            return "Moderate rain showers";
        case 82:
            return "Violent rain showers";
        case 85:
            return "Slight snow showers";
        case 86:
            return "Heavy snow showers";
        case 95:
            return "Thunderstorm";
        case 96:
            return "Thunderstorm with slight hail";
        case 97:
            return "Heavy thunderstorm";
        case 99:
            return "Thunderstorm with heavy hail";
        default:
            return "Unknown weather condition";
        }
    }
}

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

            Song song = {};
            copyString(song.name, data["item"]["name"].get<std::string>());
            copyString(song.artist, data["item"]["artists"][0]["name"].get<std::string>());
            song.progress_ms = data["progress_ms"];
            song.duration_ms = data["item"]["duration_ms"];
            song.is_playing = data["is_playing"];
            copyString(song.id, data["item"]["id"].get<std::string>());
            if (!data["item"]["album"]["images"].empty())
            {
                copyString(song.album_art_url,
                           data["item"]["album"]["images"][0]["url"].get<std::string>());
            }
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

Weather getCurrentWeather()
{
    httplib::Client cli("https://api.open-meteo.com");

    auto res = cli.Get("/v1/forecast?latitude=43.06&longitude=-88.11&daily=apparent_temperature_min,apparent_temperature_max&hourly=temperature_2m,weather_code&timezone=America%2FChicago");

    if (res && res->status == 200)
    {
        try
        {
            json data = json::parse(res->body);
            Weather weather = {};
            weather.temperature = data["hourly"]["temperature_2m"][0];
            copyString(weather.description, weatherDescriptionFromCode(
                data["hourly"]["weather_code"][0].get<int>()));
            weather.high = data["daily"]["apparent_temperature_max"][0];
            weather.low = data["daily"]["apparent_temperature_min"][0];
            return weather;
        }
        catch (const json::parse_error &e)
        {
            std::cerr << "JSON Parsing Error: " << e.what() << std::endl;
        }
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

    return Weather();
}

bool downloadAlbumArt(const char *url, const char *path)
{
    if (url == nullptr || path == nullptr || url[0] == '\0')
        return false;

    httplib::detail::UrlComponents url_components;
    if (!httplib::detail::parse_url(url, url_components) ||
        url_components.scheme != "https" || url_components.host.empty() ||
        url_components.path.empty())
    {
        std::cerr << "Invalid album art URL." << std::endl;
        return false;
    }

    httplib::Client cli(url_components.scheme + "://" + url_components.host);
    auto res = cli.Get(url_components.path +
                       (url_components.query.empty() ? "" : "?" + url_components.query));
    if (!res || res->status != 200)
    {
        std::cerr << "Album art download failed." << std::endl;
        return false;
    }

    std::ofstream output(path, std::ios::binary);
    if (!output)
    {
        std::cerr << "Could not open album art file for writing." << std::endl;
        return false;
    }

    output.write(res->body.data(), static_cast<std::streamsize>(res->body.size()));
    return output.good();
}