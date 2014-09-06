#pragma once

#include <vector>
#include <map>
typedef unsigned dom_id;
typedef unsigned thread_id;

struct nh_table {

	std::vector<dom_id> dom_ids;
	std::vector<thread_id> thread_ids;
	
	std::map<std::pair<dom_id, thread_id>, long> next_hit;

	void add_dom(dom_id);
	void add_thread(thread_id);

	//lol no.
	//void remove_dom(dom_id);
	//void remove_thread(thread_id);

	void update(dom_id, thread_id, long);
	long next(dom_id, thread_id);

	/* Given a thread, return list of hittable domains in order
	 * of longest overdue first. Result may be empty. */
	std::vector<dom_id> next(thread_id);
};
