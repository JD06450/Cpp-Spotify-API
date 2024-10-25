#include "categories/common.hpp"

#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{

image_t image_t::from_json(const json::json &json_object)
{
	image_t new_image;

	try
	{
		new_image.url = json_object["url"];
		new_image.width = !json_object["width"].is_null() ? json_object["width"].get<int>() : -1;
		new_image.height = !json_object["height"].is_null() ? json_object["height"].get<int>() : -1;
	}
	catch (std::exception e)
	{
		// TODO: Proper error handling
		return image_t();
	}

	return new_image;
}

std::string truncate_spotify_uri(const std::string &full_id)
{
	size_t id_start = full_id.find_last_of(':');
	return id_start != std::string::npos ? full_id.substr(id_start + 1) : full_id;
}

std::vector<std::string> truncate_spotify_uris(const std::vector<std::string> &full_ids, size_t limit)
{
	std::vector<std::string> truncated_ids;

	int i = 0;

	for (std::string id : full_ids)
	{
		if (limit && i == limit) break;
		size_t id_start = id.find_last_of(':');
		truncated_ids.push_back(id_start != std::string::npos ? id.substr(id_start + 1) : id);
		i++;
	}

	return truncated_ids;
}

} // namespace spotify_api
