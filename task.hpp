#pragma once

#include <string>
#include <vector>
#include <functional>

class task {

	public:

		enum task_target {
			STRING,	
			FILE,
		};

		task(unsigned);
		task();

		task(unsigned, const std::string &, const std::string &, 
			task_target target,const std::function<void(task*)> &);

		task(unsigned, std::string &&url_p, std::string &&ref_p, 
			task_target target_p, std::function<void(task*)> &&callback_p);

		void prepare_result(std::string &&data, long &&status_code, 
			double &&data_size);

		void set_url(const std::string &url);
		void set_ref(const std::string &ref);
		void set_data(std::string &&data);
		void set_status_code(long &&status_code);
		void set_data_size(double &&data_size);
		void set_callback(const std::function<void(task*)> &callback);
		void set_target_file();
		void set_target_string();
		void set_priority(const int &priority);
		void inc_priority();
		void set_max_retries(unsigned);
		bool inc_retries(); 
		void set_filepath(const std::string &filepath);
		void set_curl_result(int);

		std::string get_url() const;
		std::string get_ref() const;
		std::string get_data() const;
		long get_status_code() const;
		double get_data_size() const;
		std::function<void(task*)> get_callback() const;
		task_target get_target() const;
		int get_priority() const;
		const std::string get_filepath()const;
		unsigned get_retries() const;
		unsigned get_domain_id() const;
		int get_curl_result() const;

		bool operator()(const task *t1, const task *t2) {
			return (t1->get_priority() >= t2->get_priority());
		}


	protected:
		std::string url, ref, data, filepath;
		int curl_result;
		long status_code;
		double data_size;
		std::function<void(task*)> callback;
		task_target target; 
		int priority;
		unsigned max_retries, retries;
		unsigned domain_id;
};
