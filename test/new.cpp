#define BOOST_TEST_MODULE example
#include <boost/test/included/unit_test.hpp>
#define protected public
#define private public
#include "../qscheduler.hpp"
#include "../task.hpp"
#include "../domain_settings.hpp"
#include "test_nh_table.hpp"
#include "test_qscheduler.hpp"
#include <chrono>
#include "test_util.hpp"


BOOST_AUTO_TEST_CASE(test_ts_add_task) 
{
	tscheduler ts;
	task *t = new task();

	BOOST_CHECK(ts.tasks.empty());

	ts.add_task(t);
	BOOST_CHECK(!ts.tasks.empty());
	BOOST_CHECK(ts.tasks.front() == t);
	delete t;
}

BOOST_AUTO_TEST_CASE(test_ts_timer_resolve) 
{
	tscheduler ts;
	task *t = new task();
	ts.add_task(t);

	thread_id thread = 1;

	task *tt = nullptr;
	std::thread thrd([&ts, &tt, thread]() {
		tt = ts.get_task(thread);});
	thrd.detach();

	std::this_thread::sleep_for(pone(ts));

	BOOST_CHECK(tt == t);

	delete t;
}
