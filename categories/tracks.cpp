#include <iostream>
#include "albums.hpp"
#include "artists.hpp"
#include "tracks.hpp"

namespace json = nlohmann;

namespace spotify_api
{

track_t *Track_API::search_for_track(const std::string &search_query)
{
	track_t *retval = new track_t;
	std::string query_string = "q=" + http::url_encode(search_query) + "&type=track&limit=1";
	http::api_response res = http::get(API_PREFIX "/search", query_string, this->_access_token);

	if (res.code == 200)
	{
		json::json json_res = json::json::parse(res.body);
		object_from_json(json_res["tracks"]["items"][0].dump(), retval);
	}
	return retval;
}


void Track_API::object_from_json(const std::string &json_string, track_t *output)
{

	int step = 1;
	try
	{
		json::json json_object = json::json::parse(json_string);
		if (json_object.contains("item"))
		{
			json_object = json_object["item"];
		}

		if (json_object.contains("album"))
		{
			spotify_api::album_t *temp = new spotify_api::album_t;
			Album_API::object_from_json(json_object["album"].dump(), temp);
			output->album = temp;
		}

		json::json temp_obj = json_object["artists"];
		output->artists.reserve(temp_obj.size());
		for (auto artist = temp_obj.begin(); artist != temp_obj.end(); ++artist)
		{
			spotify_api::artist_t *temp = new spotify_api::artist_t;
			Artist_API::object_from_json(artist.value().dump(), temp);
			output->artists.push_back(temp);
		}
		output->artists.shrink_to_fit();

		step++;

		temp_obj = json_object["available_markets"];
		output->available_markets.reserve(temp_obj.size());
		for (auto market = temp_obj.begin(); market != temp_obj.end(); ++market)
		{
			output->available_markets.push_back(market.value());
		}
		output->available_markets.shrink_to_fit();

		step++;

		output->disc_number = json_object["disc_number"];
		step++;
		output->duration_ms = json_object["duration_ms"];
		step++;

		if (json_object.contains("external_ids"))
		{
			temp_obj = json_object["external_ids"];
			for (auto ext_id = temp_obj.begin(); ext_id != temp_obj.end(); ++ext_id)
			{
				output->external_ids.insert(std::pair<std::string, std::string>(ext_id.key(), ext_id.value()));
			}
		}

		step++;

		temp_obj = json_object["external_urls"];
		for (auto ext_url = temp_obj.begin(); ext_url != temp_obj.end(); ++ext_url)
		{
			output->external_urls.insert(std::pair<std::string, std::string>(ext_url.key(), ext_url.value()));
		}

		step++;

		output->href = json_object["href"];
		step++;
		output->id = json_object["id"];
		step++;
		output->is_explicit = json_object["explicit"];
		step++;
		output->is_local = json_object["is_local"];
		step++;
		if (json_object.contains("is_playable"))
			output->is_playable = json_object["is_playable"];

		step++;

		if (json_object.contains("linked_from"))
		{
			track_t *linked_track;                                         // We use a pointer here so there wont be a dangling pointer when the function returns
			object_from_json(json_object["linked_from"], linked_track); // Could recurse infinitely and cause stack overflow. Maybe look into this.
			output->linked_from.emplace(linked_track);
		}
		else
		{
			output->linked_from.reset();
		}

		step++;

		output->name = json_object["name"];
		step++;
		if (json_object.contains("popularity"))
			output->popularity = json_object["popularity"];
		step++;
		if (!json_object["preview_url"].is_null())
			output->preview_url = json_object["preview_url"];
		step++;

		if (json_object.contains("restrictions"))
		{
			temp_obj = json_object["restrictions"];
			for (auto restriction = temp_obj.begin(); restriction != temp_obj.end(); ++restriction)
			{
				output->restrictions.insert(std::pair<std::string, std::string>(restriction.key(), restriction.value()));
			}
		}

		step++;

		output->track_number = json_object["track_number"];
		step++;
		output->uri = json_object["uri"];
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		fprintf(stderr, "\033[31mspotify-api.cpp: object_from_json(): Error: Failed to convert json data to track at step %u.\nJson string: %s\nNow exiting.\n\033[39m", step, json::json::parse(json_string).dump(1, '\t').c_str());
		exit(1);
	}
}

}