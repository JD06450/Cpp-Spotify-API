#pragma once
#ifndef _SPOTIFY_API_TRACKS_
#define _SPOTIFY_API_TRACKS_

#include <string>
#include <vector>
#include <optional>
#include <cmath>
#include <memory>

#include "albums.hpp"
#include "artists.hpp"
#include "common.hpp"

namespace spotify_api
{
	struct track_t
	{
		std::unique_ptr<album_t> album;
		std::vector<std::unique_ptr<artist_t>> artists;
		std::vector<std::string> available_markets;
		int disc_number;
		int duration_ms;
		bool is_explicit;
		std::map<std::string, std::string> external_ids;
		std::map<std::string, std::string> external_urls;
		std::string href;
		std::string id;
		bool is_playable;
		std::optional<std::unique_ptr<track_t>> linked_from;
		std::map<std::string, std::string> restrictions;
		std::string name;
		int popularity;
		std::string preview_url;
		int track_number;
		const std::string type = "track";
		std::string uri;
		bool is_local;

		static std::unique_ptr<track_t> from_json(const std::string &json_string);
	};

	struct audio_features_t {
		double acousticness;
		std::string analysis_url;
		double danceability;
		unsigned int duration_ms;
		double energy;
		std::string id;
		double instrumentalness;
		int key;
		double liveness;
		double loudness;
		int mode;
		double speechiness;
		double tempo;
		int time_signature;
		std::string track_href;
		const std::string type = "audio_features";
		std::string uri;
		double valence;
	};

	struct audio_analysis_t
	{
		struct meta_t
		{
			std::string analyzer_version;
			std::string platform;
			std::string detailed_status;
			int status_code;
			uint64_t timestamp;
			double analysis_time;
			std::string input_process;
			
			static std::unique_ptr<meta_t> from_json(const std::string &json_string);
		};

		struct track_t
		{
			uint64_t samples;
			double duration;
			std::string sample_md5; // Always empty
			double offset_seconds;
			double window_seconds;
			int analysis_sample_rate;
			int analysis_channels;
			double end_of_fade_in;
			double start_of_fade_out;
			double loudness;
			double tempo;
			double tempo_confidence;
			int time_signature;
			double time_signature_confidence;
			int key;
			double key_confidence;
			int mode;
			double mode_confidence;
			std::string codestring;
			std::string code_version;
			std::string echoprintstring;
			std::string echoprint_version;
			std::string synchstring;
			std::string synch_version;
			std::string rhythmstring;
			std::string rhythm_version;
			
			static std::unique_ptr<track_t> from_json(const std::string &json_string);
		};


		struct bar_t
		{
			double start;
			double duration;
			double confidence;

			static std::unique_ptr<bar_t> from_json(const std::string &json_string);
			// static std::vector<std::unique_ptr<bar_t>> parse_analysis_bars(const std::string &json_string);
		};


		struct beat_t
		{
			double start;
			double duration;
			double confidence;
		
			static std::unique_ptr<beat_t> from_json(const std::string &json_string);
			// static std::vector<std::unique_ptr<beat_t>> parse_analysis_beats(const std::string &json_string);
		};


		struct section_t
		{
			double start;
			double duration;
			double confidence;
			double loudness;
			double tempo;
			double tempo_confidence;
			int key;
			double key_confidence;
			int mode;
			double mode_confidence;
			int time_signature;
			double time_signature_confidence;
		
			static std::unique_ptr<section_t> from_json(const std::string &json_string);
			// static std::vector<std::unique_ptr<section_t>> parse_analysis_sections(const std::string &json_string);
		};


		struct segment_t
		{
			double start;
			double duration;
			double confidence;
			double loudness_start;
			double loudness_max;
			double loudness_max_time;
			double loudness_end;
			std::vector<int> pitches;
			std::vector<int> timbre;
			
			static std::unique_ptr<segment_t> from_json(const std::string &json_string);
			// static std::vector<std::unique_ptr<segment_t>> parse_analysis_segments(const std::string &json_string);
		};


		struct tatum_t
		{
			double start;
			double duration;
			double confidence;
			
			static std::unique_ptr<tatum_t> from_json(const std::string &tatum_string);
			// static std::vector<std::unique_ptr<tatum_t>> parse_analysis_tatums(const std::string &tatum_strings);
		};


		meta_t meta;
		track_t track;
		std::vector<bar_t> bars;
		std::vector<beat_t> beats;
		std::vector<section_t> sections;
		std::vector<segment_t> segments;
		std::vector<tatum_t> tatums;

		static std::unique_ptr<audio_analysis_t> from_json(const std::string &json_string);
	};


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
		
		struct recommendation_seed_t
		{
			unsigned int after_filtering_size;
			unsigned int after_relinking_size;
			std::string href;
			std::string id;
			unsigned int initial_pool_size;
		};

		// This is a lot of filters
		// Using std::nan as defualt value to tell that the property is unused

		struct recommendation_filter_t
		{
			unsigned int limit;
			std::string market;
			std::vector<std::string> seed_artists;
			std::vector<std::string> seed_genres;
			std::vector<std::string> seed_tracks;
			// Range: 0.0 - 1.0
			double min_acousticness = std::nan("");
			// Range: 0.0 - 1.0
			double max_acousticness = std::nan("");
			// Range: 0.0 - 1.0
			double target_acousticness = std::nan("");
			// Range: 0.0 - 1.0
			double min_danceability = std::nan("");
			// Range: 0.0 - 1.0
			double max_danceability = std::nan("");
			// Range: 0.0 - 1.0
			double target_danceability = std::nan("");
			int min_duration_ms = -1;
			int max_duration_ms = -1;
			int target_duration_ms = -1;
			// Range: 0.0 - 1.0
			double min_energy = std::nan("");
			// Range: 0.0 - 1.0
			double max_energy = std::nan("");
			// Range: 0.0 - 1.0
			double target_energy = std::nan("");
			// Range: 0.0 - 1.0
			double min_instrumentalness = std::nan("");
			// Range: 0.0 - 1.0
			double max_instrumentalness = std::nan("");
			// Range: 0.0 - 1.0
			double target_instrumentalness = std::nan("");
			// Range: 0 - 11
			int min_key = -1;
			// Range: 0 - 11
			int max_key = -1;
			// Range: 0 - 11
			int target_key = -1;
			// Range: 0.0 - 1.0
			double min_liveness = std::nan("");
			// Range: 0.0 - 1.0
			double max_liveness = std::nan("");
			// Range: 0.0 - 1.0
			double target_liveness = std::nan("");
			double min_loudness = std::nan("");
			double max_loudness = std::nan("");
			double target_loudness = std::nan("");
			// Range: 0 - 1
			int min_mode = -1;
			// Range: 0 - 1
			int max_mode = -1;
			// Range: 0 - 1
			int target_mode = -1;
			// Range: 0 - 100
			int min_popularity = -1;
			// Range: 0 - 100
			int max_popularity = -1;
			// Range: 0 - 100
			int target_popularity = -1;
			// Range: 0.0 - 1.0
			double min_speechiness = std::nan("");
			// Range: 0.0 - 1.0
			double max_speechiness = std::nan("");
			// Range: 0.0 - 1.0
			double target_speechiness = std::nan("");
			double min_tempo = std::nan("");
			double max_tempo = std::nan("");
			double target_tempo = std::nan("");
			// Range: 1 - 11
			int min_time_signature = -1;
			int max_time_signature = -1;
			int target_time_signature = -1;
			// Range: 0.0 - 1.0
			double min_valence = std::nan("");
			// Range: 0.0 - 1.0
			double max_valence = std::nan("");
			// Range: 0.0 - 1.0
			double target_valence = std::nan("");
		};

		// I wish this type could be shorter. :(

		using recommendations_t = std::pair<std::vector<recommendation_seed_t>, std::vector<std::unique_ptr<track_t>>>;

		std::unique_ptr<recommendations_t> get_recommendations(const recommendation_filter_t &filter);
	};
} // namespace spotify_api

#endif