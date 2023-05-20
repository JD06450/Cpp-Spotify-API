#pragma once
#ifndef _SPOTIFY_API_SHOWS_
#define _SPOTIFY_API_SHOWS_

#include <string>
#include <vector>
#include <optional>

namespace spotify_api
{
	struct show_t
	{
		const std::string type = "show";
	};

	class Show_API
	{
		public:
		std::string _access_token;
		void static object_from_json(const std::string &json_string, show_t *output);
	};
} // namespace spotify_api


#endif