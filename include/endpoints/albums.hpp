#pragma once
#ifndef _SPOTIFY_API_ALBUMS_
#define _SPOTIFY_API_ALBUMS_

#include <string>
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <cstdint>
#include <memory>

#include <nlohmann/json.hpp>

#include "../categories/common.hpp"
#include "../categories/albums.hpp"
#include "../categories/tracks.hpp"

namespace spotify_api
{

/**
 * @brief A collection of API wrapper functions related to Spotify albums.
*/
class Album_API
{
	public:
	std::string access_token;

	Album_API(std::string access_token): access_token(access_token) {}

	/**
	 * @brief Retrieves the information of an album from Spotify using an album ID.
	 * @param album_id The [Spotify ID](https://developer.spotify.com/documentation/web-api/concepts/spotify-uris-ids) of the album to retrieve
	 * @note Endpoint: /albums/{album_id}
	 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/get-an-album
	 * @returns The album as an @ref album_t instance.
	 */
	std::unique_ptr<album_t> get_album(const std::string &album_id);
	
	/**
	 * @brief Retrieves info on multiple albums from Spotify using their IDs.
	 * @param album_ids The [Spotify IDs](https://developer.spotify.com/documentation/web-api/concepts/spotify-uris-ids) of the albums to retrieve
	 * @note Endpoint: /albums
	 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/get-multiple-albums
	 * @returns A list of @ref album_t "albums".
	*/
	std::vector<std::unique_ptr<album_t>> get_albums(const std::vector<std::string> &album_ids);

	/**
	 * @brief Gets tracks associated with an album using its ID.
	 * @param album_id The [Spotify ID](https://developer.spotify.com/documentation/web-api/concepts/spotify-uris-ids) of the album
	 * @param limit The maximum number of items to return. Range: 0 - 50
	 * @param offset The index of the first item to return.
	 * @param market An ISO 3166-1 alpha-2 country code. If a country code is specified, only content that is available in that market will be returned.
	 * If a valid user access token is specified in the request header, the country associated with the user account will take priority.
	 * @note Endpoint: /albums/{album_id}/tracks
	 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/get-an-albums-tracks
	 * @returns A page of @ref track_t "tracks" in the album.
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
	 * @returns A page of @ref album_t "albums" that the user has saved.
	*/
	page_t<std::unique_ptr<album_t>> get_users_albums(uint32_t limit = 20, uint32_t offset = 0, const std::string &market = "");

	/**
	 * @brief Saves a list of albums to the user's library by their IDs.
	 * @param album_ids The [Spotify IDs](https://developer.spotify.com/documentation/web-api/concepts/spotify-uris-ids)
	 * of the albums to save in the user's library. A maximum of 20 IDs are allowed.
	 * @note Endpoint: /me/albums
	 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/save-albums-user
	*/
	void save_albums_for_current_user(const std::vector<std::string> &album_ids);

	/**
	 * @brief Removes the specified albums from the user's library by their IDs.
	 * @param album_ids The [Spotify IDs](https://developer.spotify.com/documentation/web-api/concepts/spotify-uris-ids)
	 * of the albums to remove from the user's library. A maximum of 20 IDs are allowed.
	 * @note Endpoint: /me/albums
	 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/remove-albums-user
	 */
	void remove_saved_albums_for_current_user(const std::vector<std::string> &album_ids);

	/**
	 * @brief Searches through the user's saved album library to check whether the list contains the specified albums.
	 * @param album_ids The [Spotify IDs](https://developer.spotify.com/documentation/web-api/concepts/spotify-uris-ids)
	 * of the albums to look for. A maximum of 20 IDs are allowed.
	 * @note Endpoint: /me/albums/contains
	 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/check-users-saved-albums
	 * @returns A list of album IDs paired with a value stating whether or not that ID was found.
	*/
	std::map<std::string, bool> check_users_saved_albums(const std::set<std::string> &album_ids);

	/**
	 * @brief Gets a paged list of new albums released on Spotify.
	 * @param limit The maximum number of items to return. Range: 0 - 50
	 * @param offset The index of the first item to return.
	 * @param market An ISO 3166-1 alpha-2 country code. If a country code is specified, only content that is available in that market will be returned.
	 * If a valid user access token is specified in the request header, the country associated with the user account will take priority.
	 * @note Endpoint: /browse/new-releases
	 * @note Docs: https://developer.spotify.com/documentation/web-api/reference/get-new-releases
	 * @returns A list containing information about the new albums.
	 */
	page_t<std::unique_ptr<album_t>> get_new_releases(uint32_t limit = 20, uint32_t offset = 0, const std::string &country = "");
};

} // namespace spotify_api

#endif
