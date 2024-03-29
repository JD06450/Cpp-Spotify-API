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
		page_t<track_t *> tracks;
		const std::string item_type = "playlist";
		std::string uri = "";

		static std::unique_ptr<playlist_t> from_json(const std::string &json_string);
	};

	class Playlist_API
	{
		public:
		std::string access_token;
		Playlist_API(std::string access_token): access_token(access_token) {}

		playlist_t *get_playlist();
		std::vector<playlist_t *> get_my_playlists(int limit = 0);

		void object_from_json(const std::string &json_string, playlist_t &output);
	};
} // namespace spotify_api

#endif