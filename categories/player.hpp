#pragma once
#ifndef _SPOTIFY_API_PLAYER_
#define _SPOTIFY_API_PLAYER_

#include <string>
#include <variant>
#include <type_traits>

#include "tracks.hpp"
#include "episodes.hpp"
#include "../curl-util.hpp"

namespace spotify_api
{
	typedef struct
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
		
	} playback_device_t;
	std::unique_ptr<playback_device_t> parse_playback_device(const std::string &json_string);

	struct context_t
	{
		std::string type;
		std::string href;
		std::map<std::string, std::string> external_urls;
		std::string uri;
	};
	std::unique_ptr<context_t> parse_player_context(const std::string &json_string);

	// According to the spotify api docs, all of these options are technically "optional" in an api response,
	// but if any option is not present, then it shall be defaulted to false

	typedef struct
	{
		bool interrupting_playback;
		bool pausing;
		bool resuming;
		bool seeking;
		bool skipping_prev;
		bool skipping_next;
		bool toggling_repeat_context;
		bool toggling_shuffle;
		bool toggling_repeat_track;
		bool transfer_playback;
	} context_actions_t;
	std::unique_ptr<context_actions_t> parse_context_actions(const std::string &json_string);

	typedef struct
	{
		playback_device_t device;
		std::string repeat_state;
		bool shuffle_state;
		std::optional<context_t> context;
		// Unix timestamp when the data was fetched
		unsigned long timestamp;
		int progress_ms;
		bool is_playing;

		std::unique_ptr<track_t> item;

		std::string currently_playing_type;
		// A list of actions that can be performed and whether or not that action is allowed within the current context
		context_actions_t actions;
	} playback_state_t;
	std::unique_ptr<playback_state_t> parse_playback_state(const std::string &json_string);

	template <typename T, typename = void>
	struct HasFromJson : std::false_type {};

	template <typename T>
	struct HasFromJson<T, std::void_t<decltype(T::from_json())>> : std::true_type {};

	// template <typename Item_Type>
	struct queue_t
	{
		// static_assert(std::is_function<decltype(Item_Type::from_json)>::value, "Invalid Type");
		/**
		 * @brief specifies the type of items that are present in the queue. Currently only the values "track" and "episode" are allowed.
		*/
		std::string type;
		// std::optional<std::unique_ptr<T>> current_item;
		std::optional<std::unique_ptr<track_t>> current_track;
		std::optional<std::unique_ptr<episode_t>> current_episode;
		// std::optional<std::vector<std::unique_ptr<T>>> items;
		std::optional<std::vector<std::unique_ptr<track_t>>> tracks;
		std::optional<std::vector<std::unique_ptr<episode_t>>> episodes;

		static std::unique_ptr<queue_t> from_json(const std::string &json_string);
	};
	// std::unique_ptr<queue_t<track_t>> parse_queue(const std::string &json_string);

	struct track_history_t {
		std::unique_ptr<track_t> track;
		std::string played_at;
		context_t context;
	};

	struct recent_tracks_t
	{
	private:
		struct cursor_t
		{
			int before;
			int after;
		};
		
	public:
		std::string href;
		int limit;
		int total;
		std::string next;
		std::vector<track_history_t> items;
		
		cursor_t cursor;
		
		static std::unique_ptr<recent_tracks_t> from_json(const std::string &json_string);
	};
		
	class Player_API
	{
	public:
		std::string access_token;
		Player_API(std::string access_token): access_token(access_token) {}

		/**
		* @returns the user's current playback state
		* @note Endpoint: /me/player
		* @note Docs: https://developer.spotify.com/documentation/web-api/reference/#/operations/get-information-about-the-users-current-playback
		*/
		std::unique_ptr<playback_state_t> get_playback_state();
		
		/*
		* Usage: Transfers media playback to another device
		* Endpoint: /me/player
		* Parameters:
		* --  device_id: \<std::string> the id of the device where playback should be transferred to
		* --  play: bool: If true, then playback is ensured on the new device, otherwise the old state is used
		* Documentation: https://developer.spotify.com/documentation/web-api/reference/#/operations/transfer-a-users-playback
		*/
		void transfer_playback(const std::string &device_id, bool play);
		
		/*
		* Usage: Get information on the user's available devices
		* Endpoint: /me/player/devices
		* Parameters: none
		* Documentation: https://developer.spotify.com/documentation/web-api/reference/#/operations/get-a-users-available-devices
		*/
		std::vector<playback_device_t> get_available_devices();

		/*
		* Usage: Get the track the the user is currently playing
		* Endpoint: /me/player/currently-playing
		* Parameters: none
		* Documentation: https://developer.spotify.com/documentation/web-api/reference/#/operations/get-the-users-currently-playing-track
		*/
		std::unique_ptr<track_t> get_currently_playing_track();
		
		/*
		* Usage: Start a new context or resume current playback on the user's active device.
		* Endpoint: /me/player/play
		* Parameters:
		* --  context_uri: Spotify URI of the context to play. Valid contexts are albums, artists & playlists.
		* --  uris: A JSON array of the Spotify track URIs to play.
		* --  offset: A zero-based index that indicates from where in the context playback should start. Only available when context_uri corresponds to an album or playlist object
		* --  position_ms: Specifies where in the track to start playback
		* Documentation: https://developer.spotify.com/documentation/web-api/reference/start-a-users-playback
		*/
		void start_or_resume_playback(const std::string &context_uri, const std::vector<std::string> &uris, int offset, int position_ms);

		//TODO: create wrapper functions to automatically play specific stuff

		/*
		* Usage: Pause playback on the user's active device.
		* Endpoint: /me/player/pause
		* Parameters: none
		* Documentation: https://developer.spotify.com/documentation/web-api/reference/#/operations/pause-a-users-playback
		*/
		void pause_playback();

		/*
		* Usage: Skip playback to the previous song in the queue
		* Endpoint: /me/player/previous
		* Parameters: none
		* Returns: true if successful, false otherwise
		* Documentation: https://developer.spotify.com/documentation/web-api/reference/#/operations/skip-users-playback-to-previous-track
		*/
		bool inline skip_to_previous(const std::string &access_token)
		{
			http::api_response response = http::post(API_PREFIX "/me/player/previous", std::string(""), access_token, true);
			if (response.code == 204)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		/*
		* Usage: Skip playback to the next song in the queue
		* Endpoint: /me/player/previous
		* Parameters: none
		* Returns: true if successful, false otherwise
		* Documentation: https://developer.spotify.com/documentation/web-api/reference/#/operations/skip-users-playback-to-next-track
		*/
		bool inline skip_to_next(const std::string &access_token)
		{
			http::api_response response = http::post(API_PREFIX "/me/player/next", std::string(""), access_token, true);
			if (response.code == 204)
				return true;
			else
				return false;
		}

		/*
		 * Usage: Seeks to the given position in the user’s currently playing track.
		 * Endpoint: /me/player/seek
		 * Parameters:
		 * -- position_ms: The position in milliseconds to seek to. Must be a positive number. Passing in a position that is greater than the length of the track will cause the player to start playing the next song.
		 * Returns: void
		 * Documentation: https://developer.spotify.com/documentation/web-api/reference/seek-to-position-in-currently-playing-track
		*/
		void seek_to_position(int position_ms);
		
		enum class REPEAT_MODE
		{
			OFF,
			CONTEXT,
			TRACK
		};

		/*
		 * Usage: Set the repeat mode for the user's playback. Options are repeat-track, repeat-context, and off.
		 * Endpoint: /me/player/repeat
		 * Parameters:
		 * -- state: Can be either track, context or off. "track" will repeat the current track. "context" will repeat the current context. "off" will turn repeat off.
		 * Returns: void
		 * Documentation: https://developer.spotify.com/documentation/web-api/reference/seek-to-position-in-currently-playing-track
		*/
		void set_repeat_mode(REPEAT_MODE state);

		/*
		 * Usage: Set the volume for the user’s current playback device.
		 * Endpoint: /me/player/volume
		 * Parameters:
		 * -- volume_percent: The volume to set. Must be a value from 0 to 100 inclusive.
		 * Returns: void
		 * Documentation: https://developer.spotify.com/documentation/web-api/reference/set-volume-for-users-playback
		*/
		void set_volume(int volume_percent);

		/**
		 * @brief Toggle shuffle on or off for user’s playback.
		 * @param state the new shuffle state. true to shuffle, and false to not shuffle
		 * @note Endpoint: /me/player/suffle
		 * @note Documentation: https://developer.spotify.com/documentation/web-api/reference/toggle-shuffle-for-users-playback
		*/
		void set_shuffle(bool state);



		/**
		 * @brief Get tracks from the current user's recently played tracks. Note: Currently doesn't support podcast episodes.
		 * @param limit (Optional) The maximum number of items to return. Default: 20. Minimum: 1. Maximum: 50.
		 * @param timestamp (Optional) A unix timestamp. This will return all items before or after (but not including) this timestamp.
		 * @param after (Optional) If true, will return all items after the specified timestamp. If false, will return all items before the timestamp.
		 * @returns A list of tracks the user has recently played
		 * @note Endpoint: /me/player/recently-player
		 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/get-recently-played
		*/
		std::unique_ptr<recent_tracks_t> get_recently_played_tracks(int limit, unsigned int timestamp, bool after);

		/**
		 * @brief Get all items currently in the user's queue
		 * @returns A list of all items currently in the user's queue
		 * @note Endpoint: /me/player/queue
		 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/get-queue
		*/
		std::unique_ptr<queue_t> get_queue();

		/**
		 * @brief Adds the specified item to the user's playback queue
		 * @note Endpoint: /me/player/queue
		 * @param item_uri The Spotify URI of the item to add. Track and episode URIs allowed only.
		*/
		void add_item_to_playback_queue(const std::string &item_uri);
		
		/**
		 * @brief Use the search endpoint to search for a track
		 * @note Endpoint: /search
		 * @param q The search query
		 * @returns The track that best matches the query
		*/
		std::unique_ptr<track_t> search_for_track(const std::string &q);
		//TODO: support for all item types
	};

} // namespace spotify_api

#endif
