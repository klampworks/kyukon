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
	long interval;
	std::mutex list_mutex;
	std::function<void()> fillup;
	bool do_fillup;

	domain_settings() : list_mutex() {

		interval = 0;
		fillup = nullptr;
		do_fillup = true;
	}

	void operator=(domain_settings &&other) {
		
		std::swap(fillup, other.fillup);
		interval = other.interval;
		do_fillup = other.do_fillup;
	}
};

namespace kyukon {

	void init(const std::vector<std::pair<std::string, bool>>&);
	void signup(unsigned domain_id, int, std::function<void()>);
	void add_task(task *t, unsigned);
	void set_do_fillup(bool, unsigned);
	void stop();
	//extern bool do_fillup;

}


