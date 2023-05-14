#include <iostream>
#include "artists.hpp"

namespace json = nlohmann;

namespace spotify_api
{

void Artist_API::object_from_json(const std::string &json_string, artist_t *output)
{
	try
	{
		/* TODO: maybe replace with a custom function to parse an array.
			* Array parser can take the output location and array data as arguments
			* as well as a callback function to run on each array element.
			* So in theory this would work similar to the map() or forEach() array method in JS.
			*/
		json::json json_object = json::json::parse(json_string);
		auto url_obj = json_object["external_urls"];
		for (auto url = url_obj.begin(); url != url_obj.end(); ++url)
		{
			output->external_urls.insert(std::pair<std::string, std::string>(url.key(), url.value()));
		}

		if (json_object.contains("followers"))
		{
			output->followers.href = json_object["followers"]["href"];
			output->followers.total = json_object["followers"]["total"];
		}

		if (json_object.contains("genres"))
		{
			auto genre_array = json_object["genres"];
			output->genres.reserve(genre_array.size());
			for (auto genre = genre_array.begin(); genre != genre_array.end(); ++genre)
			{
				output->genres.push_back(genre.value());
			}
		}
		output->genres.shrink_to_fit();

		output->href = json_object["href"];
		output->id = json_object["id"];

		if (json_object.contains("images"))
		{
			auto image_array = json_object["images"];
			output->images.reserve(image_array.size());
			for (auto image = image_array.begin(); image != image_array.end(); ++image)
			{
				image_t temp;
				temp.url = image.value()["url"];
				temp.width = image.value()["width"];
				temp.height = image.value()["height"];
				output->images.push_back(temp);
			}
		}
		output->images.shrink_to_fit();

		output->name = json_object["name"];
		if (json_object.contains("popularity"))
			output->popularity = json_object["popularity"];
		output->uri = json_object["uri"];
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		fprintf(stderr, "\033[31mspotify-api/artist.cpp: object_from_json(): Error: Failed to convert json data to artist.\nJson string: %s\nNow exiting.\n\033[39m", json_string.c_str());
		exit(1);
	}
}

} // namespace spotify_api
