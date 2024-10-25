#include "categories/shows.hpp"
#include "endpoints/shows.hpp"

namespace json = nlohmann;

namespace spotify_api
{

const std::string show_t::type = item_type::SHOW;

std::unique_ptr<show_t> show_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<show_t> show_t::from_json(const json::json &json_obj)
{
	return std::unique_ptr<show_t>();
}

} // namespace spotify_api
