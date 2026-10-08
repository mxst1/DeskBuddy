#include <iostream>
#include "Spotify.h"
#include "../httplib.h"
#include "../json.hpp"

using json = nlohmann::json;

std::string refreshAccessToken()
{
    const char* clientIdEnv = getenv("SPOTIFY_CLIENT_ID");
    const char* refreshTokenEnv = getenv("SPOTIFY_REFRESH_TOKEN");

    if (!clientIdEnv || !refreshTokenEnv)
    {
        std::cerr << "Spotify credentials are missing." << std::endl;
        return "";
    }

    std::string client_id = clientIdEnv;
    std::string refresh_token = refreshTokenEnv;

    httplib::Client cli("https://accounts.spotify.com");
    std::string access_token = "";
    httplib::Params params = {
        {"grant_type", "refresh_token"},
        {"refresh_token", refresh_token},
        {"client_id", client_id}
    };

    auto res = cli.Post("/api/token", params);

    if (res && res->status == 200)
    {
        json data = json::parse(res->body);
        return data["access_token"];
    }

    std::cerr << "Failed to refresh token." << std::endl;
    return "";
}