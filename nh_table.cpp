#include "nh_table.hpp"
#include <algorithm>

void nh_table::add_dom(dom_id dom)
{
	if (std::find(dom_ids.begin(), dom_ids.end(), dom) != dom_ids.end())
		return;

	dom_ids.push_back(dom);	
	
	for (thread_id a : thread_ids)
		next_hit[std::pair<dom_id, thread_id>(dom, a)] = 0;
}

void nh_table::add_thread(thread_id thread_id)
{
	if (std::find(thread_ids.begin(), thread_ids.end(), thread_id) 
		!= thread_ids.end()) {

		return;
	}

	thread_ids.push_back(thread_id);	
	
	for (auto a : dom_ids)
		next_hit[std::make_pair(a, thread_id)] = 0;
}

void nh_table::update(dom_id dom_id, thread_id thread_id, long interval)
{
	next_hit[std::make_pair(dom_id, thread_id)] = time(NULL) + interval;
}

long nh_table::next(dom_id dom, thread_id thread)
{
	return next_hit[std::make_pair(dom, thread)];
}

dom_id nh_table::next(thread_id thread)
{
	long now = time(NULL);

	dom_id dom = 0;
	long max = -1;

	for (auto did : dom_ids) {
		//if ((long tmp = (now - next(did, thread))) > max) {
		long tmp = now - next(did, thread);
		if (tmp > max) {
			max = tmp;
			dom = did;
		}
	}

	return dom;
}
