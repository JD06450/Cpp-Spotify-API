#include <iostream>
#include "player.hpp"

#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{

playback_device_t * parse_playback_device(const std::string &json_string)
{
	playback_device_t * device = new playback_device_t();
	try
	{
		json::json json_object = json::json::parse(json_string);
		device->id = json_object.value("id", "");
		device->is_active = json_object["is_active"];
		device->is_private_session = json_object["is_private_session"];
		device->is_restricted = json_object["is_restricted"];
		device->name = json_object["name"];
		device->type = json_object["type"];
		device->volume_percent = json_object.value("volume_percent", -1);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return device;
}

context_t * parse_player_context(const std::string &json_string)
{
	context_t * context = new context_t();
	try
	{
		json::json json_object = json::json::parse(json_string);
		context->type = json_object["type"];
		context->href = json_object["href"];
		context->uri = json_object["uri"];

		for (auto url = json_object["external_urls"].begin(); url != json_object["external_urls"].end(); ++url)
		{
			context->external_urls.emplace(url.key(), url.value().dump());
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return context;
}

context_actions_t * parse_context_actions(const std::string &json_string)
{
	context_actions_t * actions = new context_actions_t();
	try
	{
		json::json json_object = json::json::parse(json_string);
		actions->interrupting_playback = json_object.value("interrupting_playback", false);
		actions->pausing = json_object.value("pausing", false);
		actions->resuming = json_object.value("resuming", false);
		actions->seeking = json_object.value("seeking", false);
		actions->skipping_prev = json_object.value("skipping_prev", false);
		actions->skipping_next = json_object.value("skipping_next", false);
		actions->toggling_repeat_context = json_object.value("toggling_repeat_context", false);
		actions->toggling_shuffle = json_object.value("toggling_shuffle", false);
		actions->toggling_repeat_track = json_object.value("toggling_repeat_track", false);
		actions->transfer_playback = json_object.value("transfer_playback", false);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return actions;
}

playback_state_t * parse_playback_state(const std::string &json_string)
{
	playback_state_t * state = new playback_state_t();
	try
	{
		json::json json_object = json::json::parse(json_string);
		// Yes, I know. This is a memory leak. Don't care at the moment. I will be changing this
		// to a smart pointer later.
		state->actions = *parse_context_actions(json_object["actions"].dump());
		if (!json_object["context"].is_null())
		{
			state->context.reset();
		}
		else
		{
			// Memory leak #2.
			state->context = *parse_player_context(json_object["context"].dump());
		}
		state->currently_playing_type = json_object["currently_playing_type"];
		// Last but not least, mem leak #3
		state->device = *parse_playback_device(json_object["device"].dump());
		state->is_playing = json_object["is_playing"];
		state->item = Track_API::object_from_json(json_object["item"].dump());
		state->progress_ms = json_object["progress_ms"];
		state->repeat_state = json_object["repeate_state"];
		state->shuffle_state = json_object["shuffle_state"];
		state->timestamp = json_object["timestamp"];
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return state;
}

queue_t * parse_queue(const std::string &json_string)
{
	queue_t * queue = new queue_t;
	json::json json_object = json::json::parse(json_string);

	if (!json_object["currently_playing"].is_null())
	{
		queue->type = json_object["currently_playing"]["type"];
	}
	else if (!json_object["queue"].empty())
	{
		queue->type = json_object["queue"][0]["type"];
	}
	else
	{
		return queue;
	}

	// TODO: error handling for invalid object type
	// if (queue_type != "episode" && queue_type != "track")
	if (queue->type == "track")
	{
		if (!json_object["currently_playing"].is_null())
		{
			queue->current_track = Track_API::object_from_json(json_object["currently_playing"].dump());
		}
		queue->tracks.emplace(std::vector<track_t *>());
		
		for (auto track = json_object["queue"].begin(); track != json_object["queue"].end(); ++track)
		{
			queue->tracks.value().push_back(Track_API::object_from_json(track.value().dump()));
		}

		return queue;
	}
	else if (queue->type == "episode")
	{
		if (!json_object["currently_playing"].is_null())
		{
			queue->current_episode = Episode_API::object_from_json(json_object["currently_playing"].dump());
		}
		queue->episodes.emplace(std::vector<episode_t *>());

		for (auto episode = json_object["queue"].begin(); episode != json_object["queue"].end(); ++episode)
		{
			queue->episodes.value().push_back(Episode_API::object_from_json(episode.value().dump()));
		}
		return queue;
	}
	return queue;
}

playback_state_t *Player_API::get_playback_state()
{
	http::api_response response = http::get(API_PREFIX "/me/player", std::string(""), this->access_token);

	if (response.code != 200) return new playback_state_t();
	return parse_playback_state(response.body);
}

void Player_API::transfer_playback(const std::string &device_id, bool ensure_playback)
{
	json::json post_data = {
		{"device_ids", {device_id}},
		{"play", ensure_playback}
	};
	(void) http::request(API_PREFIX "/me/player", http::REQUEST_METHOD::PUT, post_data.dump(), this->access_token, true);
}

std::vector<playback_device_t> Player_API::get_available_devices()
{
	http::api_response devices_string = http::get(API_PREFIX "/me/player/devices", std::string(""), this->access_token);
	std::vector<playback_device_t> devices = {};

	if (devices_string.code != 200) return devices;

	json::json devices_json = json::json::parse(devices_string.body)["devices"];
	for (auto device = devices_json.begin(); device != devices_json.end(); ++device)
	{
		// Here's a memory leak.
		devices.push_back(*parse_playback_device(device.value().dump()));
	}

	return devices;
}

track_t *Player_API::get_currently_playing_track()
{
	http::api_response response = http::get(API_PREFIX "/me/player/currently-playing", std::string(""), this->access_token);
	if (response.code != 200) return new track_t();
	return Track_API::object_from_json(response.body);
}

void Player_API::start_or_resume_playback(const std::string &context_uri, const std::vector<std::string> &uris, int offset, int position_ms)
{
	json::json put_data;
	if (context_uri != "") put_data["context_uri"] = context_uri;
	if (uris.size() > 0) put_data["uris"] = uris;
	put_data["offset"] = {{"position", offset}};
	put_data["position_ms"] = position_ms;

	http::request(API_PREFIX "/me/player/play", http::REQUEST_METHOD::PUT, put_data, this->access_token, true);
}

void Player_API::pause_playback()
{
	http::request(API_PREFIX "/me/player/pause", http::REQUEST_METHOD::PUT, std::string(), this->access_token, true);
}

void Player_API::seek_to_position(int position_ms)
{
	json::json put_data = {{"position_ms", position_ms}};
	http::request(API_PREFIX "/me/player/seek", http::REQUEST_METHOD::PUT, put_data.dump(), this->access_token, true);
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
		put_data["state"] = "context";
		break;

	default:
		put_data["state"] = "off";
		break;
	}
	http::request(API_PREFIX "/me/player/repeat", http::REQUEST_METHOD::PUT, put_data.dump(), this->access_token, true);
}

void Player_API::set_volume(int volume_percent)
{
	json::json put_data;

	if (volume_percent > 100) volume_percent = 100;
	if (volume_percent < 0) volume_percent = 0;

	put_data["volume_percent"] = volume_percent;

	http::request(API_PREFIX "/me/player/volume", http::REQUEST_METHOD::PUT, put_data.dump(), this->access_token, true);
}

void Player_API::set_shuffle(bool state)
{
	json::json put_data;
	put_data["state"] = state;
	http::request(API_PREFIX "/me/player/shuffle", http::REQUEST_METHOD::PUT, put_data.dump(), this->access_token, true);
}

recent_tracks_t * parse_recent_tracks(const std::string &json_string)
{
	recent_tracks_t * recent_tracks = new recent_tracks_t();
	try
	{
		json::json json_object = json::json(json_string);

		recent_tracks->href = json_object["href"];
		recent_tracks->cursor.before = json_object["cursor"].value("before", -1);
		recent_tracks->cursor.after = json_object["cursor"].value("after", -1);
		recent_tracks->limit = json_object["limit"];
		recent_tracks->next = json_object.value("next", "");
		recent_tracks->total = json_object["total"];
		

		for (auto item = json_object["items"].begin(); item != json_object["items"].end(); ++item)
		{
			json::json temp_obj = item.value();
			track_history_t temp_history;	

			temp_history.played_at = temp_obj["played_at"];
			// This here is an example of YAML. Yet Another Memory Leak
			temp_history.context = *parse_player_context(temp_obj["context"].dump());
			temp_history.track = Track_API::object_from_json(temp_obj["track"].dump());
			recent_tracks->items.push_back(temp_history);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return recent_tracks;
};

recent_tracks_t * Player_API::get_recently_played_tracks(int limit = 20, unsigned int timestamp = 0, bool after = false)
{
	recent_tracks_t *recent_tracks = new recent_tracks_t();

	if (limit < 0) limit = 0;
	if (limit > 50) limit = 50;

	std::string query_data = "limit=" + std::to_string(limit);
	
	if (timestamp > 0) {
		query_data += "&" + std::string(after ? "after" : "before") + "=" + std::to_string(timestamp);
	}
	
	auto response = http::get(API_PREFIX "/me/player/recently-played", query_data, this->access_token);

	if (response.code != 200) return recent_tracks;

	return parse_recent_tracks(response.body);
}

queue_t * Player_API::get_queue()
{
	auto response = http::get(API_PREFIX "/me/player/queue", std::string(), this->access_token);
	if (response.code != 200) {
		return new queue_t();
	}
	return parse_queue(response.body);
}


void Player_API::add_item_to_playback_queue(const std::string &item_uri)
{
	std::string url = API_PREFIX "/me/player/queue?uri=";
	url += http::url_encode(item_uri);

	auto response = http::post(url.c_str(), std::string(), this->access_token, true);
}

track_t * Player_API::search_for_track(const std::string &q)
{
	std::string query = "q=" + q + "&type=track";
	auto response = http::get(API_PREFIX "/search", query, this->access_token);

	if (response.code != 200) {
		std::cerr << "failed to search for track: " << response.code << '\n';
		return new track_t();
	}

	json::json temp = json::json::parse(response.body);

	return Track_API::object_from_json(temp["tracks"]["items"][0].dump());
}

} // namespace spotify_api
