#include <iostream>
#include "categories/player.hpp"
#include "endpoints/player.hpp"

#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{

std::unique_ptr<playback_device_t> playback_device_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<playback_device_t> playback_device_t::from_json(const json::json &json_obj)
{
	auto device = std::make_unique<playback_device_t>();
	try
	{
		device->id = json_obj.value("id", "");
		device->is_active = json_obj["is_active"];
		device->is_private_session = json_obj["is_private_session"];
		device->is_restricted = json_obj["is_restricted"];
		device->name = json_obj["name"];
		device->type = json_obj["type"];
		device->volume_percent = json_obj.value("volume_percent", -1);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return device;
}


std::unique_ptr<context_t> context_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<context_t> context_t::from_json(const json::json &json_obj)
{
	auto context = std::make_unique<context_t>();
	try
	{
		context->type = json_obj["type"];
		context->href = json_obj["href"];
		context->uri = json_obj["uri"];

		for (auto url = json_obj["external_urls"].begin(); url != json_obj["external_urls"].end(); ++url)
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


std::unique_ptr<context_actions_t> context_actions_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<context_actions_t> context_actions_t::from_json(const json::json &json_obj)
{
	auto actions = std::make_unique<context_actions_t>();
	try
	{
		actions->interrupting_playback = json_obj.value("interrupting_playback", false);
		actions->pausing = json_obj.value("pausing", false);
		actions->resuming = json_obj.value("resuming", false);
		actions->seeking = json_obj.value("seeking", false);
		actions->skipping_prev = json_obj.value("skipping_prev", false);
		actions->skipping_next = json_obj.value("skipping_next", false);
		actions->toggling_repeat_context = json_obj.value("toggling_repeat_context", false);
		actions->toggling_shuffle = json_obj.value("toggling_shuffle", false);
		actions->toggling_repeat_track = json_obj.value("toggling_repeat_track", false);
		actions->transfer_playback = json_obj.value("transfer_playback", false);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return actions;
}


std::unique_ptr<playback_state_t> playback_state_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<playback_state_t> playback_state_t::from_json(const json::json &json_obj)
{
	auto state = std::make_unique<playback_state_t>();
	try
	{
		state->actions = context_actions_t::from_json(json_obj["actions"]);
		if (!json_obj["context"].is_null())
		{
			state->context.reset();
		}
		else
		{
			state->context = context_t::from_json(json_obj["context"]);
		}
		state->currently_playing_type = json_obj["currently_playing_type"];
		state->device = playback_device_t::from_json(json_obj["device"]);
		state->is_playing = json_obj["is_playing"];
		state->item = track_t::from_json(json_obj["item"]);
		state->progress_ms = json_obj["progress_ms"];
		state->repeat_state = json_obj["repeate_state"];
		state->shuffle_state = json_obj["shuffle_state"];
		state->timestamp = json_obj["timestamp"];
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return state;
}


std::unique_ptr<queue_t> queue_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<queue_t> queue_t::from_json(const json::json &json_obj)
{
	auto queue = std::make_unique<queue_t>();

	if (!json_obj["currently_playing"].is_null())
		queue->type = json_obj["currently_playing"]["type"];
	else if (!json_obj["queue"].empty())
		queue->type = json_obj["queue"][0]["type"];
	else
		return queue;

	if (queue->type == "track")
	{
		if (!json_obj["currently_playing"].is_null())
		{
			queue->current_item = std::make_shared<playable_type>(*track_t::from_json(json_obj["currently_playing"]));
		}
		
		for (auto track = json_obj["queue"].begin(); track != json_obj["queue"].end(); ++track)
		{
			queue->items.push_back(std::make_shared<playable_type>(*track_t::from_json(track.value())));
		}

		return queue;
	}
	else if (queue->type == "episode")
	{
		if (!json_obj["currently_playing"].is_null())
		{
			queue->current_item = std::make_shared<playable_type>(*episode_t::from_json(json_obj["currently_playing"]));
		}

		for (auto episode = json_obj["queue"].begin(); episode != json_obj["queue"].end(); ++episode)
		{
			queue->items.push_back(std::make_shared<playable_type>(*episode_t::from_json(episode.value())));
		}
		return queue;
	}
	else
	{
		return std::unique_ptr<queue_t>();
	}
}


std::unique_ptr<track_history_t> track_history_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<track_history_t> track_history_t::from_json(const json::json &json_obj)
{
	auto track_history = std::make_unique<track_history_t>();

	track_history->played_at = json_obj["played_at"];
	track_history->context = context_t::from_json(json_obj["context"]);
	track_history->track = track_t::from_json(json_obj["track"]);

	return track_history;
}


std::unique_ptr<recent_tracks_t> recent_tracks_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<recent_tracks_t> recent_tracks_t::from_json(const json::json &json_obj)
{
	auto recent_tracks = std::make_unique<recent_tracks_t>();
	try
	{
		recent_tracks->href = json_obj["href"];
		recent_tracks->cursor.before = json_obj["cursor"].value("before", -1);
		recent_tracks->cursor.after = json_obj["cursor"].value("after", -1);
		recent_tracks->limit = json_obj["limit"];
		recent_tracks->next = json_obj.value("next", "");
		recent_tracks->total = json_obj["total"];

		for (auto item = json_obj["items"].begin(); item != json_obj["items"].end(); ++item)
		{
			recent_tracks->items.push_back(track_history_t::from_json(item.value()));
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return recent_tracks;
};



std::unique_ptr<playback_state_t> Player_API::get_playback_state()
{
	http::api_response response = http::get(API_PREFIX "/me/player", std::string(""), this->access_token);

	if (response.code != 200) return std::unique_ptr<playback_state_t>(nullptr);
	return playback_state_t::from_json(response.body);
}

void Player_API::transfer_playback(const std::string &device_id, bool ensure_playback)
{
	json::json post_data = {
		{"device_ids", {device_id}},
		{"play", ensure_playback}
	};
	(void) http::request(API_PREFIX "/me/player", http::REQUEST_METHOD::METHOD_PUT, post_data.dump(), this->access_token, true);
}

std::vector<playback_device_t> Player_API::get_available_devices()
{
	http::api_response devices_string = http::get(API_PREFIX "/me/player/devices", std::string(""), this->access_token);
	std::vector<playback_device_t> devices = {};

	if (devices_string.code != 200) return devices;

	json::json devices_json = json::json::parse(devices_string.body)["devices"];
	for (auto device = devices_json.begin(); device != devices_json.end(); ++device)
	{
		devices.push_back(*playback_device_t::from_json(device.value()));
	}

	return devices;
}

std::unique_ptr<track_t> Player_API::get_currently_playing_track()
{
	http::api_response response = http::get(API_PREFIX "/me/player/currently-playing", std::string(""), this->access_token);
	if (response.code != 200) return std::unique_ptr<track_t>(nullptr);
	return track_t::from_json(response.body);
}

void Player_API::start_or_resume_playback(const std::string &context_uri, const std::vector<std::string> &uris, int offset, int position_ms)
{
	json::json put_data;
	if (context_uri != "") put_data["context_uri"] = context_uri;
	if (uris.size() > 0) put_data["uris"] = uris;
	put_data["position_ms"] = position_ms;

	auto response = http::request(API_PREFIX "/me/player/play", http::REQUEST_METHOD::METHOD_PUT, put_data.dump(), this->access_token, true);
	std::cout << "Code: " << response.code << "\nBody: " << response.body << std::endl;
}

void Player_API::pause_playback()
{
	http::request(API_PREFIX "/me/player/pause", http::REQUEST_METHOD::METHOD_PUT, std::string(), this->access_token, true);
}

void Player_API::seek_to_position(int position_ms)
{
	json::json put_data = {{"position_ms", position_ms}};
	http::request(API_PREFIX "/me/player/seek", http::REQUEST_METHOD::METHOD_PUT, put_data.dump(), this->access_token, true);
}

void Player_API::set_repeat_mode(Player_API::REPEAT_MODE state)
{
	json::json put_data;

	switch (state)
	{
	using enum Player_API::REPEAT_MODE;
	case TRACK:
		put_data["state"] = "track";
		break;
	
	case CONTEXT:
		put_data["state"] = "context";
		break;

	case OFF:
	default:
		put_data["state"] = "off";
		break;
	}
	http::request(API_PREFIX "/me/player/repeat", http::REQUEST_METHOD::METHOD_PUT, put_data.dump(), this->access_token, true);
}

void Player_API::set_volume(int volume_percent)
{
	json::json put_data = {{"volume_percent", std::clamp(volume_percent, 0, 100)}};
	http::request(API_PREFIX "/me/player/volume", http::REQUEST_METHOD::METHOD_PUT, put_data.dump(), this->access_token, true);
}

void Player_API::set_shuffle(bool state)
{
	json::json put_data = {{"state", state}};
	http::request(API_PREFIX "/me/player/shuffle", http::REQUEST_METHOD::METHOD_PUT, put_data.dump(), this->access_token, true);
}

std::unique_ptr<recent_tracks_t> Player_API::get_recently_played_tracks(int limit = 20, unsigned int timestamp = 0, bool after = false)
{
	auto recent_tracks = std::make_unique<recent_tracks_t>();

	std::string query_data = "limit=" + std::to_string(std::clamp(limit, 0, 50));
	
	if (timestamp > 0) {
		query_data += "&" + std::string(after ? "after" : "before") + "=" + std::to_string(timestamp);
	}
	
	auto response = http::get(API_PREFIX "/me/player/recently-played", query_data, this->access_token);

	if (response.code != 200) return recent_tracks;

	return recent_tracks_t::from_json(response.body);
}

std::unique_ptr<queue_t> Player_API::get_queue()
{
	auto response = http::get(API_PREFIX "/me/player/queue", std::string(), this->access_token);
	if (response.code != 200) return std::unique_ptr<queue_t>(nullptr);
	return queue_t::from_json(response.body);
}

void Player_API::add_item_to_playback_queue(const std::string &item_uri)
{
	std::string url = API_PREFIX "/me/player/queue?uri=";
	url += http::url_encode(item_uri);

	auto response = http::post(url.c_str(), std::string(), this->access_token, true);
}

} // namespace spotify_api
