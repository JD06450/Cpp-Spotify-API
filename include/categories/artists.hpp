#pragma once
#ifndef _SPOTIFY_API_ARTIST_T_
#define _SPOTIFY_API_ARTIST_T_

#include <string>
#include <vector>
#include <map>

#include <nlohmann/json.hpp>
#include "../curl-util.hpp"

namespace spotify_api {struct artist_t;}

// #include "./albums.hpp"
#include "./common.hpp"
#include "./tracks.hpp"

namespace spotify_api
{

struct artist_t
{
	std::map<std::string, std::string> external_urls;
	follower_t followers;
	std::vector<std::string> genres;
	std::string href;
	std::string id;
	std::vector<image_t> images;
	std::string name;
	int popularity;
	static const std::string type;
	std::string uri;

	static std::unique_ptr<artist_t> from_json(const std::string &json_string);

	static std::unique_ptr<artist_t> from_json(const nlohmann::json &json_object);
};

} // namespace spotify_api

#endif