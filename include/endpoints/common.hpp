#define API_PREFIX "https://api.spotify.com/v1"

#pragma once
#ifndef _SPOTIFY_API_CATEGORY_COMMON_ENDPOINTS_
#define _SPOTIFY_API_CATEGORY_COMMON_ENDPOINTS_

#include <string>
#include <map>
#include <vector>
#include <type_traits>
#include <memory>
// #include <cstddef>
#include <concepts>

#include "../categories/common.hpp"

#include <nlohmann/json.hpp>

namespace spotify_api
{

	// enum class ItemType
	// {
	// 	ALBUM,
	// 	ARTIST,
	// 	EPISODE,
	// 	PLAYLIST,
	// 	SHOW,
	// 	TRACK,
	// 	USER
	// };

	// inline std::string to_string(ItemType item)
	// {
	// 	switch (item)
	// 	{
	// 	case ItemType::ALBUM:
	// 		return item_type::ALBUM;
	// 	case ItemType::ARTIST:
	// 		return item_type::ARTIST;
	// 	case ItemType::EPISODE:
	// 		return item_type::EPISODE;
	// 	case ItemType::PLAYLIST:
	// 		return ";
	// 	case ItemType::SHOW:
	// 		return "show";
	// 	case ItemType::TRACK:
	// 		return "track";
	// 	default:
	// 		return "";
	// 	}
	// }

	// template <typename T>
	// struct HasEnumMember {
	// private:
	// 	// Helper function to detect the presence of a member with the specified type
	// 	template <typename U>
	// 	static constexpr auto check(U* u) -> decltype(u->enumMember, std::true_type{});

	// 	template <typename>
	// 	static constexpr std::false_type check(...);

	// public:
	// 	static constexpr bool value = decltype(check<T>(nullptr))::value;
	// };

} // namespace spotify_api

#endif