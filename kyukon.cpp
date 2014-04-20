#include "kyukon.hpp"
#include "domain_settings.hpp"

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

#define CPP_LOG_INFO_COUT
#define CPP_LOG_WARN_COUT
#define CPP_LOG_INFO_COUT
#include "cpp-log/cpp_log.hpp"

namespace kyukon {

bool keep_going = true;
unsigned number_of_threads;
unsigned max_queue_length = 10000;

std::vector<unsigned> domain_ids;
std::vector<unsigned> thread_ids;

std::map<unsigned /*domain_id*/, domain_settings> settings;
std::map<unsigned /*domain_id*/, std::map<unsigned /*thread_id*/, long>> next_hit;

std::condition_variable c_avail_dom;
std::mutex m_avail_dom;

void thread_run(const std::pair<std::string, bool>&, unsigned);

void init(const std::vector<std::pair<std::string, bool>> &proxy_info) {

	if (!thread_ids.empty()) {

		clog::warn() << "Kyukon is already running!\n" 
		"Please call kyukon::stop() to reset everything.";

		return;
	}

	number_of_threads = proxy_info.size();

	for (unsigned i = 0; i < number_of_threads; i++) {

		std::string a = proxy_info[i].first;
		bool b = proxy_info[i].second;
		std::thread(thread_run, proxy_info[i], i).detach();
		thread_ids.push_back(i);
	}

	keep_going = true;

	clog::info() << "Initialising Kyukon with " << number_of_threads 
		<< " threads.";
}

void set_do_fillup(bool b, unsigned domain_id) {
	settings[domain_id].do_fillup = b;
}

void add_task(task *t) {
	unsigned domain_id = t->get_domain_id();

	if (std::find(domain_ids.begin(), domain_ids.end(), domain_id) 
		== domain_ids.end()) {

		clog::err() << "Unregistered domain_id " << domain_id;
		return;
	}

	/* Grab a reference to the settings struct for the 
	 * domain we are interested in. */
	domain_settings &set = settings[domain_id];

	std::unique_lock<std::mutex> l(set.list_mutex);

	/* Block until length of list is shorter than the max length. */
	set.nfull.wait(l, [&set]() 
		{ 
			return set.task_list.size() < max_queue_length;
		}
	);

	set.task_list.push(t);

	/* Notify threads waiting on empty lists. */
	c_avail_dom.notify_one();
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
		clog::err() << "Unregistered domain_id " << domain_id;
		return;
	}

	//TODO This is a resource leak since it does not clean up settings and next_hit.
	domain_ids.erase(it);
}

/* Find the next domain that a given thread number should service or
 * return -1 if no suitable domains were found. */
long long next_dom(unsigned thread_no)
{
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

	return domain;
}

task* get_task(unsigned thread_no) {

	long long domain = -1;

	std::unique_lock<std::mutex> l(m_avail_dom);
	c_avail_dom.wait_for(l, std::chrono::seconds(5), [&domain, thread_no]() {
		domain = next_dom(thread_no);
		return domain != -1;
	});

	if (domain == -1)
		return nullptr;

	task *ret = nullptr;

	domain_settings &set = settings[domain];
	l.unlock();

	set.list_mutex.lock();

	if (!set.task_list.empty()) {

		ret = set.task_list.top();
		set.task_list.pop();
		set.list_mutex.unlock();
		set.nfull.notify_one();
		
	} else {
		
		if (set.do_fillup && set.fillup) {

			clog::info() << "Performing fillup for domain " 
				<< domain;

			//TODO this is a dumb way of preventing double fillups.
			set.do_fillup = false;
			set.list_mutex.unlock();
			set.fillup();
		} else {
			clog::err() << thread_no << ": WARNING, queue "
			"is empty and no fillup function as "
			"been set for domain " << domain;

			//TODO is this ok?
			/*Increment the next hit by an arbitrary 
			 * value to avoid wasting time.
			 */
			next_hit[domain][thread_no] += 10;
			set.list_mutex.unlock();
		}
	}

	return ret;
}

//void thread_run(std::pair<std::string, bool> proxy_info, unsigned threadno) {
void thread_run(const std::pair<std::string , bool> &proxy_info, unsigned threadno) {

	kon m_kon(proxy_info.first, proxy_info.second);

	const std::string my_threadno = std::to_string(threadno);

	clog::info() << "Starting thread " << my_threadno;
	task *current_task = nullptr;

	for(;;) {

		do {
			if (!keep_going) goto END;
			current_task = get_task(threadno);
		} while (!current_task);
		
		clog::info() << my_threadno << ": ^^^Fetching " 
			<< current_task->get_url();
		m_kon.grab(current_task);
		clog::info() << my_threadno << ": $$$Finished " 
			<< current_task->get_url();;


		long dom = current_task->get_domain_id();

		next_hit[dom][threadno] = time(NULL) + settings[dom].interval;

		if (current_task->get_callback()) {
			std::thread(current_task->get_callback(), current_task).detach();
		} else
			delete current_task;
	}

END:
	//Remove itself from the list of threads.
	clog::info() << "Removing thread " << threadno;
	auto it = std::find(thread_ids.begin(), thread_ids.end(), threadno);
	thread_ids.erase(it);
}

void stop() {
	
	clog::info() << "Stopping Kyukon and destroying threads...";
	keep_going = false;

	while(!thread_ids.empty()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

}//namespace
