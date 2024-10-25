#include "categories/session.hpp"

#include <iostream>
#include <unistd.h>
#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{

	Session_API::Session_API(api_token_response &token_obj)
	{
		// This isn't necessary but I'm including the lock here anyway because
		// if a variable is guarded by a mutex then you should always lock it
		std::lock_guard<std::mutex> g_access_token(this->_access_mutex);
		this->_access_token = token_obj.access_token;
		this->_refresh_token = token_obj.refresh_token;
		this->_token_grant_time = time(NULL);
		this->_token_expiration_time = this->_token_grant_time + token_obj.expires_in;
		this->_refresh_done = true;
		this->_refresh_thread = nullptr;
		g_access_token.~lock_guard();

		this->new_refresh_thread();
		std::cout << "past setTimeout" << std::endl;
	}



	void Session_API::new_refresh_thread(int wait_duration)
	{
		puts("Creating a new refresh thread...");
		int timeout_duration = (wait_duration == 0) ? this->_token_expiration_time - this->_token_grant_time : wait_duration;
		
		// if the previous thread is still writing, then wait until it is done
		std::lock_guard<std::mutex> g_access_mutex(this->_access_mutex);

		if (this->_refresh_thread != nullptr && this->_refresh_thread->joinable())
		{
			delete this->_refresh_thread;
		}

		g_access_mutex.~lock_guard();

		this->_refresh_thread = new std::thread([this, timeout_duration] {
			bool refresh_successful = false;
			while (true)
			{
				sleep((refresh_successful ? (timeout_duration - 60) : 10));
				refresh_successful = this->refresh_access_token();
			}
		});
	}

	std::string Session_API::get_access_token()
	{
		std::lock_guard<std::mutex> g_access_token(this->_access_mutex);
		return this->_access_token;
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
			std::lock_guard<std::mutex> g_access_token(this->_access_mutex);
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
			fprintf(stderr, "\033[31mC++ Spotify API refresh_access_token(); Error: Failed to parse response data from Spotify.\nApi Response: %s.\nTrying again in 10s\n\033[39m", response_data.body);
			return false;
		}
		return true;
	}

	void Session_API::set_base64_id_secret(std::string &new_value)
	{
		this->_base64_client_id_secret = new_value;
	}

}