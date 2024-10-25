#include "endpoints/search.hpp"

#include "curl-util.hpp"

#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{

inline std::string search_type_to_string(search_type s)
{
	std::string output;
	if (s.album) output += "album,";
	if (s.artist) output += "artist,";
	if (s.track) output += "track,";
	if (s.playlist) output += "playlist,";
	if (s.show) output += "show,";
	if (s.episode) output += "episode,";

	if (output.ends_with(",")) output = output.substr(0, output.length() - 1);
	return output;
}



Search_API::Search_API(std::string access_token): access_token(access_token) {}

std::unique_ptr<search_result> Search_API::search(search_type search_for_types, const std::string &q)
{
	std::string query = "q=" + q + "&type=" + search_type_to_string(search_for_types);
	auto response = http::get(API_PREFIX "/search", query, this->access_token);

	if (response.code != 200) {
		std::cerr << "failed to search for track: " << response.code << '\n';
		return std::unique_ptr<search_result>(nullptr);
	}

	json::json result = json::json::parse(response.body);
	return search_result::from_json(result);
}

} // namespace spotify_api