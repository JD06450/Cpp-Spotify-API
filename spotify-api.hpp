#ifndef _SPOTIFY_API_FILE_
#define _SPOTIFY_API_FILE_

#include <iostream>
#include <optional>
#include <unistd.h>
#include <chrono>
#include <string>
#include <future>
#include <thread>
#include <vector>
#include <regex>
#include <ctime>
#include <map>

#include <nlohmann/json.hpp>
#include "curl-util.hpp"

#include "categories/session.hpp"
#include "categories/common.hpp"
#include "categories/tracks.hpp"

namespace spotify_api
{
	// Authentication Process Step 2
	// Using the authentication code retrieved from the Spotify Web API, we send another request to generate an access token.
	Api_Session *start_spotify_session(std::string &auth_code, const std::string &redirect_uri, std::string &client_keys_base64);
} // namespace spotify_api

#endif