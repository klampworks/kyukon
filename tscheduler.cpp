#include "task.hpp"
#include "tscheduler.hpp"
#include <chrono>
tscheduler::tscheduler()
{
	resolve_t = std::thread([this]() {
		while(!this->stop) {
			std::this_thread::sleep_for(resolve_t_to);
			resolve();
		}
		this->stop = false;});
	resolve_t.detach();
}

void tscheduler::add_task(task *t)
{
	tasks_m.lock();
	tasks.push(t);
	tasks_m.unlock();

	resolve();
}

void tscheduler::resolve()
{
	if (!resolve_m.try_lock())
		return;

	// Examine each thread in turn
	for (auto &thread : threads) {

		tasks_m.lock();
		if (!tasks.empty()) {

			auto *cv = thread.second.cv;

			task *t = tasks.front();	
			tasks.pop();	

			thread.second.t = t;
			cv->notify_one();
		}
		
		tasks_m.unlock();
	}

	resolve_m.unlock();
}

task* tscheduler::get_task(thread_id thread)
{
	std::condition_variable *cv = new std::condition_variable();
	threads[thread].cv = cv;
	std::unique_lock<std::mutex> lk(thread_m);
	cv->wait(lk);

	task *t = threads.at(thread).t;
	threads.erase(thread);

	delete cv;
	lk.unlock();

	return t;
	/*
	 * Create condition variable.
	 * add convar to thread queue
	 * wait on convar
	 */
}

void tscheduler::stopp()
{
	resolve_m.lock();

	for (auto &thread : threads) {

		auto *cv = thread.second.cv;
		if (cv) {
			thread.second.t = nullptr;
			cv->notify_one();
		}
	}

	resolve_m.unlock();
}

tscheduler::~tscheduler()
{
	stop = true;		
	while (stop)
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
}
