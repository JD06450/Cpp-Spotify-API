#pragma once
#ifndef _SPOTIFY_API_PLAYLISTS_
#define _SPOTIFY_API_PLAYLISTS_

#include <string>
#include <map>
#include <vector>

#include "./common.hpp"
#include "./tracks.hpp"

namespace spotify_api
{
	struct playlist_t
	{
		bool collaborative;
		std::string description = "";
		std::map<std::string, std::string> external_urls;
		follower_t followers;
		std::string href = "";
		std::string id = "";
		std::vector<image_t> images;
		std::string name = "";
		owner_t owner;
		bool is_public;
		std::string snapshot_id = "";
		batch_t<track_t> tracks;
		const std::string type = "playlist";
		std::string uri = "";
	};

	class Playlist_API
	{
		private:
		const std::atomic<std::string> &_access_token;
		
		public:
		Playlist_API(const std::atomic<std::string> &access_token): _access_token(access_token) {}

		playlist_t *get_playlist();
		std::vector<playlist_t *> get_my_playlists(int limit = 0);

		void object_from_json(const std::string &json_string, playlist_t &output);
	};
} // namespace spotify_api

#endif