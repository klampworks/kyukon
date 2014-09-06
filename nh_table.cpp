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

std::vector<dom_id> nh_table::next(thread_id thread)
{
	long now = time(NULL);

	std::map<dom_id, long> times;
	for (auto did : dom_ids) {
		times[did] = (now - next(did, thread));
	}
	auto res = dom_ids;

	/* Remove negative values, they cannot be hit yet. */
	auto end = std::remove_if(res.begin(), res.end(), 
		[&times] (dom_id a) {
			return times[a] < 0;
		}
	);

	res = std::vector<dom_id>(res.begin(), end);

	/* Order by greatest value, these are the longest overdue. */
	std::sort(res.begin(), res.end(), 
		[&times] (dom_id a, dom_id b) {
		       return times[a] > times[b];
		}
	);

	return res;
}
