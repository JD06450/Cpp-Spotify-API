#pragma once
#ifndef _SPOTIFY_API_ALBUM_T_
#define _SPOTIFY_API_ALBUM_T_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <cstdint>
#include <memory>

#include <nlohmann/json.hpp>

#include "./common.hpp"
#include "./artists.hpp"
#include "./tracks.hpp"

namespace spotify_api
{

struct artist_t;

struct copyright_t
{
	std::string text;
	std::string type;
};

/**
 * @brief A container for storing all data associated with a Spotify album object as returned from the associated
 * [API endpoints](https://developer.spotify.com/documentation/web-api/reference/get-an-album).
 */
struct album_t
{
	/// Specifies the type of an album, such as a compilation, a single, or a normal album.
	std::string album_type;
	/// The total number of tracks that an album contains.
	unsigned int total_tracks;
	/**
	 * @brief A list of markets that an album is available in, where a market typically refers to a country
	 * and is specified by its corresponding 2 letter country code.
	 * @note An album is considered available in a market when at least one of its tracks is available in that market.
	*/
	std::vector<std::string> available_markets;
	/**
	 * @brief Any known external urls for the album.
	 * @note This will always include the album's [Spotify URL](https://developer.spotify.com/documentation/web-api/concepts/spotify-uris-ids).
	*/
	std::map<std::string, std::string> external_urls;
	/// A Spotify API endpoint that provides the full details of an album.
	std::string href;
	/// The [Spotify ID](https://developer.spotify.com/documentation/web-api/concepts/spotify-uris-ids) of the album.
	std::string id;
	/// Cover art for an album in various sizes.
	std::vector<image_t> images;
	/// The name of the album.
	std::string name;
	/// The date in which the album was released.
	std::string release_date;
	/// The precision to which the release date is known. Can either be "day", "month", or "year".
	std::string release_date_precision;
	/// Any restrictions that may prevent an album from being played for the current logged-in user.
	/// (i.e. not available in the user's market; the album contains explicit content which the user has chosen to filter out)
	std::map<std::string, std::string> restrictions;
	/// Any copyrights associated with the album.
	std::vector<copyright_t> copyrights;
	static const std::string type;
	/// The [Spotify URI](https://developer.spotify.com/documentation/web-api/concepts/spotify-uris-ids) of the album.
	std::string uri;
	/// Any known external IDs for a particular album, such as a UPC, an EAN, and/or an ISRC.
	std::map<std::string, std::string> external_ids;
	std::vector<std::string> genres;
	/// A number from 0-100 describing how popular an album is.
	uint8_t popularity;
	std::string label;
	/// A list of artists who made the album.
	std::vector<std::shared_ptr<artist_t>> artists;
	/// A list of tracks in the album as a @ref page_t "page".
	page_t<std::shared_ptr<track_t>> tracks;
	
	/**
	 * @brief Converts a stringified JSON object to an album object.
	 * 
	 * Calls @ref from_json(const nlohmann::json &) after converting the string to a json object.
	 * @param json_string The stringified JSON object to convert. The expected format of the json object
	 * can be found [here](https://developer.spotify.com/documentation/web-api/reference/get-an-album).
	 * @returns A pointer to a newly created album object
	 * 
	 * @see from_json(const nlohmann::json &json_object)
	*/
	static std::unique_ptr<album_t> from_json(const std::string &json_string);

	/**
	 * @brief Converts a json object into a new @ref album_t instance
	 * @param json_object The json object to convert
	 * @returns A pointer to a newly created @ref album_t object
	 * 
	 * @see from_json(const std::string &json_string)
	 */
	static std::unique_ptr<album_t> from_json(const nlohmann::json &json_object);
};

} // namespace spotify_api

#endif