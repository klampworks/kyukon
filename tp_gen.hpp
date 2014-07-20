#pragma once

#include "tp_base.hpp"
#include "task.hpp"
#include <functional>

struct tp_gen : public tp_base<task> {


	std::function<void(task)> fn;

	tp_gen(unsigned thread_no, std::function<void(task)> && fn_p) 
		: tp_base(thread_no), fn(std::move(fn_p))
	{}

	void task_action(unsigned thread_id, task t) {
		fn(t);
	}
};
