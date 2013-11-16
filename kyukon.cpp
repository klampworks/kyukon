#include "kyukon.hpp"

#include <chrono>
#include <queue>
#include <mutex>
#include <thread>
#include "kon.hpp"
#include "task.hpp"
#include <iostream>
#include <ctime>

namespace kyukon {

bool keep_going = true;
unsigned number_of_threads;
unsigned max_queue_length = 100;


std::function<void()> fillup =  nullptr;
bool do_fillup = false;

std::priority_queue<task*, std::vector<task*>, task> task_list;
std::mutex list_mutex;
long next_hit = 0;
long interval = 10;


void thread_run(unsigned);

void init(unsigned no_threads, std::function<void()> fn) {

	number_of_threads = no_threads;

	for (unsigned i = 0; i < no_threads; i++) {
		std::thread(thread_run, i).detach();
	}

	fillup = fn;
}

void add_task(task *t) {

	while (task_list.size() > max_queue_length) {
		std::cout << "Queue limit reached! Waiting..." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	//TODO Multiple priority queues for different sites.
	list_mutex.lock();
	task_list.push(t);
	list_mutex.unlock();
}

task* get_task(unsigned thread_no) {

	task *ret = nullptr;

	list_mutex.lock();

	long tmp_time = time(NULL);

	if (!task_list.empty()) {

		if (tmp_time > next_hit) {

			ret = task_list.top();
			task_list.pop();
			next_hit = tmp_time + interval;
		}
	} else {
		if (do_fillup)
			fillup();
	}

	list_mutex.unlock();
	return ret;
}

void thread_run(const unsigned port_no) {

	kon m_kon;

	static unsigned threadno = 0;

	const std::string my_threadno = std::to_string(threadno++);

	task *current_task = nullptr;

	while(keep_going) {

		current_task = get_task(threadno);

		if (!current_task) {

			std::cout << my_threadno << ": No tasks, sleeping...." << std::endl;;
			
			do {
				std::this_thread::sleep_for(std::chrono::seconds(2));
				current_task = get_task(threadno);
			} while (!current_task);
		}
		
		//TODO Fetching and Finishing look too similar.
		std::cout << my_threadno << ": Fetching " << current_task->get_url() << std::endl;
		m_kon.grab(current_task);
		std::cout << my_threadno << ": Finished " << current_task->get_url() << std::endl;;

		if (current_task->get_callback())
			std::thread(current_task->get_callback(), current_task).detach();
		else
			delete current_task;
	}
}

}//namespace
