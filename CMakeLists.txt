cmake_minimum_required(VERSION 3.23.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Cpp-Spotify-API VERSION 0.1.0)

# TODO: Add option to compile to shared lib instead of static

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
)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)

target_link_libraries(Cpp-Spotify-API PRIVATE nlohmann_json::nlohmann_json PRIVATE CURL::libcurl)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
#include(CPack)