#pragma once
#ifndef _SPOTIFY_API_ALBUMS_
#define _SPOTIFY_API_ALBUMS_

#include <string>
#include <vector>
#include <map>
#include <regex>
#include <cstdint>
#include <memory>

#include "artists.hpp"
#include "tracks.hpp"
#include "common.hpp"

namespace spotify_api
{
	typedef struct {
		std::string text;
		std::string type;
	} copyright_t;

	typedef struct
	{
		std::string album_type;
		unsigned int total_tracks;
		std::vector<std::string> available_markets;
		std::map<std::string, std::string> external_urls;
		std::string href;
		std::string id;
		std::vector<image_t> images;
		std::string name;
		std::string release_date;
		std::string release_date_precision;
		std::map<std::string, std::string> restrictions;
		std::vector<copyright_t> copyrights;
		const std::string type = "album";
		std::string uri;
		std::map<std::string, std::string> external_ids;
		std::vector<std::string> genres;
		uint8_t popularity;
		std::string label;
		std::vector<artist_t *> artists;
		page_t<track_t *> tracks;
	} album_t;

	class Album_API
	{

		public:
		std::string access_token;	

		Album_API(std::string access_token): access_token(access_token) {}

		/**
		 * @brief Converts a stringified JSON object to an album object.
		 * @param json_string The stringified JSON object to convert
		 * @returns A pointer to a newly created album object
		*/
		static std::unique_ptr<album_t> object_from_json(const std::string &json_string);

		/**
		 * @brief Retrieves the information of an album from Spotify using an album ID.
		 * @param album_id The Spotify ID of the album to retrieve
		 * @note Endpoint: /albums/{album_id}
		 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/get-an-album
		 * @returns A new album object
		 */
		std::unique_ptr<album_t> get_album(const std::string &album_id);
		
		/**
		 * @brief Retrieves info on multiple albums from Spotify using their IDs.
		 * @param album_ids The Spotify IDs of the albums to retrieve
		 * @note Endpoint: /albums
		 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/get-multiple-albums
		 * @returns A vector containing pointers to new album objects
		*/
		std::vector<std::unique_ptr<album_t>> get_albums(const std::vector<std::string> &album_ids);

		/**
		 * @brief Gets the tracks associated with an album using its ID.
		 * @param album_id The Spotify ID of the album
		 * @param limit The maximum number of items to return. Range: 0 - 50
		 * @param offset The index of the first item to return.
		 * @param market An ISO 3166-1 alpha-2 country code. If a country code is specified, only content that is available in that market will be returned.
		 * If a valid user access token is specified in the request header, the country associated with the user account will take priority.
		 * @note Endpoint: /albums/{album_id}/tracks
		 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/get-an-albums-tracks
		 * @returns The cataloged info of each track in the album
		 */
		page_t<std::unique_ptr<track_t>> get_album_tracks(const std::string &album_id, uint32_t limit = 20, uint32_t offset = 0, const std::string &market = "");

		/**
		 * @brief Retrieves a list of albums that a user has saved in their "Your Music" library.
		 * @param limit The maximum number of items to return. Range: 0 - 50
		 * @param offset The index of the first item to return.
		 * @param market An ISO 3166-1 alpha-2 country code. If a country code is specified, only content that is available in that market will be returned.
		 * If a valid user access token is specified in the request header, the country associated with the user account will take priority.
		 * @note Endpoint: /me/albums
		 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/get-users-saved-albums
		 * @returns The list of albums that the user has saved
		*/
		page_t<std::unique_ptr<album_t>> get_users_albums(uint32_t limit = 20, uint32_t offset = 0, const std::string &market = "");

		/**
		 * @brief Saves a list of albums to the user's library.
		 * @param album_ids A list of albums specified by their Spotify ID to save in the user's library
		 * @note Endpoint: /me/albums
		 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/save-albums-user
		*/
		void save_albums_for_current_user(const std::vector<std::string> &album_ids);

		/**
		 * @brief Removes the specified albums from the user's library.
		 * @param album_ids The list of albums specified by their Spotify ID to remove from the user's library
		 * @note Endpoint: /me/albums
		 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/remove-albums-user
		 */
		void remove_saved_albums_for_current_user(const std::vector<std::string> &album_ids);

		/**
		 * @brief Searches through the user's saved album library to check whether the list contains the specified albums.
		 * @param album_ids The Spotify IDs of the albums to look for
		 * @note Endpoint: /me/albums/contains
		 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/check-users-saved-albums
		 * @returns A list of booleans that state whether or not an album was found in the library.
		 *          The order of the returned list will be the same as the list passed to this function.
		*/
		std::vector<bool> check_users_saved_albums(const std::vector<std::string> &album_ids);

		/**
		 * @brief Gets a paged list of new albums released on Spotify
		 * @param limit The maximum number of items to return. Range: 0 - 50
		 * @param offset The index of the first item to return.
		 * @param market An ISO 3166-1 alpha-2 country code. If a country code is specified, only content that is available in that market will be returned.
		 * If a valid user access token is specified in the request header, the country associated with the user account will take priority.
		 * @note Endpoint: /browse/new-releases
		 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/get-new-releases
		 * @returns A list containing information about the new albums
		 */
		page_t<std::unique_ptr<album_t>> get_new_releases(uint32_t limit = 20, uint32_t offset = 0, const std::string &country = "");
	};

} // namespace spotify_api

#endif
