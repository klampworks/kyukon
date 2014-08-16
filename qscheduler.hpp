#pragma once

#include "nh_table.hpp"
#include <functional>
#include <condition_variable>

typedef unsigned thread_id;
typedef unsigned dom_id;
class task;
class domain_settings;

union thread_val {
	std::condition_variable *cv;
	dom_id *did;
};

struct qscheduler {

	void add_task(task*);
	task* get_task(unsigned thread_id);
	dom_id reg_dom(long interval, std::function<void()> fillup_fn);
	void unreg_dom(dom_id);

	
	/* dom_id 0 signifies an invalid value. */
	dom_id latest_dom_id = 1;

	std::map<dom_id, domain_settings*> domains;
	nh_table next_hit;
	std::map<thread_id, thread_val> threads;
	std::mutex thread_m;
	~qscheduler();
	//std::priority_queue<task*, std::vector<task*>, task> task_list;

	
};
