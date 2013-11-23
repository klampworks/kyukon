#pragma once

class task;
#include <functional>
#include <vector>

namespace kyukon {

	void init(const std::vector<std::pair<std::string, bool>>&);
	void signup(unsigned domain_id, long interval_p, std::function<void()> fn);
	void add_task(task *t, unsigned);
	void set_do_fillup(bool, unsigned);
	//extern bool do_fillup;

}

