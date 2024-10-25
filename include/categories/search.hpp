#pragma once
#ifndef _SPOTIFY_API_SEARCH_T_
#define _SPOTIFY_API_SEARCH_T_

#include <memory>
#include <optional>

#include "categories/common.hpp"

// Types are needed for the search_result struct.
// I could probably use forward declarations but IDGAF

#include "categories/albums.hpp"
#include "categories/artists.hpp"
#include "categories/tracks.hpp"
#include "categories/playlist.hpp"
#include "categories/shows.hpp"
#include "categories/episodes.hpp"

#include <nlohmann/json.hpp>

namespace spotify_api
{

struct search_type
{
	bool album : 1 = false;
	bool artist : 1 = false;
	bool playlist : 1 = false;
	bool track : 1 = true;
	bool show : 1 = false;
	bool episode : 1 = false;

	search_type(
		bool album,
		bool artist,
		bool playlist,
		bool track,
		bool show,
		bool episode);
	
	search_type() = default;
};

struct search_result
{
	std::optional<page_t<std::shared_ptr<album_t>>> albums;
	std::optional<page_t<std::shared_ptr<artist_t>>> artists;
	std::optional<page_t<std::shared_ptr<track_t>>> tracks;
	std::optional<page_t<std::shared_ptr<playlist_t>>> playlists;
	std::optional<page_t<std::shared_ptr<show_t>>> shows;
	std::optional<page_t<std::shared_ptr<episode_t>>> episodes;

	static std::unique_ptr<search_result> from_json(const std::string &json_string);
	static std::unique_ptr<search_result> from_json(const nlohmann::json &json_obj);
};

} // namespace spotify_api


#endif