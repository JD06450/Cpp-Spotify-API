cmake_minimum_required(VERSION 3.23.0)
project(Cpp-Spotify-API VERSION 0.1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find and use vcpkg
set(VCPKG_INSTALLATION_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed" CACHE STRING "")
set(CMAKE_TOOLCHAIN_FILE "/opt/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Find the required packages
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)

add_executable(Cpp-Spotify-API
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

target_link_libraries(Cpp-Spotify-API PRIVATE nlohmann_json::nlohmann_json PRIVATE CURL::libcurl)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)