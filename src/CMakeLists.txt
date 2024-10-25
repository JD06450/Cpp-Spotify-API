add_library(Cpp-Spotify-API STATIC
	spotify-api.cpp
	curl-util.cpp
	categories/albums.cpp
	categories/artists.cpp
	categories/common.cpp
	categories/episodes.cpp
	categories/player.cpp
	categories/playlist.cpp
	categories/session.cpp
	categories/shows.cpp
	categories/tracks.cpp
	categories/search.cpp

	endpoints/search.cpp
)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)

message("Source Dir: ${Cpp-Spotify-API_SOURCE_DIR}\n")

target_link_libraries(Cpp-Spotify-API PRIVATE nlohmann_json::nlohmann_json PRIVATE CURL::libcurl)
target_include_directories(Cpp-Spotify-API PRIVATE ${Cpp-Spotify-API_SOURCE_DIR}/include)
