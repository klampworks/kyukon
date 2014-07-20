#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>

/* TODO Implementation must be within header file until export is implemented.*/
export template <class task_type>

struct tp_base {

	tp_base(unsigned no_threads);
	task_type get_task(unsigned thread_id);
	void done_task(task_type);

	void thread_body(unsigned);
	virtual void task_action(unsigned,task_type) = 0;
	void stop();
	void stopped(unsigned thread_id);

	std::vector<unsigned> threads;

	bool keep_going;
	std::mutex stop_m;
};

template <typename task_type>
tp_base<task_type>::tp_base(unsigned no_threads)
{
	keep_going = true;

	for (unsigned i = 0; i < no_threads; ++i) {
		std::thread(std::bind(
			&tp_base<task_type>::thread_body, this, i)).detach();
		threads.push_back(i);
	}
}

template <typename task_type>
task_type tp_base<task_type>::get_task(unsigned thread_id)
{
}

template <typename task_type>
void tp_base<task_type>::done_task(task_type)
{}


template <typename task_type>
void tp_base<task_type>::stopped(unsigned thread_id)
{
	stop_m.lock();
	
	auto it = std::find(threads.begin(), threads.end(), thread_id);
	threads.erase(it);

	stop_m.unlock();
}

template <typename task_type>
void tp_base<task_type>::stop()
{
	keep_going = false;

	while(!threads.empty())
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

#include <iostream>
template <typename task_type>
void tp_base<task_type>::thread_body(unsigned thread_id)
{
	while(keep_going) {
		auto t = get_task(thread_id);
		task_action(thread_id, t);
		done_task(t);
	}

	stopped(thread_id);
}
