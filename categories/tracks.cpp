#include <iostream>
#include "tracks.hpp"

#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{

track_t * Track_API::object_from_json(const std::string &json_string)
{
	track_t *output;
	int step = 1;
	try
	{
		json::json json_object = json::json::parse(json_string);
		if (json_object.contains("item"))
		{
			json_object = json_object["item"];
		}

		// not sure whether this check is needed
		if (json_object.contains("album"))
		{
			output->album = Album_API::object_from_json(json_object["album"].dump());
		}

		json::json temp = json_object["artists"];
		for (auto artist = temp.begin(); artist != temp.end(); ++artist)
		{
			output->artists.push_back(Artist_API::object_from_json(artist.value().dump()));
		}
		output->artists.shrink_to_fit();

		step++;

		temp = json_object["available_markets"];
		for (auto market = temp.begin(); market != temp.end(); ++market)
		{
			output->available_markets.push_back(market.value());
		}
		output->available_markets.shrink_to_fit();

		step++;

		output->disc_number = json_object["disc_number"];
		step++;
		output->duration_ms = json_object["duration_ms"];
		step++;

		temp = json_object.value("external_ids", json::json::object());
		for (auto ext_id = temp.begin(); ext_id != temp.end(); ++ext_id)
		{
			output->external_ids.emplace(ext_id.key(), ext_id.value());
		}

		step++;

		temp = json_object.value("external_urls", json::json::object());
		for (auto ext_url = temp.begin(); ext_url != temp.end(); ++ext_url)
		{
			output->external_urls.emplace(ext_url.key(), ext_url.value());
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
		output->is_playable = json_object.value("is_playable", false);

		step++;

		if (json_object.contains("linked_from"))
		{
			// There should be a check to make sure that we cant loop infinitely here
			output->linked_from.emplace(object_from_json(json_object["linked_from"].dump()));
		}
		else
		{
			output->linked_from.reset();
		}

		step++;

		output->name = json_object["name"];
		step++;
		output->popularity = json_object.value("popularity", 0);
		step++;
		output->preview_url = json_object.value("preview_url", "");
		step++;

		temp = json_object.value("restrictions", json::json::object());
		for (auto restriction = temp.begin(); restriction != temp.end(); ++restriction)
		{
			output->restrictions.emplace(restriction.key(), restriction.value());
		}

		step++;

		output->track_number = json_object["track_number"];
		step++;
		output->uri = json_object["uri"];
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		fprintf(stderr, "\033[31mspotify-api.cpp: object_from_json(): Error: Failed to convert json data to track at step %u.\nJson string: %s\n\n\033[39m", step, json::json::parse(json_string).dump(1, '\t').c_str());
		return new track_t;
	}
	return output;
}

track_t * Track_API::get_track(const std::string &track_id, const std::string &market)
{
	std::string url = API_PREFIX "/tracks/";
	url += truncate_id(track_id);
	auto response = http::get(url.c_str(), "market=" + market, this->access_token);

	if (response.code != 200) return new track_t;

	return object_from_json(response.body);
}

std::vector<track_t *> Track_API::get_tracks(const std::vector<std::string> &track_ids, const std::string &market)
{
	std::ostringstream query_data;
	query_data << "ids=";

	for (size_t i = 0; i < track_ids.size(); ++i)
	{
		query_data << truncate_id(track_ids[i]);

		if (i < (track_ids.size() - 1))
		{
			query_data << "\%2C";
		}
	}

	query_data << "&market" << market;

	auto response = http::get(API_PREFIX "/tracks", query_data.str(), this->access_token);

	std::vector<track_t *> tracks;
	if (response.code != 200) return tracks;

	json::json tracks_array = json::json::parse(response.body)["tracks"];
	for (auto track = tracks_array.begin(); track != tracks_array.end(); ++track)
	{
		tracks.push_back(object_from_json(track.value().dump()));
	}
	return tracks;
}

page_t<track_t *> Track_API::get_saved_tracks(const std::string &market, uint8_t limit, unsigned int offset)
{
	std::ostringstream query_data;
	query_data << "market=" << market << "&limit=" << limit << "&offset=" << offset;
	auto response = http::get(API_PREFIX "/me/tracks", query_data.str(), this->access_token);

	page_t<track_t *> tracks_page;
	if (response.code != 200) return tracks_page;
	
	return *page_t<track_t *>::object_from_json(response.body, object_from_json);
}

void Track_API::save_tracks(const std::vector<std::string> &track_ids)
{
	json::json json_ids = {
		{"ids", truncate_ids(track_ids, 50)}
	};
	(void) http::request(API_PREFIX "/me/tracks", http::REQUEST_METHOD::PUT, json_ids.dump(), this->access_token, true);
}

void Track_API::remove_saved_tracks(const std::vector<std::string> &track_ids)
{
	json::json json_ids = {
		{"ids", truncate_ids(track_ids, 50)}
	};
	(void) http::request(API_PREFIX "/me/tracks", http::REQUEST_METHOD::DELETE, json_ids.dump(), this->access_token, true);
}

std::vector<bool> Track_API::check_saved_tracks(const std::vector<std::string> &track_ids)
{
	std::ostringstream query_data;
	query_data << "ids=";

	std::vector<std::string> truncated_ids = truncate_ids(track_ids, 50);
	for (size_t i = 0; i < truncated_ids.size(); ++i) {
		query_data << truncated_ids[i];
		if (i < (truncated_ids.size() - 1))
		{
			query_data << "\%2C";
		}
	}
	auto response = http::get(API_PREFIX "/me/tracks/contains", query_data.str(), this->access_token);
	
	std::vector<bool> checked_tracks;
	if (response.code != 200) return checked_tracks;

	json::json json_response = json::json::parse(response.body);
	for (auto track = json_response.begin(); track != json_response.end(); ++track)
	{
		checked_tracks.push_back(track.value().get<bool>());
	}
	return checked_tracks;
}

audio_features_t * parse_audio_features(const std::string &json_string)
{
	audio_features_t * output = new audio_features_t();
	json::json json_object = json::json::parse(json_string);
	try
	{
		output->acousticness = json_object["acousticness"];
		output->analysis_url = json_object["analysis_url"];
		output->danceability = json_object["danceability"];
		output->duration_ms = json_object["duration_ms"];
		output->energy = json_object["energy"];
		output->id = json_object["id"];
		output->instrumentalness = json_object["instrumentalness"];
		output->key = json_object["key"];
		output->liveness = json_object["liveness"];
		output->loudness = json_object["loudness"];
		output->mode = json_object["mode"];
		output->speechiness = json_object["speechiness"];
		output->tempo = json_object["tempo"];
		output->time_signature = json_object["time_signature"];
		output->track_href = json_object["track_href"];
		output->uri = json_object["uri"];
		output->valence = json_object["valence"];
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return output;
}



Track_API::recommendation_seed_t parse_recommendation_seed(const std::string &json_string)
{

}

template <typename T>
T clamp(T value, T min, T max)
{
	return std::max(min, std::min(value, max));
}

std::string filter_to_query_string(Track_API::recommendation_filter_t filter)
{
	std::ostringstream query_string;
	query_string << "limit=" << clamp<unsigned int>(filter.limit, 1, 100);
	query_string << "&market=" << filter.market;

	query_string << "&seed_artists=";
	size_t artist_seed_length = std::min(filter.seed_artists.size(), 5UL);
	for (size_t i = 0; i < artist_seed_length; ++i)
	{
		query_string << filter.seed_artists[i];
		if (i < artist_seed_length - 1) {
			query_string << http::url_encode(",");
		}
	}

	query_string << "&seed_genres=";
	size_t genre_seed_length = std::min(filter.seed_genres.size(), 5UL);
	for (size_t i = 0; i < genre_seed_length; ++i)
	{
		query_string << filter.seed_genres[i];
		if (i < genre_seed_length - 1) {
			query_string << http::url_encode(",");
		}
	}

	query_string << "&seed_tracks=";
	size_t track_seed_length = std::min(filter.seed_tracks.size(), 5UL);
	for (size_t i = 0; i < track_seed_length; ++i)
	{
		query_string << filter.seed_tracks[i];
		if (i < track_seed_length - 1) {
			query_string << http::url_encode(",");
		}
	}

	// I hope there is a better way to do this.
	// But until I find a better way, I will just be sitting here,
	// mindlessly clacking away at my keyboard into the endless void...

	if (!std::isnan(filter.min_acousticness)) query_string << "&min_acousticness=" << clamp<double>(filter.min_acousticness, 0, 1);
	if (!std::isnan(filter.max_acousticness)) query_string << "&max_acousticness=" << clamp<double>(filter.max_acousticness, 0, 1);
	if (!std::isnan(filter.target_acousticness)) query_string << "&target_acousticness=" << clamp<double>(filter.target_acousticness, 0, 1);

	if (!std::isnan(filter.min_danceability)) query_string << "&min_danceability=" << clamp<double>(filter.min_danceability, 0, 1);
	if (!std::isnan(filter.max_danceability)) query_string << "&max_danceability=" << clamp<double>(filter.max_danceability, 0, 1);
	if (!std::isnan(filter.target_danceability)) query_string << "&target_danceability=" << clamp<double>(filter.target_danceability, 0, 1);

	if (filter.min_duration_ms != -1) query_string << "&min_duration_ms=" << std::max(1, filter.min_duration_ms);
	if (filter.max_duration_ms != -1) query_string << "&max_duration_ms=" << std::max(1, filter.max_duration_ms);
	if (filter.target_duration_ms != -1) query_string << "&target_duration_ms=" << std::max(1, filter.target_duration_ms);

	if (!std::isnan(filter.min_energy)) query_string << "&min_energy=" << clamp<double>(filter.min_energy, 0, 1);
	if (!std::isnan(filter.max_energy)) query_string << "&max_energy=" << clamp<double>(filter.max_energy, 0, 1);
	if (!std::isnan(filter.target_energy)) query_string << "&target_energy=" << clamp<double>(filter.target_energy, 0, 1);

	if (!std::isnan(filter.min_instrumentalness)) query_string << "&min_instrumentalness=" << clamp<double>(filter.min_instrumentalness, 0, 1);
	if (!std::isnan(filter.max_instrumentalness)) query_string << "&max_instrumentalness=" << clamp<double>(filter.max_instrumentalness, 0, 1);
	if (!std::isnan(filter.target_instrumentalness)) query_string << "&target_instrumentalness=" << clamp<double>(filter.target_instrumentalness, 0, 1);

	if (filter.min_key != -1) query_string << "&min_key=" << clamp<int>(filter.min_key, 0, 11);
	if (filter.max_key != -1) query_string << "&max_key=" << clamp<int>(filter.max_key, 0, 11);
	if (filter.target_key != -1) query_string << "&target_key=" << clamp<int>(filter.target_key, 0, 11);

	if (!std::isnan(filter.min_liveness)) query_string << "&min_liveness=" << clamp<double>(filter.min_liveness, 0, 1);
	if (!std::isnan(filter.max_liveness)) query_string << "&max_liveness=" << clamp<double>(filter.max_liveness, 0, 1);
	if (!std::isnan(filter.target_liveness)) query_string << "&target_liveness=" << clamp<double>(filter.target_liveness, 0, 1);

	if (!std::isnan(filter.min_loudness)) query_string << "&min_loudness=" << filter.min_loudness;
	if (!std::isnan(filter.max_loudness)) query_string << "&max_loudness=" << filter.max_loudness;
	if (!std::isnan(filter.target_loudness)) query_string << "&target_loudness=" << filter.target_loudness;

	if (filter.min_mode != -1) query_string << "&min_mode=" << clamp<int>(filter.min_mode, 0, 1);
	if (filter.max_mode != -1) query_string << "&max_mode=" << clamp<int>(filter.max_mode, 0, 1);
	if (filter.target_mode != -1) query_string << "&target_mode=" << clamp<int>(filter.target_mode, 0, 1);

	if (filter.min_popularity != -1) query_string << "&min_popularity=" << clamp<int>(filter.min_popularity, 0, 100);
	if (filter.max_popularity != -1) query_string << "&max_popularity=" << clamp<int>(filter.max_popularity, 0, 100);
	if (filter.target_popularity != -1) query_string << "&target_popularity=" << clamp<int>(filter.target_popularity, 0, 100);

	if (!std::isnan(filter.min_speechiness)) query_string << "&min_speechiness=" << clamp<double>(filter.min_speechiness, 0, 1);
	if (!std::isnan(filter.max_speechiness)) query_string << "&max_speechiness=" << clamp<double>(filter.max_speechiness, 0, 1);
	if (!std::isnan(filter.target_speechiness)) query_string << "&target_speechiness=" << clamp<double>(filter.target_speechiness, 0, 1);

	if (!std::isnan(filter.min_tempo)) query_string << "&min_tempo=" << std::max(0.0, filter.min_tempo);
	if (!std::isnan(filter.max_tempo)) query_string << "&max_tempo=" << std::max(0.0, filter.max_tempo);
	if (!std::isnan(filter.target_tempo)) query_string << "&target_tempo=" << std::max(0.0, filter.target_tempo);

	if (filter.min_time_signature != -1) query_string << "&min_time_signature=" << clamp<int>(filter.min_time_signature, 1, 11);
	if (filter.max_time_signature != -1) query_string << "&max_time_signature=" << clamp<int>(filter.max_time_signature, 1, 11);
	if (filter.target_time_signature != -1) query_string << "&target_time_signature=" << clamp<int>(filter.target_time_signature, 1, 11);

	if (!std::isnan(filter.min_valence)) query_string << "&min_valence=" << clamp<double>(filter.min_valence, 0, 1);
	if (!std::isnan(filter.max_valence)) query_string << "&max_valence=" << clamp<double>(filter.max_valence, 0, 1);
	if (!std::isnan(filter.target_valence)) query_string << "&target_valence=" << clamp<double>(filter.target_valence, 0, 1);
	return query_string.str();
}

audio_features_t * Track_API::get_audio_features_for_track(const std::string &track_id)
{
	std::string url = API_PREFIX "/audio-features/";
	url += truncate_id(track_id);

	auto response = http::get(url.c_str(), std::string(), this->access_token);

	if (response.code != 200) return new audio_features_t();
	return parse_audio_features(response.body);
}

std::vector<audio_features_t *> Track_API::get_audio_features_for_tracks(const std::vector<std::string> &track_ids)
{
	std::ostringstream query_data;
	query_data << "?ids=";
	std::vector<std::string> truncated_ids = truncate_ids(track_ids, 100);

	for (size_t i = 0; i < truncated_ids.size(); ++i)
	{
		query_data << truncated_ids[i];
		if (i < truncated_ids.size() - 1)
		{
			query_data << http::url_encode(",");
		}
	}
	auto response = http::get(API_PREFIX "/audio-features", query_data.str(), this->access_token);
	std::vector<audio_features_t *> features;
	if (response.code != 200) return features;

	json::json features_json = json::json::parse(response.body)["audio_features"];
	for (auto feat = features_json.begin(); feat != features_json.end(); ++feat)
	{
		features.push_back(parse_audio_features(feat.value().dump()));
	}
	return features;
}

audio_analysis_t * Track_API::get_audio_analysis_for_track(const std::string &track_id)
{
	std::ostringstream url;
	url << API_PREFIX << "/audio-analysis/" << truncate_id(track_id);
	auto response = http::get(url.str().c_str(), std::string(), this->access_token);
	if (response.code != 200) return new audio_analysis_t();
	return parse_audio_analysis(response.body);
}

std::pair<std::vector<Track_API::recommendation_seed_t>, std::vector<track_t *>> * Track_API::get_recommendations(const recommendation_filter_t &filter)
{
	std::string query_data = filter_to_query_string(filter);
	auto response = http::get(API_PREFIX "/recommendations", query_data, this->access_token);
	auto retval = new std::pair<std::vector<recommendation_seed_t>, std::vector<track_t *>>();
	if (response.code != 200) return retval;
	json::json response_json = json::json::parse(response.body);

	for (auto seed = response_json["seeds"].begin(); seed != response_json["seeds"].end(); ++seed)
	{
		retval->first.push_back(parse_recommendation_seed(seed.value().dump()));
	}

	for (auto track = response_json["tracks"].begin(); track != response_json["tracks"].end(); ++track)
	{
		retval->second.push_back(object_from_json(track.value().dump()));
	}

	return retval;
}

}