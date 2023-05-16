#pragma once
#ifndef _SPOTIFY_API_SESSION_
#define _SPOTIFY_API_SESSION_

#include <string>
#include <map>
#include <vector>
#include <atomic>
#include <thread>
#include <iostream>
// #include <ctime>
// #include <chrono>
// #include <future>
#include <unistd.h>
#include <nlohmann/json.hpp>

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
		std::atomic<std::string> _access_token;
		std::string _refresh_token;
		std::string _base64_client_id_secret;
		unsigned long int _token_grant_time;
		unsigned long int _token_expiration_time;
		
		std::atomic<bool> _refresh_done;
		bool _refresh_cycle_started;
		std::thread * _refresh_thread;
		void _new_refresh_thread(int wait_duration = 0);

	public:
		Session_API(api_token_response &token_obj);
		Session_API(std::string json_string);

		const std::atomic<std::string> &get_atomic_token() const;
		void refresh_access_token();
		void set_base64_id_secret(std::string &new_value);

	};

} // namespace spotify_api


#endif