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
#include <cassert>

namespace kyukon {

bool keep_going = true;
unsigned number_of_threads;
unsigned max_queue_length = 100;

std::vector<unsigned> domain_ids;
std::vector<unsigned> thread_ids;

std::map<unsigned /*domain_id*/, domain_settings> settings;
std::map<unsigned /*domain_id*/, std::map<unsigned /*thread_id*/, long>> next_hit;

void thread_run(const std::pair<std::string, bool>&, unsigned);

void init(const std::vector<std::pair<std::string, bool>> &proxy_info) {

	number_of_threads = proxy_info.size();

	for (unsigned i = 0; i < number_of_threads; i++) {

		std::string a = proxy_info[i].first;
		bool b = proxy_info[i].second;
		std::thread(thread_run, proxy_info[i], i).detach();
		thread_ids.push_back(i);
	}

	keep_going = true;

	std::cout << "Initialising Kyukon with " << number_of_threads << " threads." << std::endl;
}

void set_do_fillup(bool b, unsigned domain_id) {
	settings[domain_id].do_fillup = b;
}

void add_task(task *t) {

	unsigned domain_id = t->get_domain_id();

	if (std::find(domain_ids.begin(), domain_ids.end(), domain_id) == domain_ids.end()) {
		std::cout << "Unregistered domain_id " << domain_id << std::endl;
		return;
	}

	// Grab a reference to the settings struct for the domain we are interested in.
	domain_settings &set = settings[domain_id];

	while (set.task_list.size() > max_queue_length) {
		std::cout << "Queue limit reached! Waiting..." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	set.list_mutex.lock();
	set.task_list.push(t);
	set.list_mutex.unlock();
}

unsigned signup(int interval, std::function<void()> fillup) {

	unsigned new_id;

	if (!domain_ids.empty())
		new_id = domain_ids.back() + 1;
	else
		new_id = 0;

	domain_ids.push_back(new_id);

	domain_settings set;
	set.interval = interval;
	set.fillup = fillup;

	settings[new_id] = std::move(set);

	for (unsigned thread_id : thread_ids) {
		next_hit[new_id][thread_id] = 0;
	}

	return new_id;
}

void unregister(unsigned domain_id) {

	auto it = std::find(domain_ids.begin(), domain_ids.end(), domain_id);
	
	if (it == domain_ids.end()) {
		std::cout << "Unregistered domain_id " << domain_id << std::endl;
		return;
	}

	//TODO This is a resource leak since it does not clean up settings and next_hit.
	domain_ids.erase(it);
}

task* get_task(unsigned thread_no) {

	long now = time(NULL);
	long min = now;
	long long domain = -1;

	//For each domain...
	for (unsigned &d : domain_ids) {

		//Find the next hit timestamp for this domain.
		long tmp_hit = next_hit[d][thread_no];

		//If it is the eearlist timestamp make a note.
		if (tmp_hit < min) {
			domain = d;
			min = tmp_hit;
		}
	}

	//This means that it is too soon for this thread to hit any domains again.
	if (domain == -1)
		return nullptr;

	task *ret = nullptr;

	domain_settings &set = settings[domain];
	set.list_mutex.lock();

	long tmp_time = time(NULL);

	if (!set.task_list.empty()) {

		ret = set.task_list.top();
		set.task_list.pop();
		
	} else {

		if (set.do_fillup && set.fillup)
			set.fillup();
		else
			//std::cout << "WARNING, queue is empty and no fillup function as "
			//"been set for domain " << domain << std::endl;

			//TODO is this ok?
			//Increment the next hit by an arbitrary value to avoid wasting time.
			next_hit[domain][thread_no] += 10;
	}

	set.list_mutex.unlock();
	return ret;
}

//void thread_run(std::pair<std::string, bool> proxy_info, unsigned threadno) {
void thread_run(const std::pair<std::string , bool> &proxy_info, unsigned threadno) {

	kon m_kon(proxy_info.first, proxy_info.second);

	const std::string my_threadno = std::to_string(threadno);

	task *current_task = nullptr;

	for(;;) {

		do {
			if (!keep_going) return;

			current_task = get_task(threadno);

		//If current_task is null then wait some time and go to the start of the loop.
		} while (!current_task && 
			[](){std::this_thread::sleep_for(std::chrono::seconds(2)); return true;}());
		
		std::cout << my_threadno << ": ^^^Fetching " << current_task->get_url() << std::endl;
		m_kon.grab(current_task);
		std::cout << my_threadno << ": $$$Finished " << current_task->get_url() << std::endl;;


		long dom = current_task->get_domain_id();

		next_hit[dom][threadno] = time(NULL) + settings[dom].interval;

		if (current_task->get_callback()) {
			std::thread(current_task->get_callback(), current_task).detach();
		} else
			delete current_task;
	}
}

void stop() {
	
	std::cout << "Stopping Kyukon and destroying threads..." << std::endl;
	thread_ids.clear();
	keep_going = false;
}

}//namespace
