#include "session.hpp"

namespace json = nlohmann;

namespace spotify_api
{
	Api_Session *start_spotify_session(std::string &auth_code, const std::string &redirect_uri, std::string &client_keys_base64)
	{
		std::string form_data =
			"grant_type=authorization_code"
			"&code=" +
			http::url_encode(auth_code) +
			"&redirect_uri=" + http::url_encode(redirect_uri);

		http::api_response response_data = http::post("https://accounts.spotify.com/api/token", form_data, client_keys_base64, false);
		std::cout << "token response" << std::endl;
		api_token_response response_object;
		try
		{
			json::json response_json = json::json::parse(response_data.body);
			response_object.access_token = response_json["access_token"];
			response_object.token_type = response_json["token_type"];
			response_object.expires_in = response_json["expires_in"];
			response_object.refresh_token = response_json["refresh_token"];
		}
		catch (const std::exception &e)
		{
			fprintf(stderr, "\033[31mspotify-api.cpp: get_auth_token(): Error: Failed to parse response data from Spotify.\nApi Response: %s.\nNow exiting.\n\033[39m", response_data.body.c_str());
			exit(1);
		}
		Api_Session *new_session = new Api_Session(response_object);
		new_session->set_base64_id_secret(client_keys_base64);
		return new_session;
	}

	Api_Session::Api_Session(api_token_response &token_obj)
	{
		this->_access_token = token_obj.access_token;
		this->_refresh_token = token_obj.refresh_token;
		this->_token_grant_time = time(NULL);
		this->_token_expiration_time = this->_token_grant_time + token_obj.expires_in;
		this->_refresh_done = true;

		this->_new_refresh_thread();
		std::cout << "past setTimeout" << std::endl;
	}

	void Api_Session::_new_refresh_thread(int wait_duration)
	{
		puts("Creating a new refresh thread...");
		int timeout_duration = wait_duration = 0 ? this->_token_expiration_time - this->_token_grant_time : wait_duration;
		// timeout_duration = 120;
		
		// We want to check whether this function was called before the current access token should've expired.
		// The code waits for a millisecond to give the other thread time to set the refresh_done flag then it checks the flag.
		// If the flag is set, then it is safe to create a new thread. Otherwise we destroy the old thread and create a new one in its place.
		// Essentially resetting the timer;
		usleep(1000);
		if (this->_refresh_done == false)
		{
			delete this->_refresh_thread;
		}

		this->_refresh_done == false;
		this->_refresh_thread = new std::thread([this, timeout_duration] {
			sleep(timeout_duration - 60);
			this->refresh_access_token();
			this->_refresh_done = true;
		});
		this->_refresh_thread->detach();
	}

	void Api_Session::refresh_access_token()
	{
		std::string form_data =
			"grant_type=refresh_token"
			"&refresh_token=" +
			this->_refresh_token;

		http::api_response response_data = http::post("https://accounts.spotify.com/api/token", form_data, this->_base64_client_id_secret, false);

		json::json response_json = json::json::parse(response_data.body);
		try
		{
			this->_access_token = response_json["access_token"];
			this->_token_grant_time = time(NULL);
			this->_token_expiration_time = this->_token_grant_time + response_json["expires_in"].get<unsigned int>();
			if (response_json.contains("refresh_token"))
			{
				this->_refresh_token = response_json["refresh_token"];
			}
		}
		catch (const std::exception &e)
		{
			fprintf(stderr, "\033[31mendpoints.cpp: refresh_access_token(): Error: Failed to parse response data from Spotify.\nApi Response: %s.\nTrying again in 60s\n\033[39m", response_data.body);
			this->_new_refresh_thread(60);
			return;
		}
		this->_new_refresh_thread();
	}

	void Api_Session::set_base64_id_secret(std::string &new_value)
	{
		this->_base64_client_id_secret = new_value;
	}

}