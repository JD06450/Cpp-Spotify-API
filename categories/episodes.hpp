#pragma once
#ifndef _SPOTIFY_API_EPISODES_
#define _SPOTIFY_API_EPISODES_

#include <map>
#include <string>
#include <vector>
#include <optional>
#include <memory>

#include "common.hpp"
#include "shows.hpp"

namespace spotify_api
{
	struct resume_point_t {
		// Whether or not the episode has been fully played by the user.
		bool fully_played;
		// The user's most recent position in the episode in milliseconds.
		int resume_position_ms;
	};

	struct episode_t {
		// A URL to a 30 second preview (MP3 format) of the episode. null if not available
		std::string audio_preview_url;
		// A description of the episode. HTML tags are stripped away from this field, use html_description field in case HTML tags are needed
		std::string description;
		// A description of the episode. This field may contain HTML tags
		std::string html_description;
		// The length of the episode in milliseconds
		int duration_ms;
		// Whether or not the episode has explicit content (true = yes it does; false = no it does not OR unknown)
		bool is_explicit;
		// External urls for this episode
		std::map<std::string, std::string> external_urls;
		// A link to the Web API endpoint providing full details of the episode
		std::string href;
		// The spotify ID for this episode
		std::string id;
		// The cover art for the episode in various sizes, widest first.
		std::vector<image_t> images;
		// True if the episode is hosted outside of Spotify's CDN.
		bool is_externally_hosted;
		// True if the episode is playable in the given market. Otherwise false.
		bool is_playable;
		// A list of the languages used in the episode, identified by their ISO 639-1 code.
		std::vector<std::string> languages;
		// The name of the episode;
		std::string name;
		// The date the episode was released in YYYY-MM-DD format. Depending on the precision, the day or month might not be shown.
		std::string release_date;
		// The precision with which the release date is known
		std::string release_date_precision;
		// The user's most recent position in the episode. Set if the supplied access token is a user token and has the scope 'user-read-playback-position'
		resume_point_t resume_point;
		// The object type
		const std::string type = "episode";
		// The spotify URI for this episode
		std::string uri;
		// Included in the response when a content restriction is applied
		std::string restrictions;
		// The show that the episode belongs to
		show_t *show;
		
		static std::unique_ptr<episode_t> from_json(const std::string &json_string);
	};
	
	class Episode_API
	{
		public:
		std::string access_token;
		Episode_API(std::string access_token): access_token(access_token) {}
	};
	
} // namespace spotify_api


#endif