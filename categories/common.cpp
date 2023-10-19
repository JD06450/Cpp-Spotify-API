#include "common.hpp"
#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{

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

template <typename Item_Type>
page_t<Item_Type> page_t<Item_Type>::from_json(const std::string &json_string, Item_Type (*item_func)(const std::string &))
{
	page_t<Item_Type> new_page;

	json::json json_object = json::json::parse(json_string);

	new_page->href = json_object["href"];
	new_page->limit = json_object["limit"];
	new_page->offset = json_object["offset"];
	new_page->total = json_object["total"];
	new_page->next = json_object.value("next", "");
	new_page->previous = json_object.value("previous", "");
	
	for (auto item = json_object["items"].begin(); item != json_object["items"].end(); ++item)
	{
		new_page->items.push_back(item_func(item.value().dump()));
	}
	
	return new_page;
}

// Even though its not that much code, I still feel it should be extracted
// into its own function just to keep from repeating code.

void object_from_json(const std::string &json_string, image_t *output)
{
	json::json json_object = json::json::parse(json_string);
	try
	{
		output->url = json_object["url"];
		output->width = !json_object["width"].is_null() ? json_object["width"].get<int>() : -1;
		output->height = !json_object["height"].is_null() ? json_object["height"].get<int>() : -1;
	}
	catch (std::exception e)
	{
		// TODO: Proper error handling
		return;
	}
}

} // namespace spotify_api
