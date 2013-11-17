#pragma once

class task;
#include <functional>

namespace kyukon {

	void init(unsigned threads, std::function<void()> fn);
	void add_task(task *t);
	//extern bool do_fillup;

}

