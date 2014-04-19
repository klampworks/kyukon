#pragma once

#include "task.hpp"
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <map>
#include <functional>
#include <condition_variable>

struct domain_settings {

	std::priority_queue<task*, std::vector<task*>, task> task_list;
	long interval;
	std::mutex list_mutex;
	std::condition_variable nfull;
	std::function<void()> fillup;
	bool do_fillup;

	domain_settings() : list_mutex() {

		interval = 0;
		fillup = nullptr;
		do_fillup = true;
	}

	//It is important to note that this is a move, the queue and mutex
	//will both be lost. I dont forsee any future reason we would want to copy
	//a domain_settings struct and keep the original.
	void operator=(domain_settings &&other) {
		
		std::swap(fillup, other.fillup);
		interval = other.interval;
		do_fillup = other.do_fillup;
	}
};

namespace kyukon {

	void init(const std::vector<std::pair<std::string, bool>>&);
	unsigned signup(int, std::function<void()>);
	void unregister(unsigned domain_id);
	void add_task(task *t);
	void set_do_fillup(bool, unsigned);
	void stop();
	//extern bool do_fillup;

}


