#pragma once

#include "tp_base.hpp"
#include "task.hpp"
#include "kon.hpp"
#include <string>

struct tp_kon : public tp_base<task> {

	std::vector<kon> kons;

	tp_kon(std::vector<std::pair<std::string, bool>> &&proxy_info) 
		: tp_base(proxy_info.size())
	{
		for (const auto &p : proxy_info)
			kons.push_back(kon(p.first, p.second));
	}

	void task_action(unsigned thread_id, task t) {
		/* TODO pointers... */
		kons[thread_id].grab(&t);
		//add to parser queue
	}
};
