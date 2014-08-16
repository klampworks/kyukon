#include "qscheduler.hpp"
#include "task.hpp"
#include "domain_settings.hpp"
#include "cpp-log/cpp_log.hpp"


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
}


task* qscheduler::get_task(thread_id thread)
{
	std::condition_variable *cv = new std::condition_variable();
	threads[thread].cv = cv;
	std::unique_lock<std::mutex> lk(thread_m);
	cv->wait(lk);

	/* TODO, find a cooler way of doing this. */
	dom_id *dom = threads.at(thread).did;
	auto &set = domains.at(*dom);
	set->list_mutex.lock();

	task *t = set->task_list.top();	
	set->task_list.top();	


	set->list_mutex.unlock();
	threads.erase(thread);
	delete dom;
	delete cv;
	lk.unlock();

	return t;
	/*
	 * Create condition variable.
	 * add convar to thread queue
	 * wait on convar
	 */
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
