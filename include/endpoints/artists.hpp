#pragma once
#ifndef _SPOTIFY_API_ARTISTS_
#define _SPOTIFY_API_ARTISTS_

#include <string>
#include <vector>
#include <map>

#include <nlohmann/json.hpp>

#include "../curl-util.hpp"
#include "../categories/common.hpp"
#include "../categories/tracks.hpp"
#include "../categories/albums.hpp"

namespace spotify_api
{
	class Artist_API
	{
	public:
		std::string access_token;

		Artist_API(std::string access_token): access_token(access_token) {}

		std::unique_ptr<artist_t> get_artist(const std::string &artist_id);

		std::vector<std::unique_ptr<artist_t>> get_artists(const std::vector<std::string> &artist_ids);

		page_t<std::unique_ptr<album_t>> get_albums_from_artist(const std::string &artist_id, std::vector<std::string> &include_groups, std::string &market, uint8_t limit, uint32_t offset);

		std::vector<std::unique_ptr<track_t>> get_artist_top_tracks(const std::string &artist_id, const std::string &market);

		std::vector<std::unique_ptr<artist_t>> get_related_artists(const std::string &artist_id);
	};

} // namespace spotify_api

#endif
