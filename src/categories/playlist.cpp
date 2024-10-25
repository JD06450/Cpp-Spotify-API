#include <iostream>
#include <unistd.h>
#include "categories/playlist.hpp"
#include "endpoints/playlist.hpp"

#include "curl-util.hpp"

#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{

const std::string playlist_t::type = item_type::PLAYLIST;

std::unique_ptr<playlist_t> playlist_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<playlist_t> playlist_t::from_json(const json::json &json_obj)
{
	auto playlist = std::make_unique<playlist_t>();

	int step = 1;
	try
	{
		step++;
		playlist->collaborative = json_obj["collaborative"];
		step++;
		if (json_obj.contains("description") && !json_obj["description"].is_null())
		{
			playlist->description = json_obj["description"];
		}
		step++;
		
		json::json temp_obj;

		for (auto item = json_obj["external_urls"].begin(); item != json_obj["external_urls"].end(); ++item)
		{
			playlist->external_urls.emplace(item.key(), item.value().get<std::string>());
		}
		step++;
		
		if (json_obj.contains("followers"))
		{
			playlist->followers.href = json_obj["followers"]["href"];
			playlist->followers.total = json_obj["followers"]["total"];
		}
		step++;
		
		playlist->href = json_obj["href"];
		step++;
		playlist->id = json_obj["id"];
		step++;

		temp_obj = json_obj["images"];
		playlist->images.reserve(temp_obj.size());
		for (auto img = temp_obj.begin(); img != temp_obj.end(); ++img)
		{
			image_t temp;
			temp.height = !img.value()["height"].is_null() ? img.value()["height"].get<int>() : 0;
			temp.width = !img.value()["width"].is_null() ? img.value()["width"].get<int>() : 0;
			temp.url = img.value()["url"];
			playlist->images.push_back(temp);
		}
		playlist->images.shrink_to_fit();
		step++;

		playlist->is_public = json_obj["public"];
		step++;
		playlist->name = json_obj["name"];
		step++;
		
		json::json json_owner = json_obj["owner"];
		
		playlist->owner.display_name = json_owner["display_name"];
		step++;
		playlist->owner.href = json_owner["href"];
		step++;
		playlist->owner.id = json_owner["id"];
		step++;
		playlist->owner.uri = json_owner["uri"];
		step++;
		if (json_owner.contains("followers"))
		{
			playlist->owner.followers.href = json_owner["followers"]["href"];
			step++;
			playlist->owner.followers.total = json_owner["followers"]["total"];
			step++;
		} else {
			step += 2;
		}
		
		step++;
		for (auto item = json_owner["external_urls"].begin(); item != json_owner["external_urls"].end(); ++item)
		{
			playlist->owner.external_urls.emplace(item.key(), item.value().get<std::string>());
		}
		step++;
		
		playlist->snapshot_id = json_obj["snapshot_id"];
		step++;
		playlist->uri = json_obj["uri"];
		step++;
		
		if (json_obj.contains("tracks")) playlist->tracks = page_t<std::shared_ptr<track_t>>::from_json(json_obj["tracks"]);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		
		fprintf(stderr, "\033[31mspotify-api.cpp: object_from_json(): Error: Failed to convert json data to playlist at step %i.\nJson string: %s\nNow exiting.\n\033[39m", step, json_obj.dump(1, '\t').c_str());
		exit(1);
	}

	return playlist;
}



std::unique_ptr<playlist_t> Playlist_API::get_playlist()
{
	return std::unique_ptr<playlist_t>();
}

std::vector<std::shared_ptr<playlist_t>> Playlist_API::get_my_playlists(int limit)
{
	std::vector<std::shared_ptr<playlist_t>> playlists;
	
	// Sending a preliminary request to get the total number of playlists to return
	// makes the code a bit more readable and easy to work with
	http::api_response temp_res = http::get(API_PREFIX "/me/playlists", "?limit=1", this->access_token);

	int total_playlists = json::json::parse(temp_res.body)["total"];
	if (limit > total_playlists || limit < 1) limit = total_playlists;
	playlists.reserve(limit);

	int batch_size = (limit > 50) ? 50 : limit;

	for (int batch = 0; batch < ceil(limit / batch_size); batch++)
	{
		std::stringstream query;
		query << "?limit=" << batch_size << "&offset" << batch * batch_size;
		http::api_response batch_res = http::get(API_PREFIX "/me/playlists", query.str(), this->access_token);
		printf("Batch Code: %i\n", batch_res.code);
		if (batch_res.code == 429) {
			printf("\033[33mspotify-api.cpp: get_my_playlists(): Warning: Rate limit exceeded.\n\033[39m");
			batch--;
			sleep(1);
			continue;
		}
		// if (batch_res.code == 200) std::cout << batch_res.body << std::endl;
		json::json json_res = json::json::parse(batch_res.body);
		
		for (int i = 0; i < json_res["items"].size(); i++)
		{
			playlists.push_back(playlist_t::from_json(json_res["items"][i]));
		}

		usleep(1000 * 100); // Sleep for 100,000 microseconds or 100 milliseconds
	}
	playlists.shrink_to_fit();
	return playlists;
}

} // namespace spotify_api