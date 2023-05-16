#include "spotify-api.hpp"

namespace json = nlohmann;


namespace spotify_api
{

Spotify_API::Spotify_API(std::string &auth_code, const std::string &redirect_uri, std::string &client_keys_base64)
{
	std::string form_data =
		"grant_type=authorization_code"
		"&code=" +
		http::url_encode(auth_code) +
		"&redirect_uri=" + http::url_encode(redirect_uri);

	auto response = http::post("https://accounts.spotify.com/api/token", form_data, client_keys_base64, false);
	std::cout << "token response" << std::endl;
	api_token_response response_object;
	try
	{
		json::json response_json = json::json::parse(response.body);
		response_object.access_token = response_json["access_token"];
		response_object.token_type = response_json["token_type"];
		response_object.expires_in = response_json["expires_in"];
		response_object.refresh_token = response_json["refresh_token"];
	}
	catch (const std::exception &e)
	{
		fprintf(stderr, "\033[31mspotify-api.cpp: get_auth_token(): Error: Failed to parse response data from Spotify.\nApi Response: %s.\nNow exiting.\n\033[39m", response.body.c_str());
		exit(1);
	}
	this->_session_api = new Session_API(response_object);
	this->_session_api->set_base64_id_secret(client_keys_base64);

	const std::atomic<std::string> &atomic_token = this->_session_api->get_atomic_token();

	this->_access_token = response_object.access_token;
	this->_album_api = new Album_API(atomic_token);
	this->_artist_api = new Artist_API(atomic_token);
	this->_episode_api = new Episode_API(atomic_token);
	this->_player_api = new Player_API(atomic_token);
	this->_playlist_api = new Playlist_API(atomic_token);
}

} // namespace spotify_api
