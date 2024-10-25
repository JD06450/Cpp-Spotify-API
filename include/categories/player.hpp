#pragma once
#ifndef _SPOTIFY_API_PLAYER_T_
#define _SPOTIFY_API_PLAYER_T_

#include <string>
#include <variant>
#include <map>
#include <vector>
#include <variant>

#include "./tracks.hpp"
#include "./episodes.hpp"
#include "../curl-util.hpp"

#include <nlohmann/json.hpp>

namespace spotify_api
{

using playable_type = std::variant<std::monostate, spotify_api::track_t, spotify_api::episode_t>;


struct playback_device_t
{
	std::string id;
	// If this device is the currently active device
	bool is_active;
	// If this device is in a private session
	bool is_private_session;
	// Whether controlling this device is restricted. At present if this is "true" then no Web API commands will be accepted by this device.
	bool is_restricted;
	std::string name;
	std::string type;
	int volume_percent;
	
	static std::unique_ptr<playback_device_t> from_json(const std::string &json_string);

	static std::unique_ptr<playback_device_t> from_json(const nlohmann::json &json_obj);
};


struct context_t
{
	std::string type;
	std::string href;
	std::map<std::string, std::string> external_urls;
	std::string uri;
	
	static std::unique_ptr<context_t> from_json(const std::string &json_string);

	static std::unique_ptr<context_t> from_json(const nlohmann::json &json_obj);
};

// According to the spotify api docs, all of these options are technically "optional" in an api response,
// but if any option is not present, then it shall be defaulted to false

struct context_actions_t
{
	bool interrupting_playback = false;
	bool pausing = false;
	bool resuming = false;
	bool seeking = false;
	bool skipping_prev = false;
	bool skipping_next = false;
	bool toggling_repeat_context = false;
	bool toggling_shuffle = false;
	bool toggling_repeat_track = false;
	bool transfer_playback = false;
	
	static std::unique_ptr<context_actions_t> from_json(const std::string &json_string);

	static std::unique_ptr<context_actions_t> from_json(const nlohmann::json &json_obj);
};

struct playback_state_t
{
	std::shared_ptr<playback_device_t> device;
	std::string repeat_state;
	bool shuffle_state;
	std::optional<std::shared_ptr<context_t>> context;
	// Unix timestamp when the data was fetched
	unsigned long timestamp;
	int progress_ms;
	bool is_playing;

	std::shared_ptr<track_t> item;

	std::string currently_playing_type;
	// A list of actions that can be performed and whether or not that action is allowed within the current context
	std::shared_ptr<context_actions_t> actions;

	static std::unique_ptr<playback_state_t> from_json(const std::string &json_string);
	
	static std::unique_ptr<playback_state_t> from_json(const nlohmann::json &json_obj);
};

struct queue_t
{
	/**
	 * @brief specifies the type of items that are present in the queue. Currently only the values "track" and "episode" are allowed.
	*/
	std::string type = "unknown";

	std::shared_ptr<playable_type> current_item;
	std::vector<std::shared_ptr<playable_type>> items;

	static std::unique_ptr<queue_t> from_json(const std::string &json_string);

	static std::unique_ptr<queue_t> from_json(const nlohmann::json &json_obj);
};

struct track_history_t
{
	std::shared_ptr<track_t> track;
	std::string played_at;
	std::shared_ptr<context_t> context;

	static std::unique_ptr<track_history_t> from_json(const std::string &json_string);

	static std::unique_ptr<track_history_t> from_json(const nlohmann::json &json_obj);
};

struct recent_tracks_t
{
	struct cursor_t
	{
		int before;
		int after;
	};
	
	std::string href;
	int limit;
	int total;
	std::string next;
	std::vector<std::shared_ptr<track_history_t>> items;
	
	cursor_t cursor;
	
	static std::unique_ptr<recent_tracks_t> from_json(const std::string &json_string);

	static std::unique_ptr<recent_tracks_t> from_json(const nlohmann::json &json_obj);
};

} // namespace spotify_api

#endif
