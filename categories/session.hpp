#pragma once
#ifndef _SPOTIFY_API_SESSION_
#define _SPOTIFY_API_SESSION_

#include <string>
#include <map>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>

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

	// Add ability to allow user to update access token manually.

	class Session_API
	{
		private:
		std::mutex _access_mutex;
		std::string _access_token;
		std::atomic<bool> _stop_loop;
		std::string _refresh_token;
		std::string _base64_client_id_secret;
		unsigned long int _token_grant_time;
		unsigned long int _token_expiration_time;
		
		/**
		 * @brief This represents the current background thread dedicated to refreshing the access token periodically
		 */
		std::thread * _refresh_thread;

		public:
		Session_API(api_token_response &token_obj);
		Session_API(std::string json_string);

		~Session_API();

		/**
		 * @brief Uses the refresh token provided by Spotify to generate a new access token
		 * @returns true if the token was successfully refreshed, false otherwise.
		*/
		bool refresh_access_token();

		/**
		 * @brief Retrieve the current value of the access token.
		 */
		const std::string get_current_token();
		
		/**
		 * @brief Kills the current refresh thread if it exists and spawns a new one.
		 */
		void new_refresh_thread();
		void set_base64_id_secret(std::string &new_value);

	};

} // namespace spotify_api


#endif