#pragma once
#ifndef _SPOTIFY_API_SEARCH_ENDPOINTS_
#define _SPOTIFY_API_SEARCH_ENDPOINTS_

#include <concepts>
#include <type_traits>
#include <memory>

#include "categories/common.hpp"
#include "categories/search.hpp"

namespace spotify_api
{

class Search_API
{
public:
	std::string access_token;

	Search_API(std::string access_token);

	std::unique_ptr<search_result> search(search_type search_for_types, const std::string &query);
};

}

#endif