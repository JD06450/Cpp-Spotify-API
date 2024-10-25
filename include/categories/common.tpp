#include "categories/common.hpp"
#include <iostream>

#ifndef _SPOTIFY_API_CATEGORY_COMMON_INCLUDES_
#error __FILE__ Shoule only be included from categories/common.hpp.
#endif



namespace spotify_api
{

template <JsonOrJsonPointer Item_Type>
page_t<Item_Type> page_t<Item_Type>::from_json(const std::string &json_string)
{
	return from_json(nlohmann::json::parse(json_string));
}

template <JsonOrJsonPointer Item_Type>
page_t<Item_Type> page_t<Item_Type>::from_json(const nlohmann::json &json_obj)
{
	page_t<Item_Type> new_page;
	
	new_page.href = json_obj["href"];
	new_page.limit = json_obj["limit"];
	new_page.offset = json_obj["offset"];
	new_page.total = json_obj["total"];
	new_page.next = json_get_nullable(json_obj["next"], "");
	new_page.previous = json_get_nullable(json_obj["previous"], "");
	
	for (auto item = json_obj["items"].begin(); item != json_obj["items"].end(); ++item)
	{
		new_page.items.push_back(RemovePointer_T<Item_Type>::from_json(item.value()));
	}
	
	return new_page;
}

}; // namespace spotify_api