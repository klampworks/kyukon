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

std::map<unsigned /*domain_id*/, domain_settings> settings;
std::map<unsigned /*domain_id*/, std::map<unsigned /*thread_id*/, long>> next_hit;

void thread_run(const std::pair<std::string, bool>&, unsigned);

void init(const std::vector<std::pair<std::string, bool>> &proxy_info) {

	number_of_threads = proxy_info.size();

	for (unsigned i = 0; i < number_of_threads; i++) {
		//std::thread(thread_run, proxy_info[i], i).detach();
		std::string a = proxy_info[i].first;
		bool b = proxy_info[i].second;
		std::thread(thread_run, proxy_info[i], i).detach();
		thread_ids.push_back(i);
	}

	std::cout << "Number of threads = " << number_of_threads << std::endl;

}

void set_do_fillup(bool b, unsigned domain_id) {
	settings[domain_id].do_fillup = b;
}

void add_task(task *t, unsigned domain_id) {

	if (std::find(domain_ids.begin(), domain_ids.end(), domain_id) == domain_ids.end()) {
		std::cout << "Unregistered domain_id " << domain_id << std::endl;
		return;
	}

	auto *my_task_list = &settings[domain_id].task_list;

	while (my_task_list->size() > max_queue_length) {
		std::cout << "Queue limit reached! Waiting..." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	//settings[domain_id].list_mutex->lock();
	my_task_list->push(t);
	//settings[domain_id].list_mutex->unlock();
}

void signup(unsigned domain_id, domain_settings&& set) {

	std::vector<unsigned> domain_ids;

	if (std::find(domain_ids.begin(), domain_ids.end(), domain_id) != domain_ids.end()) {
		std::cout << "Domain: " << domain_id << " has already been registered." << std::endl;
		return;
	}

puts("pre");

	std::cout << "Copying settings for domain " << domain_id << std::endl;

	
	std::cout << "Settings are " << set.interval << std::endl;
	//std::swap(settings[domain_id], set);

	//this line casues a crash.
	settings[domain_id] = set;
puts("here");

	for (unsigned thread_id : thread_ids) {
		next_hit[domain_id][thread_id] = 0;
	}
}

task* get_task(unsigned thread_no) {

	//no op
	return nullptr;

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
	//settings[domain].list_mutex->lock();

	long tmp_time = time(NULL);

	if (!settings[domain].task_list.empty()) {

		ret = settings[domain].task_list.top();
		settings[domain].task_list.pop();
		
	} else {
		if (settings[domain].do_fillup)
			settings[domain].fillup();
	}

	//settings[domain].list_mutex->unlock();
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

		next_hit[dom][threadno] = time(NULL) + settings[dom].interval;

		if (current_task->get_callback())
			std::thread(current_task->get_callback(), current_task).detach();
		else
			delete current_task;
	}
}

}//namespace
