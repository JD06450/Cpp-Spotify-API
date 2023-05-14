#pragma once
#ifndef _SPOTIFY_API_ALBUMS_
#define _SPOTIFY_API_ALBUMS_

#include <string>
#include <vector>
#include <map>
#include <regex>

#include "artists.hpp"
#include "tracks.hpp"
// #include "session.hpp"
#include "common.hpp"

namespace spotify_api
{
	struct album_t
	{
		std::string album_type;
		int total_tracks;
		std::vector<std::string> available_markets;
		std::map<std::string, std::string> external_urls;
		std::string href;
		std::string id;
		std::vector<image_t> images;
		std::string name;
		std::string release_date;
		std::string release_date_precision;
		std::map<std::string, std::string> restrictions;
		std::map<std::string, std::string> copyrights;
		const std::string type = "album";
		std::string uri;
	};

	struct album_context_t : public album_t
	{
		std::vector<artist_t *> artists;
		std::string album_group;

		album_context_t(const album_t &base) : album_t(base) {};
	};

	struct album_full_t : public album_t
	{
		std::vector<artist_t *> artists;
		batch_t<track_t *> tracks;

		album_full_t(const album_t &base) : album_t(base) {};
		album_full_t(const album_context_t &base) : album_t(base), artists(base.artists) {};
	};

	class Album_API
	{
		private:
			std::string _access_token;	
		public:
			album_t *get_album(const std::string &album_id);
			std::vector<album_t> get_albums(const std::vector<std::string> &album_ids);
			void static object_from_json(const std::string &json_string, album_t *output);
	};

} // namespace spotify_api

#endif
