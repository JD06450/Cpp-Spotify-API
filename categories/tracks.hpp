#pragma once
#ifndef _SPOTIFY_API_TRACKS_
#define _SPOTIFY_API_TRACKS_

#include <string>
#include <vector>
#include <optional>

#include "albums.hpp"
#include "artists.hpp"
#include "common.hpp"

namespace spotify_api
{
	struct track_t
	{
		struct album_t *album;
		std::vector<artist_t *> artists;
		std::vector<std::string> available_markets;
		int disc_number;
		int duration_ms;
		bool is_explicit;
		std::map<std::string, std::string> external_ids;
		std::map<std::string, std::string> external_urls;
		std::string href;
		std::string id;
		bool is_playable;
		std::optional<track_t *> linked_from;
		std::map<std::string, std::string> restrictions;
		std::string name;
		int popularity;
		std::string preview_url;
		int track_number;
		const std::string type = "track";
		std::string uri;
		bool is_local;
	};

	class Track_API
	{
		public:
		std::string access_token;
		Track_API(std::string access_token): access_token(access_token) {}
		static void object_from_json(const std::string &json_string, track_t *output);
		track_t *search_for_track(const std::string &search_query);
	};
} // namespace spotify_api

#endif