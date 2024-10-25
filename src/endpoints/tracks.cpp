#include "endpoints/tracks.hpp"
#include "categories/tracks.hpp"

#include "curl-util.hpp"

#include <iostream>

namespace json = nlohmann;

namespace spotify_api {

std::unique_ptr<track_t> Track_API::get_track(const std::string &track_id, const std::string &market)
{
	std::string url = API_PREFIX "/tracks/";
	url += truncate_spotify_uri(track_id);
	auto response = http::get(url.c_str(), "market=" + market, this->access_token);

	if (response.code != 200) return std::unique_ptr<track_t>(nullptr);

	return track_t::from_json(response.body);
}

std::vector<std::unique_ptr<track_t>> Track_API::get_tracks(const std::vector<std::string> &track_ids, const std::string &market)
{
	std::ostringstream query_data;
	query_data << "ids=";

	for (size_t i = 0; i < track_ids.size(); ++i)
	{
		query_data << truncate_spotify_uri(track_ids[i]);

		if (i < (track_ids.size() - 1)) query_data << "\%2C";
	}

	query_data << "&market" << market;

	auto response = http::get(API_PREFIX "/tracks", query_data.str(), this->access_token);

	std::vector<std::unique_ptr<track_t>> tracks;
	if (response.code != 200) return tracks;

	json::json tracks_array = json::json::parse(response.body)["tracks"];
	for (auto track = tracks_array.begin(); track != tracks_array.end(); ++track)
	{
		tracks.push_back(track_t::from_json(track.value()));
	}
	return tracks;
}

page_t<std::unique_ptr<track_t>> Track_API::get_saved_tracks(const std::string &market, uint8_t limit, unsigned int offset)
{
	std::ostringstream query_data;
	query_data << "market=" << market << "&limit=" << limit << "&offset=" << offset;
	auto response = http::get(API_PREFIX "/me/tracks", query_data.str(), this->access_token);

	page_t<std::unique_ptr<track_t>> tracks_page;
	if (response.code != 200) return tracks_page;
	
	return page_t<std::unique_ptr<track_t>>::from_json(response.body);
}

void Track_API::save_tracks(const std::vector<std::string> &track_ids)
{
	json::json json_ids = {
		{"ids", truncate_spotify_uris(track_ids, 50)}
	};
	(void) http::request(API_PREFIX "/me/tracks", http::REQUEST_METHOD::METHOD_PUT, json_ids.dump(), this->access_token, true);
}

void Track_API::remove_saved_tracks(const std::vector<std::string> &track_ids)
{
	json::json json_ids = {
		{"ids", truncate_spotify_uris(track_ids, 50)}
	};
	(void) http::request(API_PREFIX "/me/tracks", http::REQUEST_METHOD::delete, json_ids.dump(), this->access_token, true);
}

std::vector<bool> Track_API::check_saved_tracks(const std::vector<std::string> &track_ids)
{
	std::ostringstream query_data;
	query_data << "ids=";

	std::vector<std::string> truncated_ids = truncate_spotify_uris(track_ids, 50);
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

std::string filter_to_query_string(recommendation_filter_t filter)
{
	std::ostringstream query_string;
	query_string << "limit=" << std::clamp<u_int>(filter.limit, 1, 100);
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

// These macros save a bunch of typing

#define if_not_nan(param, str_add, op) \
	if (!std::isnan(param)) query_string << str_add << op;

#define if_not_neg_1(param, str_add, op) \
	if (param != -1) query_string << str_add << op;

#define set_query_range_parameter_float(n) \
	if_not_nan(filter.min_##n, "&min_"#n"=", filter.min_##n) \
	if_not_nan(filter.max_##n, "&max_"#n"=", filter.max_##n) \
	if_not_nan(filter.target_##n, "&target_"#n"=", filter.target_##n)

#define set_query_range_parameter_float_clamp(n, min, max) \
	if_not_nan(filter.min_##n, "&min_"#n"=", std::clamp(filter.min_##n, min, max)) \
	if_not_nan(filter.max_##n, "&max_"#n"=", std::clamp(filter.max_##n, min, max)) \
	if_not_nan(filter.target_##n, "&target_"#n"=", std::clamp(filter.target_##n, min, max))

#define set_query_range_parameter_float_min(n, min) \
	if_not_nan(filter.min_##n, "&min_"#n"=", std::max(min, filter.min_##n)) \
	if_not_nan(filter.max_##n, "&max_"#n"=", std::max(min, filter.max_##n)) \
	if_not_nan(filter.target_##n, "&target_"#n"=", std::max(min, filter.target_##n))

#define set_query_range_parameter_int_clamp(n, min, max) \
	if_not_neg_1(filter.min_##n, "&min_"#n"=", std::clamp(filter.min_##n, min, max)) \
	if_not_neg_1(filter.max_##n, "&max_"#n"=", std::clamp(filter.max_##n, min, max)) \
	if_not_neg_1(filter.target_##n, "&target_"#n"=", std::clamp(filter.target_##n, min, max))

#define set_query_range_parameter_int_min(n, min) \
	if_not_neg_1(filter.min_##n, "&min_"#n"=", std::max(min, filter.min_##n)) \
	if_not_neg_1(filter.max_##n, "&max_"#n"=", std::max(min, filter.max_##n)) \
	if_not_neg_1(filter.target_##n, "&target_"#n"=", std::max(min, filter.target_##n))

	set_query_range_parameter_float_clamp(acousticness, 0.0, 1.0)
	set_query_range_parameter_float_clamp(danceability, 0.0, 1.0)
	set_query_range_parameter_int_min(duration_ms, 1)
	set_query_range_parameter_float_clamp(energy, 0.0, 1.0)
	set_query_range_parameter_float_clamp(instrumentalness, 0.0, 1.0)
	set_query_range_parameter_int_clamp(key, 0, 11)
	set_query_range_parameter_float_clamp(liveness, 0.0, 1.0)
	set_query_range_parameter_float(loudness)
	set_query_range_parameter_int_clamp(mode, 0, 1)
	set_query_range_parameter_int_clamp(popularity, 0, 100)
	set_query_range_parameter_float_clamp(speechiness, 0.0, 1.0)
	set_query_range_parameter_float_min(tempo, 0.0)
	set_query_range_parameter_int_clamp(time_signature, 1, 11)
	set_query_range_parameter_float_clamp(valence, 0.0, 1.0)

	return query_string.str();

	#undef set_query_range_parameter_float
	#undef set_query_range_parameter_float_clamp
	#undef set_query_range_parameter_float_min
	#undef set_query_range_parameter_int_clamp
	#undef set_query_range_parameter_int_min

	#undef is_not_nan
	#undef is_not_neg_1
}

std::unique_ptr<audio_features_t> Track_API::get_audio_features_for_track(const std::string &track_id)
{
	std::string url = API_PREFIX "/audio-features/";
	url += truncate_spotify_uri(track_id);

	auto response = http::get(url.c_str(), std::string(), this->access_token);

	if (response.code != 200) return std::unique_ptr<audio_features_t>(nullptr);
	return audio_features_t::from_json(response.body);
}

std::vector<std::unique_ptr<audio_features_t>> Track_API::get_audio_features_for_tracks(const std::vector<std::string> &track_ids)
{
	std::ostringstream query_data;
	query_data << "?ids=";
	std::vector<std::string> truncated_ids = truncate_spotify_uris(track_ids, 100);

	for (size_t i = 0; i < truncated_ids.size(); ++i)
	{
		query_data << truncated_ids[i];
		if (i < truncated_ids.size() - 1)
		{
			query_data << http::url_encode(",");
		}
	}
	auto response = http::get(API_PREFIX "/audio-features", query_data.str(), this->access_token);
	std::vector<std::unique_ptr<audio_features_t>> features;
	if (response.code != 200) return features;

	json::json features_json = json::json::parse(response.body)["audio_features"];
	for (auto feat = features_json.begin(); feat != features_json.end(); ++feat)
	{
		features.push_back(audio_features_t::from_json(feat.value()));
	}
	return features;
}

std::unique_ptr<audio_analysis_t> Track_API::get_audio_analysis_for_track(const std::string &track_id)
{
	std::ostringstream url;
	url << API_PREFIX << "/audio-analysis/" << truncate_spotify_uri(track_id);
	auto response = http::get(url.str().c_str(), std::string(), this->access_token);
	if (response.code != 200) return std::unique_ptr<audio_analysis_t>(nullptr);
	return audio_analysis_t::from_json(response.body);
}

std::unique_ptr<Track_API::recommendations_t> Track_API::get_recommendations(const recommendation_filter_t &filter)
{
	std::string query_data = filter_to_query_string(filter);
	auto response = http::get(API_PREFIX "/recommendations", query_data, this->access_token);
	
	if (response.code != 200) return std::unique_ptr<Track_API::recommendations_t>(nullptr);

	auto retval = std::make_unique<Track_API::recommendations_t>();
	json::json response_json = json::json::parse(response.body);

	for (auto seed = response_json["seeds"].begin(); seed != response_json["seeds"].end(); ++seed)
	{
		retval->first.push_back(*recommendation_seed_t::from_json(seed.value()));
	}

	for (auto track = response_json["tracks"].begin(); track != response_json["tracks"].end(); ++track)
	{
		retval->second.push_back(track_t::from_json(track.value()));
	}

	return retval;
}

} // namespace spotify_api