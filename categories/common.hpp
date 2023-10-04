#define API_PREFIX "https://api.spotify.com/v1"

#pragma once
#ifndef _SPOTIFY_API_CATEGORY_COMMON_INCLUDES_
#define _SPOTIFY_API_CATEGORY_COMMON_INCLUDES_

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace spotify_api
{
	struct follower_t
	{
		std::string href;
		int total;
	};

	typedef struct
	{
		std::string url;
		int width;
		int height;
	} image_t;

	struct owner_t
	{
		std::map<std::string, std::string> external_urls;
		follower_t followers;
		std::string href = "";
		std::string id = "";
		const std::string type = "user";
		std::string uri = "";
		std::string display_name = "";
	};

	template <typename Item_Type>
	struct page_t
	{
		std::string href = "";
		std::vector<Item_Type> items = {};
		int limit = 20;
		std::string next = "";
		int offset = 0;
		std::string previous = "";
		int total = 0;

		static std::unique_ptr<page_t<Item_Type>> object_from_json(const std::string &json_string, Item_Type (*item_func)(const std::string &));
	};

	std::string truncate_id(const std::string &full_id);
	std::vector<std::string> truncate_ids(const std::vector<std::string> &full_ids, size_t limit = 0);

	void object_from_json(const std::string &json_string, image_t *output);
} // namespace spotify_api

#endif