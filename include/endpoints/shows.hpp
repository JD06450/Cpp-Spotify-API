#pragma once
#ifndef _SPOTIFY_API_SHOWS_
#define _SPOTIFY_API_SHOWS_

#include <string>
#include <vector>
#include <optional>

namespace spotify_api
{

class Show_API
{
public:
	std::string access_token;
	Show_API(std::string access_token): access_token(access_token) {}
};

} // namespace spotify_api


#endif