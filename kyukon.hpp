#pragma once

#include "task.hpp"
#include <functional>
#include <vector>

namespace kyukon {

	void init(const std::vector<std::pair<std::string, bool>>&);
	void signup(unsigned domain_id, long interval_p, std::function<void()> fn);
	void add_task(task *t, unsigned);
	void set_do_fillup(bool, unsigned);
	//extern bool do_fillup;

}


#include <queue>
#include <mutex>
#include <map>
#include <functional>

struct domain_settings {

	std::priority_queue<task*, std::vector<task*>, task> task_list;
	std::mutex list_mutex;
	long interval;
	std::function<void()> fillup;
	bool do_fillup;
};
