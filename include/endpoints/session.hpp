#pragma once
#ifndef _SPOTIFY_API_SESSION_
#define _SPOTIFY_API_SESSION_
#endif

#ifndef _SPOTIFY_API_SESSION_

#include <string>
#include <map>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>

#include "../curl-util.hpp"

namespace spotify_api
{
	struct api_token_response
	{
		std::string access_token;
		std::string token_type;
		int expires_in;
		std::string refresh_token;
	};	

	class Session_API
	{
	private:
		std::mutex _access_mutex;
		std::string _access_token;
		std::string _refresh_token;
		std::string _base64_client_id_secret;
		unsigned long int _token_grant_time;
		unsigned long int _token_expiration_time;
		
		std::atomic<bool> _refresh_done;
		bool _refresh_cycle_started;
		std::thread * _refresh_thread;

	public:
		Session_API(api_token_response &token_obj);
		Session_API(std::string json_string);

		bool refresh_access_token();
		void new_refresh_thread(int wait_duration = 0);
		void set_base64_id_secret(std::string &new_value);
		std::string get_access_token();
	};

} // namespace spotify_api


#endif