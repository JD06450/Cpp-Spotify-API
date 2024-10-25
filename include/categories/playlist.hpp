#pragma once
#ifndef _SPOTIFY_API_PLAYLIST_T_
#define _SPOTIFY_API_PLAYLIST_T_

#include <string>
#include <map>
#include <vector>

#include "./common.hpp"
#include "./tracks.hpp"

#include <nlohmann/json.hpp>

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
		spotify_api::page_t<std::shared_ptr<track_t>> tracks;
		static const std::string type;
		std::string uri = "";

		static std::unique_ptr<playlist_t> from_json(const std::string &json_string);

		static std::unique_ptr<playlist_t> from_json(const nlohmann::json &json_object);
	};

} // namespace spotify_api

#endif