#include <iostream>
#include <unistd.h>
#include "playlist.hpp"

#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{

std::vector<playlist_t *> Playlist_API::get_my_playlists(int limit)
{
	std::vector<playlist_t *> playlists;
	
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
			playlist_t * temp = new playlist_t;
			std::string item = json_res["items"][i].dump();
			// printf("Batch Item: %s\n\n", item.c_str());
			object_from_json(item, *temp);
			playlists.push_back(temp);
		}
		usleep(1000 * 100); // Sleep for 100,000 microseconds or 100 milliseconds
	}
	playlists.shrink_to_fit();
	return playlists;
}

    void object_from_json(const std::string &json_string, playlist_t &output)
    {
        int step = 1;
        try
        {
            json::json json_object = json::json::parse(json_string);
            step++;
            output.collaborative = json_object["collaborative"];
            step++;
            if (json_object.contains("description") && !json_object["description"].is_null())
            {
                output.description = json_object["description"];
            }
            step++;
            
            json::json temp_obj;

            for (auto item = json_object["external_urls"].begin(); item != json_object["external_urls"].end(); ++item)
            {
                output.external_urls.emplace(item.key(), item.value().get<std::string>());
            }
            step++;
            
            if (json_object.contains("followers"))
            {
                output.followers.href = json_object["followers"]["href"];
                output.followers.total = json_object["followers"]["total"];
            }
            step++;
            
            output.href = json_object["href"];
            step++;
            output.id = json_object["id"];
            step++;

            temp_obj = json_object["images"];
            output.images.reserve(temp_obj.size());
            for (auto img = temp_obj.begin(); img != temp_obj.end(); ++img)
            {
                image_t temp;
                temp.height = !img.value()["height"].is_null() ? img.value()["height"].get<int>() : 0;
                temp.width = !img.value()["width"].is_null() ? img.value()["width"].get<int>() : 0;
                temp.url = img.value()["url"];
                output.images.push_back(temp);
            }
            output.images.shrink_to_fit();
            step++;

            output.is_public = json_object["public"];
            step++;
            output.name = json_object["name"];
            step++;
            
            json::json json_owner = json_object["owner"];
            
            output.owner.display_name = json_owner["display_name"];
            step++;
            output.owner.href = json_owner["href"];
            step++;
            output.owner.id = json_owner["id"];
            step++;
            output.owner.uri = json_owner["uri"];
            step++;
            if (json_owner.contains("followers"))
            {
                output.owner.followers.href = json_owner["followers"]["href"];
                step++;
                output.owner.followers.total = json_owner["followers"]["total"];
                step++;
            } else {
                step += 2;
            }
            
            step++;
            for (auto item = json_owner["external_urls"].begin(); item != json_owner["external_urls"].end(); ++item)
            {
                output.owner.external_urls.emplace(item.key(), item.value().get<std::string>());
            }
            step++;
            
            output.snapshot_id = json_object["snapshot_id"];
            step++;
            output.uri = json_object["uri"];
            step++;
            
            if (json_object.contains("tracks"))
            {
                json::json json_tracks = json_object["tracks"];
                output.tracks.href = json_tracks["href"];
                output.tracks.total = json_tracks["total"];
                
                if (json_tracks.contains("limit"))
                    output.tracks.limit = json_tracks["limit"];
                if (json_tracks.contains("offset"))
                    output.tracks.offset = json_tracks["offset"];
                
                if (!json_tracks["next"].is_null())
                    output.tracks.next = json_tracks["next"];
                if (!json_tracks["previous"].is_null())
                    output.tracks.previous = json_tracks["previous"];
                
                if (json_tracks.contains("items"))
                {
                    json_tracks = json_tracks["items"];
                    output.tracks.items.reserve(json_tracks.size());
                    for (auto track = json_tracks.begin(); track != json_tracks.end(); ++track)
                    {
                        spotify_api::track_t *temp = new spotify_api::track_t;
                        Track_API::object_from_json(track.value().dump(), temp);
                        output.tracks.items.push_back(temp);
                    }
                }
                output.tracks.items.shrink_to_fit();
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            
            fprintf(stderr, "\033[31mspotify-api.cpp: object_from_json(): Error: Failed to convert json data to playlist at step %i.\nJson string: %s\nNow exiting.\n\033[39m", step, json::json::parse(json_string).dump(1, '\t').c_str());
            exit(1);
        }
    }
} // namespace spotify_api