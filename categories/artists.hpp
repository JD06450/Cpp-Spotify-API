#pragma once
#ifndef _SPOTIFY_API_ARTISTS_
#define _SPOTIFY_API_ARTISTS_

#include <string>
#include <vector>
#include <map>

#include "./common.hpp"

#include "../curl-util.hpp"

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
		const std::string type = "artist";
		std::string uri;
	};

	class Artist_API
	{
	private:
		std::string _access_token;
	public:
		void static object_from_json(const std::string &json_string, artist_t *output);
	};

} // namespace spotify_api

#endif
