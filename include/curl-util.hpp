#ifndef _CURL_UTIL_FILE_
#define _CURL_UTIL_FILE_

#include <curl/curl.h>
#include <string>
#include <iomanip>
#include <cstring>

namespace http
{
	struct api_response {
		int code;
		std::string body;
	};

	// Enum names are prefixed with "METHOD_" to avoid a naming conflict
	// with the Windows "DELETE" macro.

	enum class REQUEST_METHOD {
		METHOD_GET,
		METHOD_POST,
		METHOD_PUT,
		METHOD_DELETE,
		METHOD_OPTIONS,
		METHOD_TRACE,
		METHOD_PATCH,
		METHOD_CONNECT,
		METHOD_HEAD
	};

	std::string url_encode(const std::string &to_encode);

	api_response get(const char *url, const std::string &query_data, const std::string &auth_token);
	api_response post(const char *url, const std::string &post_data, const std::string &auth_header_value, bool is_token);
	api_response request(const char *url, REQUEST_METHOD method, const std::string &body_data, const std::string &auth_header_value, bool is_token);
} // namespace http

#endif