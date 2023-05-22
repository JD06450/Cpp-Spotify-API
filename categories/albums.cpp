#include <iostream>
#include "albums.hpp"

namespace json = nlohmann;

namespace spotify_api
{

album_full_t *Album_API::get_album(const std::string &album_id)
{
    album_full_t *retval = new album_full_t;
    std::string url = API_PREFIX "/albums/";
    url += album_id;

    http::api_response response = http::get(url.c_str(), std::string(""), this->_access_token);
    if (response.code == 200)
    {
        object_from_json(response.body, retval);
    }
    return retval;
}

std::vector<album_t> Album_API::get_albums(const std::vector<std::string> &album_ids)
{
    std::vector<album_t> albums;
    albums.reserve(album_ids.size());
    std::string query_string = "";
    size_t num_batches = ceil(album_ids.size() / 20);

    for (size_t batch = 0; batch < num_batches; batch++)
    {
        size_t batch_size = batch * 20 + 20 < album_ids.size() ? 20 : album_ids.size() - (batch * 20 + 20);
        query_string = "ids=";
        for (size_t i = batch * 20; i < (batch * 20) + batch_size; i++)
        {
            query_string += album_ids.at(i) + ",";
        }
        query_string = std::regex_replace(query_string, std::regex(",+$"), "");
        http::api_response batch_response = http::get(API_PREFIX "/albums", query_string, this->_access_token);
        if (batch_response.code == 200)
        {
            json::json albums_json = json::json::parse(batch_response.body)["albums"];
            for (auto new_album = albums_json.begin(); new_album != albums_json.end(); ++new_album)
            {
                album_t temp;
                object_from_json(new_album.value().dump(), &temp);
                albums.push_back(temp);
            }
        }
    }
    albums.shrink_to_fit();
    return albums;
}


void Album_API::object_from_json(const std::string &json_string, album_t *output)
{
    try
    {
        json::json json_object = json::json::parse(json_string);

        output->album_type = json_object["album_type"];

        json::json temp = json_object["artists"];
        // output->artists.reserve(temp.size());
        // for (auto artist = temp.begin(); artist != temp.end(); ++artist)
        // {
        //     spotify_api::artist_t *temp_artist = new spotify_api::artist_t;
        //     Artist_API::object_from_json(artist.value().dump(), temp_artist);
        //     output->artists.push_back(temp_artist);
        // }
        // output->artists.shrink_to_fit();

        temp = json_object["available_markets"];
        output->available_markets.reserve(temp.size());
        for (auto market = temp.begin(); market != temp.end(); ++market)
        {
            output->available_markets.push_back(market.value());
        }
        output->available_markets.shrink_to_fit();

        temp = json_object["external_urls"];
        for (auto ext_url = temp.begin(); ext_url != temp.end(); ++ext_url)
        {
            output->external_urls.insert(std::pair<std::string, std::string>(ext_url.key(), ext_url.value().get<std::string>()));
        }

        output->href = json_object["href"];

        output->id = json_object["id"];

        temp = json_object["images"];
        output->images.reserve(temp.size());
        for (auto image = temp.begin(); image != temp.end(); ++image)
        {
            image_t temp_image;
            temp_image.url = image.value()["url"];
            temp_image.width = image.value()["width"];
            temp_image.height = image.value()["height"];
            output->images.push_back(temp_image);
        }
        output->images.shrink_to_fit();

        output->name = json_object["name"];

        output->release_date = json_object["release_date"];

        output->release_date_precision = json_object["release_date_precision"];

        output->total_tracks = json_object["total_tracks"];

        output->uri = json_object["uri"];
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        fprintf(stderr, "\033[31mspotify-api/endpoints.cpp: object_from_json(): Error: Failed to convert json data to album.\nJson string: %s\nNow exiting.\n\033[39m", json_string.c_str());
        exit(1);
    }
}

void Album_API::object_from_json(const std::string &json_string, album_full_t *output)
{
    object_from_json(json_string, (album_t *) output);
    json::json json_object = json::json::parse(json_string);
    
    if (json_object.contains("tracks"))
    {
        json::json json_tracks = json_object["tracks"];
        batch_t<track_t> temp_track;
        temp_track.href = json_tracks["href"];
        temp_track.limit = json_tracks["limit"];
        temp_track.offset = json_tracks["offset"];
        output->tracks.total = json_tracks["total"];
        if (!json_tracks["next"].is_null())
            output->tracks.next = json_tracks["next"];
        if (!json_tracks["previous"].is_null())
            output->tracks.previous = json_tracks["previous"];
        json_tracks = json_tracks["items"];
        output->tracks.items.reserve(json_tracks.size());
        for (auto track = json_tracks.begin(); track != json_tracks.end(); ++track)
        {
            spotify_api::track_t *temp = new spotify_api::track_t;
            Track_API::object_from_json(track.value().dump(), temp);
            output->tracks.items.push_back(temp);
        }
    }
    output->tracks.items.shrink_to_fit();
}

} // namespace spotify_api