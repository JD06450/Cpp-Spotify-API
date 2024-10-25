#include <iostream>
#include <unordered_set>
#include "categories/artists.hpp"
#include "endpoints/artists.hpp"

#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{

const std::string artist_t::type = item_type::ARTIST;

std::unique_ptr<artist_t> artist_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<artist_t> artist_t::from_json(const json::json &json_obj)
{
	auto output = std::make_unique<artist_t>();
	try
	{
		auto url_obj = json_obj["external_urls"];
		for (auto url = url_obj.begin(); url != url_obj.end(); ++url)
		{
			output->external_urls.emplace(url.key(), url.value());
		}

		if (json_obj.contains("followers"))
		{
			output->followers.href = json_obj["followers"].value("href", "");
			output->followers.total = json_obj["followers"]["total"];
		}

		auto genre_array = json_obj.value("genres", json::json::array());
		output->genres.reserve(genre_array.size());
		for (auto genre = genre_array.begin(); genre != genre_array.end(); ++genre)
		{
			output->genres.push_back(genre.value());
		}
		output->genres.shrink_to_fit();

		output->href = json_obj["href"];
		output->id = json_obj["id"];

		auto image_array = json_obj.value("images", json::json::array());
		for (auto image = image_array.begin(); image != image_array.end(); ++image)
		{
			output->images.push_back(image_t::from_json(image.value().dump()));
		}
		output->images.shrink_to_fit();

		output->name = json_obj["name"];
		output->popularity = json_obj.value("popularity", 0);
		output->uri = json_obj["uri"];
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		fprintf(stderr, "\033[31mspotify-api/artist.cpp: object_from_json(): Error: Failed to convert json data to artist.\nJson string: %s\n\n\033[39m", json_obj.dump().c_str());
		return std::unique_ptr<artist_t>(nullptr);
	}
	return output;
}

std::unique_ptr<artist_t> Artist_API::get_artist(const std::string &artist_id)
{
	const std::string url = API_PREFIX "/artists/" + artist_id;
	auto response = http::get(url.c_str(), std::string(), this->access_token);

	if (response.code != 200) return std::unique_ptr<artist_t>(nullptr);
	return artist_t::from_json(response.body);
}

std::vector<std::unique_ptr<artist_t>> Artist_API::get_artists(const std::vector<std::string> &artist_ids)
{
	std::vector<std::string> truncated_ids = truncate_spotify_uris(artist_ids, 50);
	std::ostringstream query_data;
	for (size_t i = 0; i < truncated_ids.size(); i++)
	{
		query_data << truncated_ids[i];
		if (i < truncated_ids.size() - 1)
		{
			query_data << "\%2C"; // url-encoded comma
		}
	}

	auto response = http::get(API_PREFIX "/artists", query_data.str(), this->access_token);
	std::vector<std::unique_ptr<artist_t>> artists;
	if (response.code != 200) return artists;

	json::json artists_array = json::json::parse(response.body)["artists"];
	for (auto artist = artists_array.begin(); artist != artists_array.end(); ++artist)
	{
		artists.push_back(artist_t::from_json(artist.value().dump()));
	}
	return artists;
}


page_t<std::unique_ptr<album_t>> Artist_API::get_albums_from_artist(const std::string &artist_id, std::vector<std::string> &include_groups, std::string &market, uint8_t limit, uint32_t offset)
{
	const std::unordered_set<std::string> album_types = {"single", "compilation", "appears_on", "album"};
	std::unordered_set<std::string> included_types = {};
	std::ostringstream url, query_data;
	url << API_PREFIX << "/artists/" << truncate_spotify_uri(artist_id) << "/albums";

	// Filter the included groups to only include allowed values and remove duplicates.
	std::string groups_string;
	for (size_t i = 0; i < include_groups.size(); ++i)
	{
		if (!album_types.count(include_groups[i]) || included_types.count(include_groups[i])) continue;
		groups_string += include_groups[i] + "\%2C";
		included_types.emplace(groups_string);
	}
	// If the user specified any groups, then remove the trailing comma added by the above for-loop
	if (groups_string != "") {
		groups_string = groups_string.substr(0, groups_string.length() - 3);
	}
	if (limit > 50) limit = 50;

	query_data << "include_groups=" << groups_string << "&limit=" << limit << "&offset=" << offset << "&market=" << market.substr(0, 2);
	auto response = http::get(url.str().c_str(), std::string(), this->access_token);

	page_t<std::unique_ptr<album_t>> albums;
	if (response.code != 200) return albums;
	json::json albums_page = json::json::parse(response.body);

	return decltype(albums)::from_json(albums_page.dump());
}

std::vector<std::unique_ptr<track_t>> Artist_API::get_artist_top_tracks(const std::string &artist_id, const std::string &market)
{
	std::ostringstream url;
	url << API_PREFIX << "/artists/" << truncate_spotify_uri(artist_id) << "/tracks";
	std::string query_data = "market=" + market;
	auto response = http::get(url.str().c_str(), query_data, this->access_token);

	std::vector<std::unique_ptr<track_t>> tracks;
	if (response.code != 200) return tracks;

	json::json tracks_array = json::json::parse(response.body)["tracks"];
	for (auto track = tracks_array.begin(); track != tracks_array.end(); ++track)
	{
		tracks.push_back(track_t::from_json(track.value().dump()));
	}
	return tracks;
}

std::vector<std::unique_ptr<artist_t>> Artist_API::get_related_artists(const std::string &artist_id)
{
	std::ostringstream url;
	url << API_PREFIX << "/artists/" << truncate_spotify_uri(artist_id) << "/related-artists";

	auto response = http::get(url.str().c_str(), std::string(), this->access_token);
	std::vector<std::unique_ptr<artist_t>> related_artists;
	if (response.code != 200) return related_artists;
	
	json::json artists_array = json::json::parse(response.body)["artists"];
	for (auto artist = artists_array.begin(); artist != artists_array.end(); ++artist)
	{
		related_artists.push_back(artist_t::from_json(artist.value().dump()));
	}
	return related_artists;
}

} // namespace spotify_api
