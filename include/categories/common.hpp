#define API_PREFIX "https://api.spotify.com/v1"

#pragma once
#ifndef _SPOTIFY_API_CATEGORY_COMMON_INCLUDES_
#define _SPOTIFY_API_CATEGORY_COMMON_INCLUDES_

#include <string>
#include <map>
#include <vector>
#include <type_traits>
#include <memory>
#include <concepts>

#include <nlohmann/json.hpp>

#define json_get_nullable(key, def) key.is_null() ? def : key

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
	int width = -1;
	int height = -1;

	/**
	 * @brief Converts an `nlohmann::json` json object into an @ref image_t object.
	 * @param json_object The json object to be converted. his json object shall have
	 * all of the member variables of the @ref image_t struct.
	 * @returns A new instance of the @ref image_t struct.
	 * 
	 * @sa from_json(const std::string &json_string)
	 */
	static image_t from_json(const nlohmann::json &json_object);

	/**
	 * @brief Converts a stringified json object to an @ref image_t object.
	 * 
	 * Calls @ref from_json(const nlohmann::json &json_object) after parsing the `json_string`
	 * into an `nlohmann::json` json object.
	 * @param json_string The stringified json object to convert. This json object shall have
	 * all of the member variables of the @ref image_t struct.
	 * @returns A new instance of the @ref image_t struct.
	 * 
	 * @sa from_json(const nlohmann::json &json_object)
	 */
	static inline image_t from_json(const std::string &json_string) { return image_t::from_json(nlohmann::json::parse(json_string)); };
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



namespace item_type
{
	static const std::string ALBUM = "album";
	static const std::string ARTIST = "artist";
	static const std::string AUDIOBOOK = "audiobook";
	static const std::string EPISODE = "episode";
	static const std::string PLAYLIST = "playlist";
	static const std::string TRACK = "track";
	static const std::string SHOW = "show";
	static const std::string USER = "user";
}

// std::remove_pointer does not work on smart pointers.
// Why the fuck does it not work on smart pointers?
// Now I have to use this which just seems stupid.
// And no, I will not add overloads to std::remove_pointer.
// I do not want to fuck around with stdlib, because I don't feel like finding out.

template<class T> struct RemovePointer { typedef T type; };

template<class T> struct RemovePointer<T*> { typedef T type; };
template<class T> struct RemovePointer<T* const> { typedef T type; };
template<class T> struct RemovePointer<T* volatile> { typedef T type; };
template<class T> struct RemovePointer<T* const volatile> { typedef T type; };

template<class T> struct RemovePointer<std::unique_ptr<T>> { typedef T type; };
template<class T> struct RemovePointer<std::unique_ptr<T> const> { typedef T type; };
template<class T> struct RemovePointer<std::unique_ptr<T> volatile> { typedef T type; };
template<class T> struct RemovePointer<std::unique_ptr<T> const volatile> { typedef T type; };

template<class T> struct RemovePointer<std::shared_ptr<T>> { typedef T type; };
template<class T> struct RemovePointer<std::shared_ptr<T> const> { typedef T type; };
template<class T> struct RemovePointer<std::shared_ptr<T> volatile> { typedef T type; };
template<class T> struct RemovePointer<std::shared_ptr<T> const volatile> { typedef T type; };

template<class T> struct RemovePointer<std::weak_ptr<T>> { typedef T type; };
template<class T> struct RemovePointer<std::weak_ptr<T> const> { typedef T type; };
template<class T> struct RemovePointer<std::weak_ptr<T> volatile> { typedef T type; };
template<class T> struct RemovePointer<std::weak_ptr<T> const volatile> { typedef T type; };

template<class T>
using RemovePointer_T = typename RemovePointer<T>::type;

template<class T>
concept HasFromJson = requires(T a) {
	{ T::from_json(std::string()) } -> std::same_as<std::unique_ptr<T>>;
	{ T::from_json(nlohmann::json()) } -> std::same_as<std::unique_ptr<T>>;
};

template<class T>
concept HasFromJsonPointer = requires(T t) {
	requires HasFromJson<std::remove_reference_t<decltype(*t)>>;
};

template<class T>
concept JsonOrJsonPointer = HasFromJson<T> || HasFromJsonPointer<T>;

/**
 * @brief A special type of list that Spotify uses as an alternative to returning an entire list all at once.
 * A page will contain a portion of a list if that list is longer than the page's item `limit`
 * starting at the specified `offset`.
 * and is used presumably to save on potentially unnecessary network traffic and RAM usage.
 * @tparam Item_Type The type of the item to store. (i.e. @ref track_t, @ref album_t, @ref episode_t, etc...)
 */
template <JsonOrJsonPointer Item_Type>
struct page_t
{
	/// A copy of the Spotify API endpoint that was used to generate a page with all unused optional request parameters
	/// preset to their default values.
	std::string href;
	/// The list of items that a page contains which has a maximum size of @ref page_t::limit "limit".
	std::vector<Item_Type> items;
	/// The maximum number of items that a page will contain with a maximum value of 50.
	int limit;
	/// A Spotify API endpoint that returns the next page of the list.
	std::string next;
	/// An offset into the complete list in which a page starts its enumeration.
	int offset;
	/// A Spotify API endpoint that returns the previous page of the list.
	std::string previous;
	/// The total number of items in the complete list.
	int total;

	/**
	 * @brief Parses a stringified json object and converts it into a `page_t` object containing the `Item_Type`.
	 * @param json_string
	 * @param item_func A function to parse an `Item_Type` from its JSON format.
	 */
	static page_t<Item_Type> from_json(const std::string &json_string);

	static page_t<Item_Type> from_json(const nlohmann::json &json_obj);
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

#include "categories/common.tpp"

#endif