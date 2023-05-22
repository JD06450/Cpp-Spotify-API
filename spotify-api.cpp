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

	this->session_api = new Session_API(response_object);
	this->session_api->set_base64_id_secret(client_keys_base64);

	this->_access_token = response_object.access_token;
	this->album_api = new Album_API(response_object.access_token);
	this->artist_api = new Artist_API(response_object.access_token);
	this->episode_api = new Episode_API(response_object.access_token);
	this->player_api = new Player_API(response_object.access_token);
	this->playlist_api = new Playlist_API(response_object.access_token);
	this->track_api = new Track_API(response_object.access_token);
}
	
	void Spotify_API::resync_access_token()
	{
		std::string temp_token = this->session_api->get_current_token();

		if (this->_access_token == temp_token) return;

		this->_access_token = temp_token;

		this->album_api->_access_token = this->_access_token;
		this->artist_api->_access_token = this->_access_token;
		this->episode_api->_access_token = this->_access_token;
		this->player_api->_access_token = this->_access_token;
		this->playlist_api->_access_token = this->_access_token;
		this->track_api->_access_token = this->_access_token;
	}

	Spotify_API::~Spotify_API()
	{
		delete this->album_api;
		delete this->artist_api;
		delete this->episode_api;
		delete this->player_api;
		delete this->playlist_api;
		delete this->session_api;
		delete this->track_api;
	}
} // namespace spotify_api
