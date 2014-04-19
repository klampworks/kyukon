#pragma once

#include "task.hpp"
#include <functional>
#include <vector>
#include <functional>

namespace kyukon {

	void init(const std::vector<std::pair<std::string, bool>>&);
	unsigned signup(int, std::function<void()>);
	void unregister(unsigned domain_id);
	void add_task(task *t);
	void set_do_fillup(bool, unsigned);
	void stop();
}


