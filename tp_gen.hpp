#pragma once

#include "tp_base.hpp"
#include "task.hpp"
#include <functional>

struct tp_gen : public tp_base<task> {

	tp_gen(unsigned thread_no) 
		: tp_base(thread_no)
	{}

	void task_action(unsigned thread_id, task t) {
		t.get_callback()(&t);
	}
};
