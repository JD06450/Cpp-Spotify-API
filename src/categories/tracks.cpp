#include <iostream>
#include "categories/tracks.hpp"
#include "endpoints/tracks.hpp"
#include "categories/albums.hpp"
#include "categories/artists.hpp"

#include <type_traits>

#include <nlohmann/json.hpp>

namespace json = nlohmann;

namespace spotify_api
{

const std::string track_t::type = item_type::TRACK;

std::unique_ptr<track_t> track_t::from_json(const std::string &json_string, bool parse_linked_from)
{
	return from_json(json::json::parse(json_string), parse_linked_from);
}

std::unique_ptr<track_t> track_t::from_json(const json::json &json_object, bool parse_linked_from)
{
	auto track = std::make_unique<track_t>();
	int step = 1;
	try
	{
		json::json json_obj = json_object;
		if (json_obj.contains("item"))
		{
			json_obj = json_obj["item"];
		}

		// not sure whether this check is needed
		if (json_obj.contains("album"))
		{
			track->album = album_t::from_json(json_obj["album"]);
		}

		json::json temp = json_obj["artists"];
		for (auto artist = temp.begin(); artist != temp.end(); ++artist)
		{
			track->artists.push_back(artist_t::from_json(artist.value().dump()));
		}
		track->artists.shrink_to_fit();

		step++;

		temp = json_obj["available_markets"];
		for (auto market = temp.begin(); market != temp.end(); ++market)
		{
			track->available_markets.push_back(market.value());
		}
		track->available_markets.shrink_to_fit();

		step++;

		track->disc_number = json_obj["disc_number"];
		step++;
		track->duration_ms = json_obj["duration_ms"];
		step++;

		temp = json_obj.value("external_ids", json::json::object());
		for (auto ext_id = temp.begin(); ext_id != temp.end(); ++ext_id)
		{
			track->external_ids.emplace(ext_id.key(), ext_id.value());
		}

		step++;

		temp = json_obj.value("external_urls", json::json::object());
		for (auto ext_url = temp.begin(); ext_url != temp.end(); ++ext_url)
		{
			track->external_urls.emplace(ext_url.key(), ext_url.value());
		}

		step++;

		track->href = json_obj["href"];
		step++;
		track->id = json_obj["id"];
		step++;
		track->is_explicit = json_obj["explicit"];
		step++;
		track->is_local = json_obj["is_local"];
		step++;
		track->is_playable = json_obj.value("is_playable", false);

		step++;

		if (json_obj.contains("linked_from") && parse_linked_from)
			track->linked_from.emplace(track_t::from_json(json_obj["linked_from"], false));
		else
			track->linked_from.reset();

		step++;

		track->name = json_obj["name"];
		step++;
		track->popularity = json_obj.value("popularity", 0);
		step++;
		track->preview_url = json_get_nullable(json_obj["preview_url"], "");
		step++;

		temp = json_obj.value("restrictions", json::json::object());
		for (auto restriction = temp.begin(); restriction != temp.end(); ++restriction)
		{
			track->restrictions.emplace(restriction.key(), restriction.value());
		}

		step++;

		track->track_number = json_obj["track_number"];
		step++;
		track->uri = json_obj["uri"];
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		fprintf(stderr, "\033[31mspotify-api.cpp: object_from_json(): Error: Failed to convert json data to track at step %u.\nJson string: %s\n\n\033[39m", step, json_object.dump(1, '\t').c_str());
		return std::unique_ptr<track_t>(nullptr);
	}

	return track;
}


std::unique_ptr<audio_features_t> audio_features_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<audio_features_t> audio_features_t::from_json(const json::json &json_obj)
{
	auto output = std::make_unique<audio_features_t>();
	
	try
	{
		output->acousticness = json_obj["acousticness"];
		output->analysis_url = json_obj["analysis_url"];
		output->danceability = json_obj["danceability"];
		output->duration_ms = json_obj["duration_ms"];
		output->energy = json_obj["energy"];
		output->id = json_obj["id"];
		output->instrumentalness = json_obj["instrumentalness"];
		output->key = json_obj["key"];
		output->liveness = json_obj["liveness"];
		output->loudness = json_obj["loudness"];
		output->mode = json_obj["mode"];
		output->speechiness = json_obj["speechiness"];
		output->tempo = json_obj["tempo"];
		output->time_signature = json_obj["time_signature"];
		output->track_href = json_obj["track_href"];
		output->uri = json_obj["uri"];
		output->valence = json_obj["valence"];
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return output;
}


std::unique_ptr<recommendation_seed_t> recommendation_seed_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<recommendation_seed_t> recommendation_seed_t::from_json(const json::json &json_obj)
{
	auto seed = std::make_unique<recommendation_seed_t>();

	seed->after_filtering_size = json_obj["afterFilteringSize"];
	seed->after_relinking_size = json_obj["afterRelinkingSize"];
	seed->href = json_obj["href"];
	seed->id = json_obj["id"];
	seed->initial_pool_size = json_obj["initialPoolSize"];
	seed->type = json_obj["type"];

	return seed;
}


template <HasFromJson T>
std::vector<std::shared_ptr<T>> parse_vector(const json::json &json_obj)
{
	std::vector<std::shared_ptr<T>> vec;
	for (auto item = json_obj.begin(); item != json_obj.end(); ++item)
	{
		vec.push_back(T::from_json(item.value()));
	}
	return vec;
}


std::unique_ptr<audio_analysis_t> audio_analysis_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<audio_analysis_t> audio_analysis_t::from_json(const nlohmann::json &json_obj)
{
	auto analysis = std::make_unique<audio_analysis_t>();

	analysis->meta = audio_analysis_t::meta_t::from_json(json_obj["meta"]);
	analysis->track = audio_analysis_t::track_t::from_json(json_obj["track"]);
	analysis->bars = parse_vector<audio_analysis_t::bar_t>(json_obj["bars"]);
	analysis->beats = parse_vector<audio_analysis_t::beat_t>(json_obj["beats"]);
	analysis->sections = parse_vector<audio_analysis_t::section_t>(json_obj["sections"]);
	analysis->segments = parse_vector<audio_analysis_t::segment_t>(json_obj["segments"]);
	analysis->tatums = parse_vector<audio_analysis_t::tatum_t>(json_obj["tatums"]);

	return analysis;
}


std::unique_ptr<audio_analysis_t::meta_t> audio_analysis_t::meta_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<audio_analysis_t::meta_t> audio_analysis_t::meta_t::from_json(const json::json &json_obj)
{
	auto meta = std::make_unique<audio_analysis_t::meta_t>();

	meta->analyzer_version = json_obj["analyzer_version"];
	meta->platform = json_obj["platform"];
	meta->detailed_status = json_obj["detailed_status"];
	meta->status_code = json_obj["status_code"];
	meta->timestamp = json_obj["timestamp"];
	meta->analysis_time = json_obj["analysis_time"];
	meta->input_process = json_obj["input_process"];

	return meta;
}


std::unique_ptr<audio_analysis_t::track_t> audio_analysis_t::track_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<audio_analysis_t::track_t> audio_analysis_t::track_t::from_json(const json::json &json_obj)
{
	auto track = std::make_unique<audio_analysis_t::track_t>();
	
	track->samples = json_obj["samples"];
	track->duration = json_obj["duration"];
	track->sample_md5 = json_obj["sample_md5"];
	track->offset_seconds = json_obj["offset_seconds"];
	track->window_seconds = json_obj["window_seconds"];
	track->analysis_sample_rate = json_obj["analysis_sample_rate"];
	track->analysis_channels = json_obj["analysis_channels"];
	track->end_of_fade_in = json_obj["end_of_fade_in"];
	track->start_of_fade_out = json_obj["start_of_fade_out"];
	track->loudness = json_obj["loudness"];
	track->tempo = json_obj["tempo"];
	track->tempo_confidence = json_obj["tempo_confidence"];
	track->time_signature = json_obj["time_signature"];
	track->time_signature_confidence = json_obj["time_signature_confidence"];
	track->key = json_obj["key"];
	track->key_confidence = json_obj["key_confidence"];
	track->mode = json_obj["mode"];
	track->mode_confidence = json_obj["mode_confidence"];
	track->codestring = json_obj["codestring"];
	track->code_version = json_obj["code_version"];
	track->echoprintstring = json_obj["echoprintstring"];
	track->echoprint_version = json_obj["echoprint_version"];
	track->synchstring = json_obj["synchstring"];
	track->synch_version = json_obj["synch_version"];
	track->rhythmstring = json_obj["rhythmstring"];
	track->rhythm_version = json_obj["rhythm_version"];
	
	return track;
}


std::unique_ptr<audio_analysis_t::bar_t> audio_analysis_t::bar_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<audio_analysis_t::bar_t> audio_analysis_t::bar_t::from_json(const json::json &json_obj)
{
	auto bar = std::make_unique<audio_analysis_t::bar_t>();

	bar->start = json_obj["start"];
	bar->duration = json_obj["duration"];
	bar->confidence = json_obj["confidence"];

	return bar;
}


std::unique_ptr<audio_analysis_t::beat_t> audio_analysis_t::beat_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<audio_analysis_t::beat_t> audio_analysis_t::beat_t::from_json(const json::json &json_obj)
{
	auto beat = std::make_unique<audio_analysis_t::beat_t>();

	beat->start = json_obj["start"];
	beat->duration = json_obj["duration"];
	beat->confidence = json_obj["confidence"];

	return beat;
}


std::unique_ptr<audio_analysis_t::section_t> audio_analysis_t::section_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<audio_analysis_t::section_t> audio_analysis_t::section_t::from_json(const json::json &json_obj)
{
	auto section = std::make_unique<audio_analysis_t::section_t>();

	section->start = json_obj["start"];
	section->duration = json_obj["duration"];
	section->confidence = json_obj["confidence"];
	section->loudness = json_obj["loudness"];
	section->tempo = json_obj["tempo"];
	section->tempo_confidence = json_obj["tempo_confidence"];
	section->key = json_obj["key"];
	section->key_confidence = json_obj["key_confidence"];
	section->mode = json_obj["mode"];
	section->mode_confidence = json_obj["mode_confidence"];
	section->time_signature = json_obj["time_signature"];
	section->time_signature_confidence = json_obj["time_signature_confidence"];

	return section;
}


std::unique_ptr<audio_analysis_t::segment_t> audio_analysis_t::segment_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<audio_analysis_t::segment_t> audio_analysis_t::segment_t::from_json(const json::json &json_obj)
{
	auto segment = std::make_unique<audio_analysis_t::segment_t>();

	segment->start = json_obj["start"];
	segment->duration = json_obj["duration"];
	segment->confidence = json_obj["confidence"];
	segment->loudness_start = json_obj["loudness_start"];
	segment->loudness_max = json_obj["loudness_max"];
	segment->loudness_max_time = json_obj["loudness_max_time"];
	segment->loudness_end = json_obj["loudness_end"];
	segment->start = json_obj["start"];

	return segment;
}


std::unique_ptr<audio_analysis_t::tatum_t> audio_analysis_t::tatum_t::from_json(const std::string &json_string)
{
	return from_json(json::json::parse(json_string));
}

std::unique_ptr<audio_analysis_t::tatum_t> audio_analysis_t::tatum_t::from_json(const json::json &json_obj)
{
	auto tatum = std::make_unique<audio_analysis_t::tatum_t>();

	tatum->start = json_obj["start"];
	tatum->duration = json_obj["duration"];
	tatum->confidence = json_obj["confidence"];

	return tatum;
}

}