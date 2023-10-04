#include <iostream>
#include "albums.hpp"

#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{

std::unique_ptr<album_t> Album_API::object_from_json(const std::string &json_string)
{
	std::unique_ptr<album_t> album = std::make_unique<album_t>();
	// using a single "temp" variable to save on memory space.
	// why initialize a bunch of big stack variables when you could just use 1

	json::json temp;
	try
	{
		json::json json_object = json::json::parse(json_string);

		album->album_type = json_object["album_type"];
		album->total_tracks = json_object["total_tracks"];

		temp = json_object["available_markets"];
		album->available_markets.reserve(temp.size());
		for (auto market = temp.begin(); market != temp.end(); ++market)
		{
			album->available_markets.push_back(market.value());
		}
		album->available_markets.shrink_to_fit();

		temp = json_object["external_urls"];
		for (auto ext_url = temp.begin(); ext_url != temp.end(); ++ext_url)
		{
			album->external_urls.emplace(ext_url.key(), ext_url.value().get<std::string>());
		}

		album->href = json_object["href"];
		album->id = json_object["id"];

		temp = json_object["images"];
		album->images.reserve(temp.size());
		for (auto image = temp.begin(); image != temp.end(); ++image)
		{
			image_t temp_image;
			spotify_api::object_from_json(image.value().dump(), &temp_image);
			album->images.push_back(temp_image);
		}
		album->images.shrink_to_fit();

		album->name = json_object["name"];
		album->release_date = json_object["release_date"];
		album->release_date_precision = json_object["release_date_precision"];

		
		temp = json_object.value("restrictions", json::json::object());
		for (auto item = temp.begin(); item != temp.end(); ++item)
		{
			album->restrictions.emplace(item.key(), item.value().get<std::string>());
		}

		temp = json_object.value("copyrights", json::json::array());
		for (auto cp = temp.begin(); cp != temp.end(); ++cp)
		{
			album->copyrights.push_back((copyright_t) {cp.value()["text"].get<std::string>(), cp.value()["type"].get<std::string>()});
		}
		album->copyrights.shrink_to_fit();

		album->uri = json_object["uri"];

		temp = json_object.value("external_ids", json::json::object());
		for (auto id = temp.begin(); id != temp.end(); ++id)
		{
			album->restrictions.emplace(id.key(), id.value().get<std::string>());
		}

		temp = json_object["genres"];
		for (auto genre = temp.begin(); genre != temp.end(); ++genre)
		{
			album->genres.push_back(genre.value());
		}

		album->popularity = json_object.value("popularity", 0);

		album->label = json_object.value("artists", "");

		temp = json_object.value("artists", json::json::array());
		for (auto artist = temp.begin(); artist != temp.end(); ++artist)
		{
			album->artists.push_back(Artist_API::object_from_json(artist.value().dump()));
		}

		album->tracks = json_object.contains("tracks") ? *page_t<track_t *>::object_from_json(json_object["tracks"], Track_API::object_from_json) : page_t<track_t *>();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		fprintf(stderr, "\033[31mspotify-api/categories/albums.cpp: object_from_json(): Error: Failed to convert json data to album.\nJson string: %s\n\n\033[39m", json_string.c_str());
		
		// TODO: idk if this try-catch is necessary, but i guess it would be nice to have
		// if someone gives us junk data. maybe return as much useful data as we can collect
		// instead of returning an empty object? ¯\_(ツ)_/¯
		return std::make_unique<album_t>();
	}
	return album;
}

std::unique_ptr<album_t> Album_API::get_album(const std::string &album_id)
{
	std::string url = API_PREFIX "/albums/";
	url += album_id;

	http::api_response response = http::get(url.c_str(), std::string(""), this->access_token);
	if (response.code == 200)
	{
		return object_from_json(response.body);
	}
}

std::vector<std::unique_ptr<album_t>> Album_API::get_albums(const std::vector<std::string> &album_ids)
{
	std::vector<std::unique_ptr<album_t>> albums;
	albums.reserve(album_ids.size());
	std::string query_string = "";
	size_t num_batches = ceil(album_ids.size() / 20);

	std::vector<std::string> response_batches;

	for (size_t batch = 0; batch < num_batches; batch++)
	{
		size_t batch_size = batch * 20 + 20 < album_ids.size() ? 20 : album_ids.size() - (batch * 20 + 20);
		query_string = "ids=";
		for (size_t i = batch * 20; i < (batch * 20) + batch_size; i++)
		{
			query_string += album_ids.at(i) + ",";
		}
		query_string = std::regex_replace(query_string, std::regex(",+$"), "");
		http::api_response batch_response = http::get(API_PREFIX "/albums", query_string, this->access_token);
		if (batch_response.code == 200)
		{
			response_batches.push_back(batch_response.body);
		}
	}

	for (auto page : response_batches)
	{
		json::json page_json = json::json::parse(page)["albums"];
		for (auto album = page_json.begin(); album != page_json.end(); ++album)
		{
			albums.push_back(object_from_json(album.value().dump()));
		}
	}
	albums.shrink_to_fit();
	return albums;
}

page_t<std::unique_ptr<track_t>> Album_API::get_album_tracks(const std::string &album_id, uint32_t limit, uint32_t offset, const std::string &market)
{
	std::string trunc_album_id = truncate_id(album_id);

	std::ostringstream url, query_data;
	url << API_PREFIX << "/albums/" << trunc_album_id << "/tracks";
	// TODO: batches
	if (limit > 50) limit = 50;
	query_data << "limit=" << limit << "&offset=" << offset << "&market=" << market;

	auto response = http::get(url.str().c_str(), query_data.str(), this->access_token);

	json::json json_object = json::json::parse(response.body);

	page_t<std::unique_ptr<track_t>> retval;

	if (response.code != 200) {
		return retval;
	}

	retval.href = json_object["href"];
	retval.limit = json_object["limit"];
	retval.next = json_object.value("next", "");
	retval.previous = json_object.value("previous", "");
	retval.offset = json_object["offset"];
	retval.total = json_object["total"];

	for (auto track = json_object["items"].begin(); track != json_object["items"].end(); ++track)
	{
		retval.items.push_back(Track_API::object_from_json(track.value().dump()));
	}
	retval.items.shrink_to_fit();

	return retval;
}

page_t<std::unique_ptr<album_t>> Album_API::get_users_albums(uint32_t limit, uint32_t offset, const std::string &market)
{
	std::ostringstream query_data;
	query_data << "limit=" << limit << "&offset=" << offset << "&market=" << market;
	auto response = http::get(API_PREFIX "/me/albums", query_data.str(), this->access_token);

	if (response.code != 200) {
		return page_t<std::unique_ptr<album_t>>();
	}
	return std::move(*page_t<std::unique_ptr<album_t>>::object_from_json(response.body, object_from_json));
}

void Album_API::save_albums_for_current_user(const std::vector<std::string> &album_ids)
{
	std::vector<std::string> truncated_ids = truncate_ids(album_ids, 20);

	std::ostringstream query_data;
	query_data << "?ids=";

	for (size_t i = 0; i < truncated_ids.size(); i++)
	{
		query_data << truncated_ids[i];
		if (i < truncated_ids.size() - 1)
		{
			query_data << "\%2C"; // url-encoded comma
		}
	}

	std::string url = API_PREFIX "/me/albums";
	url += query_data.str();
	auto response = http::request(url.c_str(), http::REQUEST_METHOD::PUT, "", this->access_token, true);
}

void Album_API::remove_saved_albums_for_current_user(const std::vector<std::string> &album_ids)
{
	std::vector<std::string> truncated_ids = truncate_ids(album_ids, 20);

	std::ostringstream query_data;
	query_data << "?ids=";

	for (size_t i = 0; i < truncated_ids.size(); i++)
	{
		query_data << truncated_ids[i];
		if (i < truncated_ids.size() - 1)
		{
			query_data << "\%2C"; // url-encoded comma
		}
	}

	std::string url = API_PREFIX "/me/albums";
	url += query_data.str();
	auto response = http::request(url.c_str(), http::REQUEST_METHOD::DELETE, "", this->access_token, true);
}

std::vector<bool> Album_API::check_users_saved_albums(const std::vector<std::string> &album_ids)
{
	std::vector<std::string> truncated_ids = truncate_ids(album_ids, 20);

	std::ostringstream query_data;
	query_data << "ids=";

	for (size_t i = 0; i < truncated_ids.size(); i++)
	{
		query_data << truncated_ids[i];
		if (i < truncated_ids.size() - 1)
		{
			query_data << "\%2C"; // url-encoded comma
		}
	}

	std::string url = API_PREFIX "/me/albums/contains";
	
	auto response = http::get(API_PREFIX "/me/albums/contains", query_data.str(), this->access_token);

	std::vector<bool> contains_vector;

	if (response.code != 200) {
		return contains_vector;
	}

	json::json response_array = json::json::parse(response);

	for (auto el = response_array.begin(); el != response_array.end(); ++el)
	{
		contains_vector.push_back(el.value().get<bool>());
	}

	return contains_vector;
}

page_t<std::unique_ptr<album_t>> Album_API::get_new_releases(uint32_t limit, uint32_t offset, const std::string &country)
{
	std::ostringstream query_data;
	query_data << "limit=" << limit << "&offset=" << offset << "&country=" << country;

	auto response = http::get(API_PREFIX "/browse/new-releases", query_data.str(), this->access_token);

	json::json json_object = json::json::parse(response.body)["albums"];

	page_t<std::unique_ptr<album_t>> new_releases;

	if (response.code != 200) {
		return new_releases;
	}

	new_releases.href = json_object["href"];
	new_releases.limit = json_object["limit"];
	new_releases.next = json_object.value("next", "");
	new_releases.previous = json_object.value("previous", "");
	new_releases.offset = json_object["offset"];
	new_releases.total = json_object["total"];

	for (auto album = json_object["items"].begin(); album != json_object["items"].end(); ++album)
	{
		new_releases.items.push_back(object_from_json(album.value().dump()));
	}
	new_releases.items.shrink_to_fit();

	return new_releases;
}

} // namespace spotify_api