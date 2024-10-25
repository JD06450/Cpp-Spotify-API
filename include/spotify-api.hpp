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
#include <mutex>
#include <map>
#include <condition_variable>


#include "categories/common.hpp"
#include "categories/session.hpp"
#include "categories/tracks.hpp"
#include "categories/artists.hpp"
#include "categories/albums.hpp"
#include "categories/episodes.hpp"
#include "categories/player.hpp"
#include "categories/shows.hpp"
#include "categories/playlist.hpp"
#include "categories/search.hpp"

#include "endpoints/common.hpp"
#include "endpoints/session.hpp"
#include "endpoints/tracks.hpp"
#include "endpoints/artists.hpp"
#include "endpoints/albums.hpp"
#include "endpoints/episodes.hpp"
#include "endpoints/player.hpp"
#include "endpoints/shows.hpp"
#include "endpoints/playlist.hpp"
#include "endpoints/search.hpp"

namespace spotify_api
{
	// Authentication Process Step 2
	// Using the authentication code retrieved from the Spotify Web API, we send another request to generate an access token.

	class Spotify_API
	{
		private:
		std::string      _access_token;

		public:
		Album_API *      album_api;
		Artist_API *     artist_api;
		Episode_API *    episode_api;
		Player_API *     player_api;
		Playlist_API *   playlist_api;
		Track_API *      track_api;
		Search_API *     search_api;

		//TODO: look into possibly merging Session_API into Spotify_API

		Session_API *    session_api;

		/// Start a new Spotify session using an auth code and client keys
		Spotify_API(std::string &auth_code, const std::string &redirect_uri, std::string &client_keys_base64);

		~Spotify_API();
		/**
		 * @brief This function will update the main thread's version of the access token with the value stored in the background thread.
		*/
		void resync_access_token();
	};
} // namespace spotify_api

#endif