#pragma once
#ifndef _SPOTIFY_API_SHOW_T_
#define _SPOTIFY_API_SHOW_T_

#include <string>
#include <vector>
#include <optional>

#include <nlohmann/json.hpp>

#include "./common.hpp"

namespace spotify_api
{

struct show_t
{
	static const std::string type;
	
	static std::unique_ptr<show_t> from_json(const std::string &json_string);
	
	static std::unique_ptr<show_t> from_json(const nlohmann::json &json_obj);
};

} // namespace spotify_api


#endif