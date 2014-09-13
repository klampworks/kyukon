#include "kyukon.hpp"
#include "kon.hpp"
#include "task.hpp"
#include <algorithm>

#define CPP_LOG_INFO_COUT
#define CPP_LOG_WARN_COUT
#define CPP_LOG_INFO_COUT
#include "cpp-log/cpp_log.hpp"

#include "tscheduler.hpp"
#include "qscheduler.hpp"
namespace kyukon {

std::vector<unsigned> qthreads;
unsigned number_of_threads = 0;

qscheduler *qs;
tscheduler *ts;

void t_thread_run(unsigned);
void thread_run(const std::pair<std::string , bool> &, unsigned);
bool keep_going;

void init(const std::vector<std::pair<std::string, bool>> &proxy_info)
{
	if (!qthreads.empty()) {

		clog::warn() << "Kyukon is already running!\n" 
		"Please call kyukon::stop() to reset everything.";

		return;
	}

	qs = new qscheduler;
	ts = new tscheduler;

	keep_going = true;
	number_of_threads = proxy_info.size();

	unsigned i = 0;
	for (; i < number_of_threads; ++i) {

		std::string a = proxy_info[i].first;
		bool b = proxy_info[i].second;
		std::thread(thread_run, proxy_info[i], i).detach();
		qthreads.push_back(i);
	}

	++i;
	std::thread(t_thread_run, i).detach();
	qthreads.push_back(i);


	clog::info() << "Initialising Kyukon with " << number_of_threads 
		<< " threads.";

}

unsigned signup(int interval, std::function<void()> fillup_fn)
{
	return qs->reg_dom(interval, fillup_fn);
}

void unregister(unsigned domain_id) {}

void add_task(task *t)
{
	qs->add_task(t);
}

void set_do_fillup(bool b, unsigned domain_id)
{
	qs->set_do_fillup(b, domain_id);
}

std::mutex qthreads_m;
std::vector<thread_id> qthreads_done;

void stop() 
{
	keep_going = false;

	qs->stopp();
	ts->stopp();

	while (qthreads_done.size() != qthreads.size()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

	}
}

void thread_run(const std::pair<std::string , bool> &proxy_info, 
	unsigned threadno) 
{
	kon m_kon(proxy_info.first, proxy_info.second);

	const std::string my_threadno = std::to_string(threadno);

	clog::info() << "Starting thread " << my_threadno;
	task *current_task = nullptr;

	qs->reg_thread(threadno);

	while (keep_going) {

		std::cout << threadno << "acquiring task..." << std::endl;
		current_task = qs->get_task(threadno);

		if (!current_task) continue;

		clog::info() << my_threadno << ": ^^^Fetching " 
			<< current_task->get_url();
		m_kon.grab(current_task);
		clog::info() << my_threadno << ": $$$Finished " 
			<< current_task->get_url();;

		qs->update_nh(current_task->get_domain_id(), threadno);

		if (current_task->get_callback())
			ts->add_task(current_task);
		 else
			delete current_task;
	}

	//Remove itself from the list of threads.
	clog::info() << "Removing thread " << threadno;
	qthreads_m.lock();
	//auto it = std::find(qthreads.begin(), qthreads.end(), threadno);
	//qthreads.erase(it);
	qthreads_done.push_back(threadno);
	qthreads_m.unlock();
}

void t_thread_run(unsigned threadno)
{
	task *current_task = nullptr;

	while (keep_going) {
		current_task = ts->get_task(threadno);

		if (!current_task) continue;
		if (current_task->get_callback())
			current_task->get_callback()(current_task);

	}

	clog::info() << "Removing thread " << threadno;
	qthreads_m.lock();
	///auto it = std::find(qthreads.begin(), qthreads.end(), threadno);
	//qthreads.erase(it);
	qthreads_done.push_back(threadno);
	qthreads_m.unlock();
}

} /* namespace kyukon */
