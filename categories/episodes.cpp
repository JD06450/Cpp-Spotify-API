#include "episodes.hpp"

#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{
	std::unique_ptr<episode_t> episode_t::from_json(const std::string &json_string)
	{
		auto output = std::make_unique<episode_t>();

		json::json json_object = json::json::parse(json_string);

		if (!json_object["audio_preview_url"].is_null()) {
			output->audio_preview_url = json_object["audio_preview_url"];
		}

		output->description = json_object["description"];
		output->html_description = json_object["html_description"];
		output->duration_ms = json_object["duration_ms"];
		output->is_explicit = json_object["explicit"];
		
		json::json temp_obj = json_object["external_urls"];

		for (auto i = temp_obj.begin(); i != temp_obj.end(); ++i)
		{
			output->external_urls.emplace(i.key(), i.value());
		}
		
		output->href = json_object["href"];
		output->id = json_object["id"];

		temp_obj = json_object["images"];

		for (auto i = temp_obj.begin(); i != temp_obj.end(); ++i)
		{
			image_t temp_img;
			temp_img.url = i.value()["url"];
			temp_img.width = i.value()["width"];
			temp_img.height = i.value()["height"];
			output->images.push_back(temp_img);
		}

		output->is_externally_hosted = json_object["is_externally_hosted"];

		output->is_playable = json_object["is_playable"];
		
		temp_obj = json_object["languages"];
		for (auto i = temp_obj.begin(); i != temp_obj.end(); ++i)
		{
			output->languages.push_back(i.value());
		}

		output->name = json_object["name"];
		output->release_date = json_object["release_date"];
		output->release_date_precision = json_object["release_date_precision"];

		output->resume_point.fully_played = json_object["resume_point"]["fully_played"];
		output->resume_point.resume_position_ms = json_object["resume_point"]["resume_position_ms"];
		
		if (!json_object["restrictions"].is_null() && json_object["restrictions"].is_string())
		{
			output->restrictions = json_object["restrictions"]["reason"];
		}


		Show_API::object_from_json(json_object["show"].dump(), output->show);

		output->uri = json_object["uri"];
	}
} // namespace spotify_api
