#pragma once
#ifndef _SPOTIFY_API_EPISODES_
#define _SPOTIFY_API_EPISODES_

#include <map>
#include <string>
#include <vector>
#include <optional>
#include <memory>

#include "./common.hpp"
#include "./shows.hpp"

#include <nlohmann/json.hpp>

namespace spotify_api
{

class Episode_API
{
	public:
	std::string access_token;
	Episode_API(std::string access_token): access_token(access_token) {}
};

} // namespace spotify_api


#endif