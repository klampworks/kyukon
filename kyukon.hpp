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
	std::function<void()> fillup;
	bool do_fillup;

	domain_settings() {

		interval = 0;
		fillup = nullptr;
		do_fillup = true;
	}
};

namespace kyukon {

	void init(const std::vector<std::pair<std::string, bool>>&);
	void signup(unsigned domain_id, int, void(*)());
	void add_task(task *t, unsigned);
	void set_do_fillup(bool, unsigned);
	void stop();
	//extern bool do_fillup;

}


