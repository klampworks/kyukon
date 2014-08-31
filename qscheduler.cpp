#include "qscheduler.hpp"
#include "task.hpp"
#include "domain_settings.hpp"
#include "cpp-log/cpp_log.hpp"
#include <chrono>

qscheduler::qscheduler() : tscheduler() {}

void qscheduler::add_task(task *t)
{
	dom_id dom = t->get_domain_id();
	domain_settings *set = nullptr;

	try {
		set = domains.at(dom);
	} catch (const std::out_of_range &e) {
		clog::err() << "Unregistered domain id " << dom;
		return;
	}

	set->list_mutex.lock();
	set->task_list.push(t);
	set->list_mutex.unlock();

	resolve();
}

void qscheduler::resolve()
{
	if (!resolve_m.try_lock())
		return;

	// Examine each thread in turn
	for (auto &thread : threads) {
		
		// Find a suitable domain for this thread
		if (dom_id dom = next_hit.next(thread.first)) {

			/* If a domain has been found, send the task to the 
			 * waiting thread. */

			/* Assume no parallel access. */
			auto *cv = thread.second.cv;

			auto &set = domains.at(dom);
			set->list_mutex.lock();

			task *t = set->task_list.top();	
			set->task_list.pop();	

			set->list_mutex.unlock();
			thread.second.t = t;

			cv->notify_one();
		}
	}

	resolve_m.unlock();
}

void qscheduler::reg_thread(thread_id thread)
{
	next_hit.add_thread(thread);
}

dom_id qscheduler::reg_dom(long interval, std::function<void()> fillup_fn)
{
	dom_id new_id = latest_dom_id++;

	domain_settings *set = new domain_settings;
	set->interval = interval;
	set->fillup = fillup_fn;

	domains[new_id] = set;

	next_hit.add_dom(new_id);

	return new_id;
}

void qscheduler::unreg_dom(dom_id)
{}

qscheduler::~qscheduler()
{
	for (auto a : domains)
		delete a.second;
}

#include <ctime>
void qscheduler::update_nh(dom_id dom, thread_id thread)
{
	next_hit.update(dom, thread, 
		time(NULL) + domains[dom]->interval);
}
