#pragma once
#ifndef _SPOTIFY_API_PLAYLISTS_
#define _SPOTIFY_API_PLAYLISTS_

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "../categories/common.hpp"
#include "../categories/tracks.hpp"
#include "../categories/playlist.hpp"

#include <nlohmann/json.hpp>

namespace spotify_api
{

class Playlist_API
{
public:
	std::string access_token;
	Playlist_API(std::string access_token): access_token(access_token) {}

	std::unique_ptr<playlist_t> get_playlist();
	std::vector<std::shared_ptr<playlist_t>> get_my_playlists(int limit = 0);
};

} // namespace spotify_api

#endif