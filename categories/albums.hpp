#pragma once
#ifndef _SPOTIFY_API_ALBUMS_
#define _SPOTIFY_API_ALBUMS_

#include <string>
#include <vector>
#include <map>
#include <regex>
#include <cstdint>

#include "artists.hpp"
#include "tracks.hpp"
#include "common.hpp"

namespace spotify_api
{
	typedef struct {
		std::string text;
		std::string type;
	} copyright_t;

	typedef struct
	{
		std::string album_type;
		unsigned int total_tracks;
		std::vector<std::string> available_markets;
		std::map<std::string, std::string> external_urls;
		std::string href;
		std::string id;
		std::vector<image_t> images;
		std::string name;
		std::string release_date;
		std::string release_date_precision;
		std::map<std::string, std::string> restrictions;
		std::vector<copyright_t> copyrights;
		const std::string type = "album";
		std::string uri;
		std::map<std::string, std::string> external_ids;
		std::vector<std::string> genres;
		uint8_t popularity;
		std::string label;
		std::vector<struct artist_t *> artists;
		page_t<struct track_t *> tracks;
	} album_t;

	class Album_API
	{
		public:
		std::string access_token;	

		Album_API(std::string access_token): access_token(access_token) {}

		static album_t * object_from_json(const std::string &json_string);

		album_t *get_album(const std::string &album_id);
		
		std::vector<album_t *> get_albums(const std::vector<std::string> &album_ids);

		page_t<track_t *> get_album_tracks(const std::string &album_id, uint32_t limit = 20, uint32_t offset = 0, const std::string &market = "");

		page_t<album_t *> get_users_albums(uint32_t limit = 20, uint32_t offset = 0, const std::string &market = "");

		void save_albums_for_current_user(const std::vector<std::string> &album_ids);

		void remove_saved_albums_for_current_user(const std::vector<std::string> &album_ids);

		std::vector<bool> check_users_saved_albums(const std::vector<std::string> &album_ids);

		page_t<album_t *> get_new_releases(uint32_t limit = 20, uint32_t offset = 0, const std::string &country = "");
	};

} // namespace spotify_api

#endif
