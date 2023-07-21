#include "common.hpp"
#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{

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
