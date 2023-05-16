#include "session.hpp"

namespace json = nlohmann;

namespace spotify_api
{

	Session_API::Session_API(api_token_response &token_obj)
	{
		this->_access_token = token_obj.access_token;
		this->_refresh_token = token_obj.refresh_token;
		this->_token_grant_time = time(NULL);
		this->_token_expiration_time = this->_token_grant_time + token_obj.expires_in;
		this->_refresh_done = true;

		this->_new_refresh_thread();
		std::cout << "past setTimeout" << std::endl;
	}

	const std::atomic<std::string> &Session_API::get_atomic_token() const {
		return this->_access_token;
	}

	void Session_API::_new_refresh_thread(int wait_duration = 0)
	{
		puts("Creating a new refresh thread...");
		int timeout_duration = (wait_duration == 0) ? this->_token_expiration_time - this->_token_grant_time : wait_duration;
		
		// We want to check whether this function was called before the current access token should've expired.
		// The code waits for a millisecond to give the other thread time to set the refresh_done flag then it checks the flag.
		// If the flag is set, then it is safe to create a new thread. Otherwise we destroy the old thread and create a new one in its place.
		// Essentially resetting the timer;
		usleep(1000);
		if (this->_refresh_done == false)
		{
			delete this->_refresh_thread;
		}

		this->_refresh_done = false;

		this->_refresh_thread = new std::thread([this, timeout_duration] {
			sleep(timeout_duration - 60);

			this->_refresh_done = true;
			this->refresh_access_token();
		});
	}

	void Session_API::refresh_access_token()
	{
		std::string form_data =
			"grant_type=refresh_token"
			"&refresh_token=" +
			this->_refresh_token;

		http::api_response response_data = http::post("https://accounts.spotify.com/api/token", form_data, this->_base64_client_id_secret, false);

		json::json response_json = json::json::parse(response_data.body);
		try
		{
			this->_access_token = response_json["access_token"].get<std::string>();
			this->_token_grant_time = time(NULL);
			this->_token_expiration_time = this->_token_grant_time + response_json["expires_in"].get<unsigned int>();
			if (response_json.contains("refresh_token"))
			{
				this->_refresh_token = response_json["refresh_token"];
			}
		}
		catch (const std::exception &e)
		{
			fprintf(stderr, "\033[31mC++ Spotify API refresh_access_token(); Error: Failed to parse response data from Spotify.\nApi Response: %s.\nTrying again in 60s\n\033[39m", response_data.body);
			this->_new_refresh_thread(60);
			return;
		}
		this->_new_refresh_thread();
	}

	void Session_API::set_base64_id_secret(std::string &new_value)
	{
		this->_base64_client_id_secret = new_value;
	}

}