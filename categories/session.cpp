#include "session.hpp"

#include <iostream>
#include <unistd.h>

namespace json = nlohmann;

namespace spotify_api
{

	Session_API::Session_API(api_token_response &token_obj)
	{
		this->_access_token = token_obj.access_token;
		this->_refresh_token = token_obj.refresh_token;
		this->_token_grant_time = time(NULL);
		this->_token_expiration_time = this->_token_grant_time + token_obj.expires_in;

		this->_stop_loop = false;

		this->new_refresh_thread();
		std::cout << "past setTimeout" << std::endl;
	}

	void Session_API::new_refresh_thread()
	{
		puts("Creating a new refresh thread...");
		
		if (this->_refresh_thread->joinable())
		{
			this->_stop_loop = true;
			this->_refresh_thread->join();
		}

		this->_stop_loop = false;

		this->_refresh_thread = new std::thread([this] {
			bool retry = false;
			while (!this->_stop_loop) {
				int timeout_duration = !retry ? (this->_token_expiration_time - this->_token_grant_time - 60) : 60;
				sleep(timeout_duration);

				retry = !this->refresh_access_token();
			}
		});
	}

	bool Session_API::refresh_access_token()
	{
		std::string form_data =
			"grant_type=refresh_token"
			"&refresh_token=" +
			this->_refresh_token;

		http::api_response response_data = http::post("https://accounts.spotify.com/api/token", form_data, this->_base64_client_id_secret, false);

		json::json response_json = json::json::parse(response_data.body);
		try
		{
			std::lock_guard<std::mutex> g_access_mutex(this->_access_mutex);
			this->_access_token = response_json["access_token"].get<std::string>();
			g_access_mutex.~lock_guard();
			this->_token_grant_time = time(NULL);
			this->_token_expiration_time = this->_token_grant_time + response_json["expires_in"].get<unsigned int>();
			if (response_json.contains("refresh_token"))
			{
				this->_refresh_token = response_json["refresh_token"];
			}
		}
		catch (const std::exception &e)
		{
			fprintf(stderr, "\033[31mC++ Spotify API refresh_access_token(); Error: Failed to parse response data from Spotify.\nApi Response: %s.\n\033[39m", response_data.body);
			return false;
		}
		return true;
	}

	const std::string Session_API::get_current_token()
	{
		std::lock_guard<std::mutex> g_access_mutex(this->_access_mutex);
		return this->_access_token;
	}

	void Session_API::set_base64_id_secret(std::string &new_value)
	{
		this->_base64_client_id_secret = new_value;
	}

	Session_API::~Session_API() {
		this->_stop_loop = true;
		if (this->_refresh_thread->joinable()) this->_refresh_thread->join();
	}
}