#include "curl-util.hpp"

size_t curl_callback(char *contents, size_t size, size_t nmemb, std::string* output) {
	size_t realsize = size * nmemb;
	// printf("Data: ");
	// for (int i = 0; i < realsize; i++) {
	// 	printf("%c", contents[i]);
	// }
	// printf("\n");
	output->append(contents, realsize);
	return realsize;
}


namespace http
{

std::string method_to_string(REQUEST_METHOD method)
{
	switch (method)
	{

	case REQUEST_METHOD::METHOD_POST:
		return "POST";
	case REQUEST_METHOD::METHOD_PUT:
		return "PUT";
	case REQUEST_METHOD::METHOD_DELETE:
		return "DELETE";
	case REQUEST_METHOD::METHOD_OPTIONS:
		return "OPTIONS";
	case REQUEST_METHOD::METHOD_PATCH:
		return "PATCH";
	case REQUEST_METHOD::METHOD_HEAD:
		return "HEAD";
	case REQUEST_METHOD::METHOD_CONNECT:
		return "CONNECT";
	case REQUEST_METHOD::METHOD_TRACE:
		return "TRACE";
	case REQUEST_METHOD::METHOD_GET:
	default:
		return "GET";
	};
}

std::string url_encode(const std::string &to_encode)
{
	std::ostringstream encoded;
	encoded.fill(0x00);
	encoded << std::hex;

	for (int i = 0; i < to_encode.length(); i++)
	{
		unsigned char ch = to_encode.at(i);

		if (isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
			encoded << ch;
		} else {
			encoded << '%' << std::uppercase << std::setw(2) << (int) ch;
		}
	}
	return encoded.str();
}

api_response get(const char *url, const std::string &query_data, const std::string &auth_token) {
	api_response retval;

	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;

	slist1 = NULL;
	slist1 = curl_slist_append(slist1, (std::string("Authorization: Bearer ") + auth_token).c_str());

	std::string full_url = url;
	if (!query_data.empty()) {
		full_url.append("\?");
	}

	full_url += query_data;

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
	// curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(hnd, CURLOPT_URL, full_url.c_str());
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.84.0");
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &(retval.body));
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, curl_callback);

	ret = curl_easy_perform(hnd);

	curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &(retval.code));

	curl_easy_cleanup(hnd);
	hnd = NULL;
	curl_slist_free_all(slist1);
	slist1 = NULL;

	if (ret != CURLE_OK) {
		throw "cURL operation failed";
	}
	return retval;
}

//TODO: use application/json instead of x-www-url-formencoded

api_response post(const char *url, const std::string &post_data, const std::string &auth_header_value, bool is_token) {
	api_response retval;

	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;

	slist1 = NULL;
	std::string auth_header_prefix = is_token ? "Authorization: Bearer " : "Authorization: Basic ";
	slist1 = curl_slist_append(slist1, (auth_header_prefix + auth_header_value).c_str());
	// slist1 = curl_slist_append(slist1, "Content-Type: application/json");

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
	// curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, post_data.c_str());
	curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t) post_data.length());
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.84.0");
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(hnd, CURLOPT_POST, 1);
	curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &(retval.body));
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, curl_callback);

	ret = curl_easy_perform(hnd);

	curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &(retval.code));

	curl_easy_cleanup(hnd);
	hnd = NULL;
	curl_slist_free_all(slist1);
	slist1 = NULL;

	if (ret != CURLE_OK) {
		throw "cURL operation failed";
	}
	return retval;
}

api_response request(const char *url, REQUEST_METHOD method, const std::string &body_data, const std::string &auth_header_value, bool is_token) {
	api_response retval;

	CURLcode ret;
	CURL *hnd;
	struct curl_slist *slist1;

	slist1 = NULL;
	std::string auth_header_prefix = is_token ? "Authorization: Bearer " : "Authorization: Basic ";
	slist1 = curl_slist_append(slist1, (auth_header_prefix + auth_header_value).c_str());
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");

	

	hnd = curl_easy_init();
	curl_easy_setopt(hnd, CURLOPT_BUFFERSIZE, 102400L);
	curl_easy_setopt(hnd, CURLOPT_VERBOSE, 1);
	curl_easy_setopt(hnd, CURLOPT_URL, url);
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(hnd, CURLOPT_POST, 1L);
	curl_easy_setopt(hnd, CURLOPT_COPYPOSTFIELDS, body_data.c_str());
	// curl_easy_setopt(hnd, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t) body_data.length());
	curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, slist1);
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, "curl/7.84.0");
	curl_easy_setopt(hnd, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(hnd, CURLOPT_HTTP_VERSION, (long) CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, method_to_string(method).c_str());
	curl_easy_setopt(hnd, CURLOPT_TCP_KEEPALIVE, 1L);
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, &(retval.body));
	curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, curl_callback);

	ret = curl_easy_perform(hnd);

	curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &(retval.code));

	curl_easy_cleanup(hnd);
	hnd = NULL;
	curl_slist_free_all(slist1);
	slist1 = NULL;

	if (ret != CURLE_OK) {
		throw "cURL operation failed";
	}
	return retval;
}

} // namespace http