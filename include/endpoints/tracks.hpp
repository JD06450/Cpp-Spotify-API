#pragma once
#ifndef _SPOTIFY_API_TRACKS_
#define _SPOTIFY_API_TRACKS_

#include <string>
#include <vector>
#include <optional>
#include <cmath>
#include <memory>

#include <nlohmann/json.hpp>

#include "../categories/common.hpp"
#include "../categories/tracks.hpp"

namespace spotify_api
{
class Track_API
{
	public:
	std::string access_token;
	Track_API(std::string access_token): access_token(access_token) {}
	
	std::unique_ptr<track_t> get_track(const std::string &track_id, const std::string &market);

	std::vector<std::unique_ptr<track_t>> get_tracks(const std::vector<std::string> &track_ids, const std::string &market);

	page_t<std::unique_ptr<track_t>> get_saved_tracks(const std::string &market, uint8_t limit, unsigned int offset);

	void save_tracks(const std::vector<std::string> &track_ids);

	void remove_saved_tracks(const std::vector<std::string> &track_ids);

	std::vector<bool> check_saved_tracks(const std::vector<std::string> &track_ids);

	std::unique_ptr<audio_features_t> get_audio_features_for_track(const std::string &track_id);

	std::vector<std::unique_ptr<audio_features_t>> get_audio_features_for_tracks(const std::vector<std::string> &track_ids);

	std::unique_ptr<audio_analysis_t> get_audio_analysis_for_track(const std::string &track_id);
	
	
	// I wish this type could be shorter. :(

	using recommendations_t = std::pair<std::vector<recommendation_seed_t>, std::vector<std::unique_ptr<track_t>>>;

	std::unique_ptr<recommendations_t> get_recommendations(const recommendation_filter_t &filter);

	/**
	 * @brief Search for a track matching a specific query
	 * @note Endpoint: /search
	 * @param query The search query
	 * @returns The track that best matches the query
	*/
	page_t<std::unique_ptr<track_t>> search_for_track(const std::string &query);
	//TODO: support for all item types
};
} // namespace spotify_api

#endif