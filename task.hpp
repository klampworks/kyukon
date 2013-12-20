#pragma once

#include <string>
#include <vector>
#include <functional>

class task {

	public:

		task(unsigned);
		task();

		void prepare_task(const std::string &url, const std::string &ref, const std::function<void(task*)> &callback);
		void prepare_result(const std::string &data, const long &status_code, const double &data_size);

		void set_url(const std::string &url);
		void set_ref(const std::string &ref);
		void set_data(const std::string &data);
		void set_status_code(const long &status_code);
		void set_data_size(const double &data_size);
		void set_callback(const std::function<void(task*)> &callback);
		void set_target_file();
		void set_target_string();
		void set_priority(const int &priority);
		void inc_priority();
		void set_max_retries(unsigned);
		bool inc_retries(); 
		void set_filepath(const std::string &filepath);

		std::string get_url() const;
		std::string get_ref() const;
		std::string get_data() const;
		long get_status_code() const;
		double get_data_size() const;
		std::function<void(task*)> get_callback() const;
		int get_target() const;
		int get_priority() const;
		const std::string get_filepath()const;
		unsigned get_retries() const;
		unsigned get_domain_id() const;

		bool operator()(const task *t1, const task *t2) {
			return (t1->get_priority() >= t2->get_priority());
		}

		enum task_target {
			STRING,	
			FILE,
		};

	protected:
		std::string url, ref, data, filepath;
		long status_code;
		double data_size;
		std::function<void(task*)> callback;
		int target, priority;
		unsigned max_retries, retries;
		unsigned domain_id;
};
