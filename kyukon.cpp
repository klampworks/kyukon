#include "kyukon.hpp"

#include <chrono>
#include <queue>
#include <mutex>
#include <thread>
#include "kon.hpp"
#include "task.hpp"
#include <iostream>
#include <ctime>
#include <map>
#include <algorithm>
#include <functional>

namespace kyukon {

bool keep_going = true;
unsigned number_of_threads;
unsigned max_queue_length = 100;

std::vector<unsigned> domain_ids;
std::vector<unsigned> thread_ids;

std::map<unsigned /*domain_id*/, std::priority_queue<task*, std::vector<task*>, task>> task_list;
std::map<unsigned /*domain_id*/, std::mutex> list_mutex;
std::map<unsigned /*domain_id*/, long> interval;
std::map<unsigned /*domain_id*/, std::function<void()>> fillup;
std::map<unsigned /*domain_id*/, bool> do_fillup;
std::map<unsigned /*domain_id*/, std::map<unsigned /*thread_id*/, long>> next_hit;

void thread_run(const std::pair<std::string, bool>&, unsigned);

void init(const std::vector<std::pair<std::string, bool>> &proxy_info, std::function<void()> fn) {

	number_of_threads = proxy_info.size();

	for (unsigned i = 0; i < number_of_threads; i++) {
		//std::thread(thread_run, proxy_info[i], i).detach();
		std::string a = proxy_info[i].first;
		bool b = proxy_info[i].second;
		std::thread(thread_run, proxy_info[i], i).detach();
		thread_ids.push_back(i);
	}

}

void add_task(task *t, unsigned domain_id) {

	if (std::find(domain_ids.begin(), domain_ids.end(), domain_id) == domain_ids.end()) {
		std::cout << "Unregistered domain_id " << domain_id << std::endl;
		return;
	}

	auto *my_task_list = &task_list[domain_id];

	while (my_task_list->size() > max_queue_length) {
		std::cout << "Queue limit reached! Waiting..." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	list_mutex[domain_id].lock();
	my_task_list->push(t);
	list_mutex[domain_id].unlock();
}

void signup(unsigned domain_id, long interval_p, std::function<void()> fn) {

	std::vector<unsigned> domain_ids;
	if (std::find(domain_ids.begin(), domain_ids.end(), domain_id) == domain_ids.end()) {
		std::cout << "Domain: " << domain_id << " has already been registered." << std::endl;
	}

	interval[domain_id] = interval_p;
	fillup[domain_id] = fn;
	do_fillup[domain_id] = false;

	for (unsigned thread_id : thread_ids) {
		next_hit[domain_id][thread_id] = 0;
	}
}

task* get_task(unsigned thread_no) {

	long now = time(NULL);
	long min = now;
	unsigned domain = 0;

	for (auto &a: next_hit) {
		
		long tmp_hit = a.second[thread_no];

		if (tmp_hit < min) {

			domain = a.first;
			min = tmp_hit;
		}
	}

	if (min == now)
		return nullptr;

	task *ret = nullptr;
	list_mutex[domain].lock();

	long tmp_time = time(NULL);

	if (!task_list[domain].empty()) {

		ret = task_list[domain].top();
		task_list[domain].pop();
		
	} else {
		if (do_fillup[domain])
			fillup[domain]();
	}

	list_mutex[domain].unlock();
	return ret;
}

//void thread_run(std::pair<std::string, bool> proxy_info, unsigned threadno) {
void thread_run(const std::pair<std::string , bool> &proxy_info, unsigned threadno) {

	kon m_kon(proxy_info.first, proxy_info.second);

	const std::string my_threadno = std::to_string(threadno);

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

		long dom = current_task->get_domain_id();

		next_hit[dom][threadno] = time(NULL) + interval[dom];

		if (current_task->get_callback())
			std::thread(current_task->get_callback(), current_task).detach();
		else
			delete current_task;
	}
}

}//namespace
