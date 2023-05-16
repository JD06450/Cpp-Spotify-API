#include "player.hpp"

namespace json = nlohmann;

namespace spotify_api
{
	void Player_API::object_from_json(const std::string &json_value, context_t *output)
	{
		json::json json_object = json::json::parse(json_value);

		output->type = json_object["type"];
		output->href = json_object["href"];
		output->uri = json_object["uri"];

		auto external_urls = json_object["external_urls"];
		for (auto uri = external_urls.begin(); uri != external_urls.end(); ++uri)
		{
			output->external_urls.emplace(uri.key(), uri.value().dump());
		}
	}

	playback_state_t *Player_API::get_playback_state()
	{
		playback_state_t *player_state = new playback_state_t;

		http::api_response response = http::get(API_PREFIX "/me/player", std::string(""), this->_access_token);

		if (response.code != 200) return player_state;
		
		json::json json_object = json::json::parse(response.body);
		// playback device
		json::json json_device = json_object["device"];

		player_state->device.id = json_device["id"];
		player_state->device.is_active = json_device["is_active"];
		player_state->device.is_private_session = json_device["is_private_session"];
		player_state->device.is_restricted = json_device["is_restricted"];
		player_state->device.name = json_device["name"];
		player_state->device.type = json_device["type"];
		player_state->device.volume_percent = json_device["volume_percent"];

		// std::string repeat_state;
		player_state->repeat_state = json_object["repeat_state"];

		// bool shuffle_state;
		player_state->shuffle_state = json_object["shuffle_state"];

		// std::optional<context_t> context;
		player_state->context.reset();
		if (json_object.contains("context"))
		{
			context_t *temp_context = new context_t;
			object_from_json(json_object["context"].dump(), temp_context);
			player_state->context = *temp_context;
		}

		// uint64_t timestamp;
		player_state->timestamp = json_object["timestamp"];

		// int progress_ms;
		player_state->progress_ms = json_object["progress_ms"];

		// bool is_playing;
		player_state->is_playing = json_object["is_playing"];

		// track_t *item;
		track_t *temp_track = new track_t;
		Track_API::object_from_json(json_object["item"].dump(), temp_track);
		player_state->item = temp_track;

		// std::string currently_playing_type;
		player_state->currently_playing_type = json_object["currently_playing_type"];

		// context_actions_t actions;
		json::json json_actions = json_object["actions"];
		player_state->actions.interrupting_playback = json_actions["interrupting_playback"];
		player_state->actions.pausing = json_actions["pausing"];
		player_state->actions.resuming = json_actions["resuming"];
		player_state->actions.seeking = json_actions["seeking"];
		player_state->actions.skipping_next = json_actions["skipping_next"];
		player_state->actions.skipping_prev = json_actions["skipping_prev"];
		player_state->actions.toggling_repeat_context = json_actions["toggling_repeat_context"];
		player_state->actions.toggling_repeat_track = json_actions["toggling_repeat_track"];
		player_state->actions.toggling_shuffle = json_actions["toggling_shuffle"];
		player_state->actions.transfer_playback = json_actions["transfer_playback"];

		return player_state;
	}

	void Player_API::transfer_playback(const std::string &device_id, bool play)
	{
		std::string post_data = "{\"device_ids\": [\"" + device_id + "\"], \"play\": " + (play ? "true" : "false") + "}";
		http::put(API_PREFIX "/me/player", post_data, this->_access_token, true);
	}

	std::vector<playback_device_t> Player_API::get_available_devices()
	{
		http::api_response devices_string = http::get(API_PREFIX "/me/player/devices", std::string(""), this->_access_token);
		std::vector<playback_device_t> devices = {};

		if (devices_string.code != 200) return devices;

		json::json devices_json = json::json::parse(devices_string.body);

		devices_json = devices_json["devices"];
		for (auto device = devices_json.begin(); device != devices_json.end(); ++device)
		{
			playback_device_t temp;
			temp.id = device.value()["id"];
			temp.is_active = device.value()["is_active"];
			temp.is_private_session = device.value()["is_private_session"];
			temp.is_restricted = device.value()["is_restricted"];
			temp.name = device.value()["name"];
			temp.type = device.value()["type"];
			temp.volume_percent = device.value()["volume_percent"];
			devices.push_back(temp);
		}

		return devices;
	}

	track_t *Player_API::get_currently_playing_track()
	{
		track_t *current_track = new track_t;
		http::api_response response = http::get(API_PREFIX "/me/player/currently-playing", std::string(""), this->_access_token);
		printf("current track res code: %u\n", response.code);
		if (response.code == 200)
		{
			Track_API::object_from_json(response.body, current_track);
		}
		return current_track;
	}

	void Player_API::start_or_resume_playback(const std::string &context_uri, const std::vector<std::string> &uris, int offset, int position_ms)
	{
		json::json put_data;
		if (context_uri != "") put_data["context_uri"] = context_uri;
		if (uris.size() > 0) put_data["uris"] = uris;
		put_data["offset"] = {{"position", offset}};
		put_data["position_ms"] = position_ms;

		http::put(API_PREFIX "/me/player/play", put_data, this->_access_token, true);
	}

	void Player_API::pause_playback()
	{
		http::put(API_PREFIX "/me/player/pause", std::string(), this->_access_token, true);
	}

	void Player_API::seek_to_position(int position_ms)
	{
		json::json put_data;
		put_data["position_ms"] = position_ms;
		http::put(API_PREFIX "/me/player/seek", put_data, this->_access_token, true);
	}

	void Player_API::set_repeat_mode(Player_API::REPEAT_MODE state)
	{
		json::json put_data;

		switch (state)
		{
		case Player_API::REPEAT_MODE::TRACK:
			put_data["state"] = "track";
			break;
		
		case Player_API::REPEAT_MODE::CONTEXT:
			put_data["state"] = "track";
			break;

		default:
			put_data["state"] = "off";
			break;
		}
		http::put(API_PREFIX "/me/player/repeat", put_data, this->_access_token, true);
	}

	void Player_API::set_volume(int volume_percent)
	{
		json::json put_data;

		if (volume_percent > 100) volume_percent = 100;
		if (volume_percent < 0) volume_percent = 0;

		put_data["volume_percent"] = volume_percent;

		http::put(API_PREFIX "/me/player/volume", put_data, this->_access_token, true);
	}
	
	void Player_API::set_shuffle(bool state)
	{
		json::json put_data;
		put_data["state"] = state;
		http::put(API_PREFIX "/me/player/shuffle", put_data, this->_access_token, true);
	}

	void Player_API::object_from_json(const std::string &json_string, Player_API::recent_tracks_t *output)
	{
		json::json json_obj = json::json(json_string);

		output->href = json_obj["href"];
		
		if (json_obj["cursor"].contains("before")) {
			output->cursor.before = json_obj["cursor"]["before"];
		}

		if (json_obj["cursor"].contains("after")) {
			output->cursor.after = json_obj["cursor"]["after"];
		}

		output->limit = json_obj["limit"];
		if (!json_obj["next"].is_null()) {
			output->next = json_obj["next"];
		}

		output->total = json_obj["total"];
		

		for (auto i = json_obj["items"].begin(); i != json_obj["items"].end(); ++i)
		{
			json::json temp_obj = i.value();
			track_history_t *temp_history;	

			temp_history->played_at = temp_obj["played_at"];
			object_from_json(temp_obj["context"].dump(), &temp_history->context);
			Track_API::object_from_json(temp_obj["track"].dump(), &temp_history->track);
		}
	};

	Player_API::recent_tracks_t * Player_API::get_recently_played_tracks(int limit = 20, unsigned int timestamp = 0, bool after = false)
	{
		Player_API::recent_tracks_t *recent_tracks = new Player_API::recent_tracks_t;

		if (limit < 0) limit = 0;
		if (limit > 50) limit = 50;

		std::string query_data = "?limit=" + std::to_string(limit);
		
		if (timestamp > 0) {
			query_data += "&" + std::string(after ? "after" : "before") + "=" + std::to_string(timestamp);
		}
		
		auto response = http::get(API_PREFIX "/me/player/recently-played", query_data, this->_access_token);

		if (response.code != 200) {
			return recent_tracks;
		}

		object_from_json(response.body, recent_tracks);
		return recent_tracks;
	}

	void Player_API::object_from_json(const std::string &json_string, queue_t *output)
	{
		json::json json_object = json::json::parse(json_string);

		if (!json_object["currently_playing"].is_null()) {
			std::string queue_type = json_object["currently_playing"]["type"].get<std::string>();
			// TODO: error handling for invalid object type
			// if (queue_type != "episode" && queue_type != "track")
			output->type = queue_type;
		} else if (!json_object["queue"].empty()) {
			std::string queue_type = json_object["queue"][0]["type"].get<std::string>();

			output->type = queue_type;
		} else {
			return;
		}

		if (output->type == "track") {
			track_t *temp = new track_t;
			if (!json_object["currently_playing"].is_null()) {
				Track_API::object_from_json(json_object["currently_playing"].dump(), temp);
				output->current_track = temp;
			}
			output->tracks = {};
			
			for (auto i = json_object["queue"].begin(); i != json_object["queue"].end(); ++i)
			{
				track_t *temp_track = new track_t;
				Track_API::object_from_json(i.value().dump(), temp_track);
				output->tracks.value().push_back(temp_track);
			}

			return;
		} else if (output->type == "episode") {

		}

	}

	queue_t * Player_API::get_queue()
	{
		queue_t * queue = new queue_t;

		auto api_response = http::get(API_PREFIX "/me/player/queue", std::string(), this->_access_token);

		if (api_response.code != 200) {
			return queue;
		}

		object_from_json(api_response.body, queue);
	}
	
	track_t * Player_API::search_for_track(const std::string &q)
	{
		std::string query = "q=" + q + "&type=track";
		auto response = http::get(API_PREFIX "/search", query, this->_access_token);

		if (response.code != 200) {
			return new track_t;
		}

		json::json temp = json::json::parse(response.body);

		track_t *top_track = new track_t;
		Track_API::object_from_json(temp["items"][0].dump(), top_track);
		return top_track;
	}
} // namespace spotify_api
