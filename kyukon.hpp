#pragma once

#include "task.hpp"
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <map>
#include <functional>

struct domain_settings {

	std::priority_queue<task*, std::vector<task*>, task> task_list;
	std::mutex *list_mutex;
	long interval;
	std::function<void()> fillup;
	bool do_fillup;

	domain_settings() {

		interval = 0;
		list_mutex = new std::mutex();
		fillup = nullptr;
		do_fillup = true;
	}

	~domain_settings() {

		if (list_mutex)
			delete list_mutex;
	}
};

namespace kyukon {

	void init(const std::vector<std::pair<std::string, bool>>&);
	void signup(unsigned domain_id, domain_settings&&);
	void add_task(task *t, unsigned);
	void set_do_fillup(bool, unsigned);
	//extern bool do_fillup;

}


