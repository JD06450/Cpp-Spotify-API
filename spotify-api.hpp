#ifndef _SPOTIFY_API_FILE_
#define _SPOTIFY_API_FILE_

#include <iostream>
#include <optional>
#include <unistd.h>
#include <chrono>
#include <string>
#include <future>
#include <thread>
#include <vector>
#include <regex>
#include <ctime>
#include <map>

#include <nlohmann/json.hpp>
#include "curl-util.hpp"

#include "categories/albums.hpp"
#include "categories/artists.hpp"
#include "categories/common.hpp"
#include "categories/episodes.hpp"
#include "categories/player.hpp"
#include "categories/playlist.hpp"
#include "categories/session.hpp"
#include "categories/shows.hpp"
#include "categories/tracks.hpp"

namespace spotify_api
{
	// Authentication Process Step 2
	// Using the authentication code retrieved from the Spotify Web API, we send another request to generate an access token.

	class Spotify_API
	{
	private:
		Album_API *		_album_api;
		Artist_API *	_artist_api;
		Episode_API *	_episode_api;
		Player_API *	_player_api;
		Playlist_API *	_playlist_api;
		//TODO: look into possibly merging Session_API into Spotify_API

		Session_API *	_session_api;
		std::string		_access_token;

	public:
		/// Start a new Spotify session using an auth code and client keys
		Spotify_API(std::string &auth_code, const std::string &redirect_uri, std::string &client_keys_base64);
	};
} // namespace spotify_api

#endif