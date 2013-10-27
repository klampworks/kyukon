#pragma once

#include <string>
#include <curl/curl.h>
#include <curl/easy.h>
#include "task.hpp"

class task;

class kon {

	public: 
		kon();
		kon(const std::string &proxy_address, bool socks);

		void grab(task *t);

	private:
		void init_curl();
		CURL *curl;
		static size_t write_data_string(char *ptr, size_t size, size_t nmemb, std::string *stream);
		static size_t write_data_file(char *ptr, size_t size, size_t nmemb, FILE *stream);
		void make_filepath(std::string &mi, const task*);
};
