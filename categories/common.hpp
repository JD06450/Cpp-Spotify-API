#define API_PREFIX "https://api.spotify.com/v1"

#pragma once
#ifndef _SPOTIFY_API_CATEGORY_COMMON_INCLUDES_
#define _SPOTIFY_API_CATEGORY_COMMON_INCLUDES_

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace spotify_api
{
	struct follower_t
	{
		std::string href;
		int total;
	};

	struct image_t
	{
		std::string url;
		int width;
		int height;

		static image_t from_json(const std::string &json_string);
	};

	struct owner_t
	{
		std::map<std::string, std::string> external_urls;
		follower_t followers;
		std::string href = "";
		std::string id = "";
		const std::string type = "user";
		std::string uri = "";
		std::string display_name = "";
	};

	enum class ItemType
	{
		ALBUM,
		ARTIST,
		EPISODE,
		PLAYLIST,
		SHOW,
		TRACK,
		USER
	};

	std::string to_string(ItemType item)
	{
		switch (item)
		{
		case ItemType::ALBUM:
			return "album";	
		case ItemType::ARTIST:
			return "artist";	
		case ItemType::EPISODE:
			return "episode";	
		case ItemType::PLAYLIST:
			return "playlist";	
		case ItemType::SHOW:
			return "show";	
		case ItemType::TRACK:
			return "track";	
		default:
			return "";
		}
	}

	// template <typename T>
	// struct HasEnumMember {
	// private:
	// 	// Helper function to detect the presence of a member with the specified type
	// 	template <typename U>
	// 	static constexpr auto check(U* u) -> decltype(u->enumMember, std::true_type{});

	// 	template <typename>
	// 	static constexpr std::false_type check(...);

	// public:
	// 	static constexpr bool value = decltype(check<T>(nullptr))::value;
	// };

	template <typename Item_Type>
	struct page_t
	{
		std::string href = "";
		std::vector<Item_Type> items = {};
		int limit = 20;
		std::string next = "";
		int offset = 0;
		std::string previous = "";
		int total = 0;

		/**
		 * @brief Parses a stringified json object and converts it into a `page_t` object containing the `Item_Type`.
		 * @param json_string
		 * @param item_func A function to parse an `Item_Type` from its JSON format.
		 */
		static page_t<Item_Type> from_json(const std::string &json_string, Item_Type (*item_func)(const std::string &));
	};

	/**
	 * @brief Truncates a Spotify URI to just the Spotify ID of the resource at the end of the URI.
	 * Example: "spotify:track:6rqhFgbbKwnb9MLmUQDhG6" gets truncated to "6rqhFgbbKwnb9MLmUQDhG6"
	 * @param full_id 
	 * @note Docs: https://developer.spotify.com/documentation/web-api/concepts/spotify-uris-ids
	 */
	std::string truncate_spotify_uri(const std::string &full_id);

	/**
	 * @brief Truncates multiple Spotify URIs at once.
	 * See @ref truncate_spotify_uri for more info.
	 * @param full_ids 
	 * @param limit An optional maximum number of IDs to return
	 */
	std::vector<std::string> truncate_spotify_uris(const std::vector<std::string> &full_ids, size_t limit = 0);

} // namespace spotify_api

#endif