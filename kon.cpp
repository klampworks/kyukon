#include "kon.hpp"

#include <sstream>
#include <unistd.h>

kon::kon() {
	init_curl();
}

kon::kon(const std::string &proxy_address, bool socks) {

	init_curl();


	//TODO remove this.
	curl_easy_setopt(this->curl, CURLOPT_TIMEOUT, 5); 

	if (!proxy_address.empty())
		curl_easy_setopt(this->curl, CURLOPT_PROXY, proxy_address.c_str()); 

	//IMPORTANT to prevent DNS leaks.
	if (socks)
		curl_easy_setopt(this->curl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME); 
	
}

void kon::init_curl() {

	//Initialise our curl handle.
	this->curl = curl_easy_init();
	
	//Windows 8, Internet Explorer 10, fairly standard.
	//TODO Pick random user agents from a pool to avoid profiling. 
	static const char *default_ua = 
		"Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.2; WOW64; Trident/6.0)";

	curl_easy_setopt(this->curl, CURLOPT_USERAGENT, default_ua);
}

void kon::grab(task *t) {
				
	//Filename or HTML goes here.
	std::string mi;

	FILE *my_file = 0;

	curl_easy_setopt(curl, CURLOPT_URL, t->get_url().c_str());
	curl_easy_setopt(curl, CURLOPT_REFERER, t->get_ref().c_str());

	//Do we want this data as a file or string?
	if (t->get_target() == task::STRING) {
		//string
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mi);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_string);
	} else {
		//file
		make_filepath(mi, t);
		my_file = fopen(mi.c_str(), "wb+");

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, my_file);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_file);
	}

	CURLcode res = curl_easy_perform(curl);
	
	//It is important to close the file otherwise a chunk of the end may not be written.
	if (my_file)
		fclose(my_file);
	
	long code;
	double dl;

	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
	curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &dl);

	t->set_status_code(std::move(code));
	t->set_data_size(std::move(dl));
	t->set_data(std::move(mi));

	if (res)
		t->set_curl_result(curl_easy_strerror(res));
}

void kon::make_filepath(std::string &mi, const task *t) {

	unsigned int offset = t->get_url().find_last_of('/') + 1;

	if (offset < t->get_url().size()) {
		mi = t->get_filepath() + t->get_url().substr(offset);
	} else {
		//TODO Refactor, this never happens.
		mi = "dsklhf";
	}
}

size_t kon::write_data_file(char *ptr, size_t size, size_t nmemb, FILE *stream) {

	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}

size_t kon::write_data_string(char *ptr, size_t size, size_t nmemb, std::string *stream) {

	int result = 0;

	if (stream != NULL) {
	    stream->append(ptr, size * nmemb);
	    result = size * nmemb;
	}

	return result;
}
