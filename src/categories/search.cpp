#include "categories/search.hpp"

namespace json = nlohmann;

namespace spotify_api
{

search_type::search_type(
	bool album,
	bool artist,
	bool playlist,
	bool track,
	bool show,
	bool episode) :
	album(album),
	artist(artist),
	playlist(playlist),
	track(track),
	show(show),
	episode(episode)
{}

std::unique_ptr<search_result> search_result::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<search_result> search_result::from_json(const json::json &json_obj)
{
	auto result = std::make_unique<search_result>();

	result->albums.reset();
	result->artists.reset();
	result->tracks.reset();
	result->playlists.reset();
	result->shows.reset();
	result->episodes.reset();

	if (json_obj.contains("albums"))
		result->albums = page_t<std::shared_ptr<album_t>>::from_json(json_obj["albums"]);
	
	if (json_obj.contains("artists"))
		result->artists = page_t<std::shared_ptr<artist_t>>::from_json(json_obj["artists"]);

	if (json_obj.contains("tracks"))
		result->tracks = page_t<std::shared_ptr<track_t>>::from_json(json_obj["tracks"]);

	if (json_obj.contains("playlists"))
		result->playlists = page_t<std::shared_ptr<playlist_t>>::from_json(json_obj["playlists"]);

	if (json_obj.contains("shows"))
		result->shows = page_t<std::shared_ptr<show_t>>::from_json(json_obj["shows"]);

	if (json_obj.contains("episodes"))
		result->episodes = page_t<std::shared_ptr<episode_t>>::from_json(json_obj["episodes"]);

	return result;
}

}