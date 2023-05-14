#include "common.hpp"

namespace json = nlohmann;

std::map<std::string, std::string> json_iterate_map(const json::json &json_map)
{
    std::map<std::string, std::string> output;
    for (auto item = json_map.begin(); item != json_map.end(); ++item)
    {
        output.insert(std::pair<std::string, std::string>(item.key(), item.value().get<std::string>()));
    }
    return output;
}