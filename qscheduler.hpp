#pragma once

#include "nh_table.hpp"
#include <functional>
#include <condition_variable>
#include <thread>
#include "tscheduler.hpp"

typedef unsigned thread_id;
typedef unsigned dom_id;
class task;
class domain_settings;

struct qscheduler : public tscheduler {

	qscheduler();
	void add_task(task*);
	dom_id reg_dom(long interval, std::function<void()> fillup_fn);
	void reg_thread(thread_id);
	void unreg_dom(dom_id);
	void resolve();
	void update_nh(dom_id, thread_id);
	void set_do_fillup(bool b, unsigned domain_id);

	/* dom_id 0 signifies an invalid value. */
	dom_id latest_dom_id = 1;

	std::map<dom_id, domain_settings*> domains;
	nh_table next_hit;
	~qscheduler();
};
